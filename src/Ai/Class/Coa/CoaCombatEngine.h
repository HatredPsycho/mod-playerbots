/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license.
 */

#ifndef _PLAYERBOT_COACOMBATENGINE_H
#define _PLAYERBOT_COACOMBATENGINE_H

#include "Define.h"
#include <vector>

class Player;
class Unit;
class SpellInfo;

/*
 * A scoring layer over the CoA ability picker, for group content.
 *
 * The plain CoA rotation asks "which of my abilities of this kind can I cast right now" and takes
 * the first that works (CoaAiObjectContext's CastFirst). That holds up solo and never breaks when a
 * class is reworked, because nothing is written down per class. In a dungeon it is too little: the
 * order is fixed, so a bot spends a two minute cooldown on a single trash mob, keeps hitting one
 * target while four stand next to it, and heals whoever its trigger named rather than whoever is
 * about to die.
 *
 * This engine does not replace that picker. It takes the same candidate list and puts it in a
 * different order, scoring each ability against a snapshot of the fight taken once per tick. The
 * caller then casts the first one that works, exactly as before, so benching, telemetry and every
 * failure path stay where they are. If the engine has nothing to say it leaves the order alone and
 * the bot behaves as it does today.
 *
 * It is on only in group content, because that is where the ordering pays and where a wrong call
 * is cheap: a solo bot killing a boar does not need to husband its cooldowns.
 */
namespace CoaCombatEngine
{
    enum Mode : uint8
    {
        MODE_OFF    = 0,
        MODE_GROUP  = 1,  // the default: group content only
        MODE_ALWAYS = 2   // for measuring; ignores the group gate
    };

    enum Role : uint8
    {
        ROLE_DPS,
        ROLE_HEAL,
        ROLE_TANK
    };

    /*
     * What the engine is allowed to know about an ability. Deliberately not the picker's own
     * Usable, and deliberately not its KIND_* bitmask either: those bits live in an anonymous
     * namespace, and mirroring them here would be two definitions of one thing, free to drift
     * apart. The caller translates them into these flags once, so the engine reads intent rather
     * than bit positions.
     */
    struct Candidate
    {
        SpellInfo const* info = nullptr;

        bool damage = false;
        bool aoe = false;
        bool heal = false;
        bool groupHeal = false;
        bool hot = false;
        bool taunt = false;
        bool defensive = false;
        bool control = false;

        float score = 0.0f;
    };

    /*
     * One reading of the fight, taken before the candidates are scored so that every score sees the
     * same state and no ability pays for a second pass over the group.
     */
    struct Snapshot
    {
        Player* bot = nullptr;
        Unit* victim = nullptr;

        float healthPct = 100.0f;
        float powerPct = 100.0f;
        bool moving = false;

        Player* lowestAlly = nullptr;
        float lowestAllyPct = 100.0f;
        Player* tankAlly = nullptr;
        float tankAllyPct = 100.0f;
        uint8 aliveAllies = 0;
        uint8 injuredAllies = 0;   // below 80%
        uint8 criticalAllies = 0;  // below 40%

        float victimHealthPct = 100.0f;
        uint8 enemiesNearVictim = 0;
        bool victimOnNonTank = false;
        bool bossFight = false;
    };

    /// AiPlayerbot.CoaCombatEngine, read once.
    Mode ConfiguredMode();

    /// Whether the engine orders this bot's abilities at all.
    bool Active(Player* bot);

    /// The bot's role, from the strategies it carries rather than from its class.
    Role RoleOf(Player* bot, bool tank, bool heal);

    /*
     * `enemiesInFight` is how many enemies are on the group, which the caller already has as the
     * "attackers count" value. The engine does not go looking for it: a grid search once per bot per
     * tick is the kind of cost that only shows up at five hundred bots.
     */
    Snapshot Take(Player* bot, Unit* victim, uint8 enemiesInFight);

    /*
     * Scores every candidate against the snapshot and sorts them, best first. A stable sort, so
     * abilities the engine cannot tell apart keep the picker's own order - that order is by
     * required level, which is a reasonable tiebreak and keeps today's behaviour visible underneath.
     */
    void Order(Snapshot const& snapshot, Role role, std::vector<Candidate>& candidates);
}

#endif
