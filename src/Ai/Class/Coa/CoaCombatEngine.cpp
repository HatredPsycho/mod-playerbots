/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license.
 */

#include "CoaCombatEngine.h"

#include "Config.h"
#include "Group.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Unit.h"

#include <algorithm>
#include <cmath>

namespace
{
    // Below this an ally counts as injured, below the second as critical. The same two steps the
    // group-heal triggers already use, so the engine and the triggers agree on what a hurt group is.
    constexpr float InjuredPct = 80.0f;
    constexpr float CriticalPct = 40.0f;

    // Three enemies around the target is the point where hitting all of them beats hitting one. Two
    // is the tank's threshold instead: holding a pair is already work.
    constexpr uint8 AoeEnemies = 3;
    constexpr uint8 TankAoeEnemies = 2;

    // A cooldown longer than this is worth saving for something that will live long enough to take
    // the whole of it.
    constexpr uint32 LongCooldownMs = 30000;

    // How much health a single enemy must still have before a long cooldown is spent on it, as a
    // share of the bot's own maximum: a lone trash mob is not worth a two minute ability.
    constexpr float CooldownWorthHealthPct = 55.0f;

    float Percent(uint32 current, uint32 maximum)
    {
        return maximum ? 100.0f * float(current) / float(maximum) : 0.0f;
    }

    uint32 CooldownOf(Player* bot, SpellInfo const* info)
    {
        if (!info)
            return 0;
        uint32 const recovery = std::max(info->RecoveryTime, info->CategoryRecoveryTime);
        return bot->HasSpellCooldown(info->Id) ? 0 : recovery;
    }
}

namespace CoaCombatEngine
{

Mode ConfiguredMode()
{
    // Read once: the value is a realm setting, and this runs inside the rotation.
    static Mode const mode = [] {
        int32 const value = sConfigMgr->GetOption<int32>("AiPlayerbot.CoaCombatEngine", int32(MODE_GROUP));
        if (value <= int32(MODE_OFF))
            return MODE_OFF;
        if (value >= int32(MODE_ALWAYS))
            return MODE_ALWAYS;
        return MODE_GROUP;
    }();
    return mode;
}

bool Active(Player* bot)
{
    Mode const mode = ConfiguredMode();
    if (mode == MODE_OFF || !bot)
        return false;
    if (mode == MODE_ALWAYS)
        return true;

    // Group content: the bot is in a group that has somebody else in it. Dungeons and raids are
    // covered by that on their own, and so is a party out in the world, which is where a bot
    // standing next to a player is judged.
    Group* group = bot->GetGroup();
    return group && group->GetMembersCount() > 1;
}

Role RoleOf(Player* /*bot*/, bool tank, bool heal)
{
    if (tank)
        return ROLE_TANK;
    if (heal)
        return ROLE_HEAL;
    return ROLE_DPS;
}

Snapshot Take(Player* bot, Unit* victim, uint8 enemiesInFight)
{
    Snapshot snapshot;
    snapshot.bot = bot;
    snapshot.victim = victim;

    if (!bot)
        return snapshot;

    snapshot.healthPct = bot->GetHealthPct();
    Powers const power = bot->getPowerType();
    snapshot.powerPct = Percent(bot->GetPower(power), bot->GetMaxPower(power));
    snapshot.moving = bot->isMoving();

    if (victim)
    {
        snapshot.victimHealthPct = victim->GetHealthPct();
        snapshot.enemiesNearVictim = std::max<uint8>(enemiesInFight, 1);

        Creature const* creature = victim->ToCreature();
        snapshot.bossFight = creature && (creature->isWorldBoss() || creature->isElite());

        if (Unit* target = victim->GetVictim())
            snapshot.victimOnNonTank = target->IsPlayer() && target->GetHealthPct() > 0.0f;
    }

    Group* group = bot->GetGroup();
    if (!group)
    {
        snapshot.lowestAlly = bot;
        snapshot.lowestAllyPct = snapshot.healthPct;
        snapshot.aliveAllies = 1;
        if (snapshot.healthPct < InjuredPct)
            ++snapshot.injuredAllies;
        if (snapshot.healthPct < CriticalPct)
            ++snapshot.criticalAllies;
        return snapshot;
    }

    for (GroupReference* reference = group->GetFirstMember(); reference; reference = reference->next())
    {
        Player* member = reference->GetSource();
        if (!member || !member->IsAlive() || !member->IsInWorld())
            continue;
        if (member != bot && !member->IsWithinDistInMap(bot, 60.0f))
            continue;

        ++snapshot.aliveAllies;
        float const pct = member->GetHealthPct();
        if (pct < InjuredPct)
            ++snapshot.injuredAllies;
        if (pct < CriticalPct)
            ++snapshot.criticalAllies;

        if (!snapshot.lowestAlly || pct < snapshot.lowestAllyPct)
        {
            snapshot.lowestAlly = member;
            snapshot.lowestAllyPct = pct;
        }

        // The group's tank is whoever the mob is actually hitting, not whoever is labelled one: a bot
        // reading the label would heal the wrong player whenever a pull goes wrong, which is exactly
        // when it matters.
        if (victim && victim->GetVictim() == member)
        {
            snapshot.tankAlly = member;
            snapshot.tankAllyPct = pct;
        }
    }

    if (!snapshot.lowestAlly)
    {
        snapshot.lowestAlly = bot;
        snapshot.lowestAllyPct = snapshot.healthPct;
    }
    if (!snapshot.tankAlly)
    {
        snapshot.tankAlly = snapshot.lowestAlly;
        snapshot.tankAllyPct = snapshot.lowestAllyPct;
    }

    return snapshot;
}

namespace
{
    /*
     * Damage dealing. Three questions decide the order: is this a pack, is the target worth a
     * cooldown, and can the cast finish.
     */
    float ScoreDps(Snapshot const& snapshot, Candidate const& candidate)
    {
        float score = 0.0f;
        SpellInfo const* info = candidate.info;

        if (candidate.aoe)
        {
            // Worth its cost from three enemies on, and progressively more beyond that. Below that
            // an area ability is a single-target ability with a worse coefficient.
            if (snapshot.enemiesNearVictim >= AoeEnemies)
                score += 30.0f + 6.0f * float(snapshot.enemiesNearVictim - AoeEnemies);
            else
                score -= 25.0f;
        }
        else if (candidate.damage)
        {
            score += 10.0f;
            // With a pack up, single-target damage gives way to the area abilities rather than
            // competing with them.
            if (snapshot.enemiesNearVictim >= AoeEnemies)
                score -= 12.0f;
        }

        if (candidate.control)
            score -= 40.0f;  // never part of a rotation; the control triggers own it

        uint32 const cooldown = CooldownOf(snapshot.bot, info);
        if (cooldown >= LongCooldownMs)
        {
            // Spend it on something that will be alive long enough to take it: a boss, a pack, or a
            // single target still healthy enough to be a real fight.
            bool const worth = snapshot.bossFight || snapshot.enemiesNearVictim >= AoeEnemies ||
                               snapshot.victimHealthPct >= CooldownWorthHealthPct;
            score += worth ? 18.0f : -35.0f;
        }
        else if (cooldown > 0)
        {
            // A short cooldown used as it comes up is what carries a rotation's damage.
            score += 8.0f;
        }

        if (info)
        {
            // A cast the bot will walk out of is a cast wasted. The movement triggers may well move
            // it next tick, so a long cast only wins while standing still.
            uint32 const castTime = info->CalcCastTime();
            if (castTime > 0 && snapshot.moving)
                score -= 20.0f;

            // Low on power: let the cheap abilities through, or the bot spends its bar on one cast
            // and then stands there.
            if (snapshot.powerPct < 25.0f && info->ManaCost > 0)
                score -= 10.0f;
        }

        return score;
    }

    /*
     * Healing. Urgency of the worst-off member decides the shape of the heal: direct when somebody
     * is about to die, group when several are hurt, a heal over time only when nothing is urgent.
     */
    float ScoreHeal(Snapshot const& snapshot, Candidate const& candidate)
    {
        float score = 0.0f;
        bool const emergency = snapshot.lowestAllyPct < CriticalPct;
        bool const spread = snapshot.criticalAllies >= 3 || snapshot.injuredAllies >= 4;

        if (candidate.groupHeal)
            score += spread ? 35.0f : -20.0f;
        else if (candidate.heal)
            score += emergency ? 30.0f : 12.0f;
        else if (candidate.hot)
            // A heal over time placed while somebody is dying is health that arrives too late.
            score += emergency ? -25.0f : 18.0f;
        else if (candidate.damage)
            // A healer attacks only when the group is whole.
            score += snapshot.injuredAllies ? -30.0f : 4.0f;

        if (candidate.info)
        {
            uint32 const castTime = candidate.info->CalcCastTime();
            if (emergency && castTime > 2000)
                score -= 15.0f;  // too slow for the member that is dropping now
            if (snapshot.powerPct < 20.0f && candidate.info->ManaCost > 0)
                score -= 12.0f;
        }

        return score;
    }

    /*
     * Tanking. Holding what is already on the group comes before damage, and the pack threshold is
     * lower than a damage dealer's because threat on two is already a job.
     */
    float ScoreTank(Snapshot const& snapshot, Candidate const& candidate)
    {
        float score = 0.0f;

        if (candidate.taunt)
            score += snapshot.victimOnNonTank ? 45.0f : -50.0f;

        if (candidate.aoe)
            score += snapshot.enemiesNearVictim >= TankAoeEnemies
                         ? 28.0f + 5.0f * float(snapshot.enemiesNearVictim - TankAoeEnemies)
                         : -15.0f;
        else if (candidate.damage)
            score += 10.0f;

        if (candidate.defensive)
            // Mitigation is worth its slot from the moment the tank is actually being hit down.
            score += snapshot.healthPct < 65.0f ? 32.0f : -30.0f;

        if (candidate.control)
            score -= 40.0f;

        if (candidate.info && snapshot.moving && candidate.info->CalcCastTime() > 0)
            score -= 20.0f;

        return score;
    }
}

void Order(Snapshot const& snapshot, Role role, std::vector<Candidate>& candidates)
{
    if (candidates.size() < 2)
        return;

    for (Candidate& candidate : candidates)
    {
        switch (role)
        {
            case ROLE_HEAL:
                candidate.score = ScoreHeal(snapshot, candidate);
                break;
            case ROLE_TANK:
                candidate.score = ScoreTank(snapshot, candidate);
                break;
            default:
                candidate.score = ScoreDps(snapshot, candidate);
                break;
        }
    }

    // Stable, so abilities the engine scores alike keep the picker's own order - by required level,
    // which leaves today's behaviour showing through underneath wherever the engine is indifferent.
    std::stable_sort(candidates.begin(), candidates.end(),
                     [](Candidate const& left, Candidate const& right) { return left.score > right.score; });
}

}
