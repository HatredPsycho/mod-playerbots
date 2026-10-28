/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CoaMissionAction.h"

#include "CoaMissionMgr.h"
#include "DBCStores.h"
#include "Event.h"
#include "GameObject.h"
#include "LFGMgr.h"
#include "LootObjectStack.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "Timer.h"
#include "TravelMgr.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace
{
// CoA ruleset aura (core: AscensionRulesets.cpp): a lone gatherer would be killed again and again.
constexpr uint32 MISSION_HIGH_RISK_AURA = 1004019;

// The picks LootObject::IsLootPossible accepts for a mining node.
bool HasMiningPick(Player* bot)
{
    for (uint32 pick : {756u, 778u, 1819u, 1893u, 1959u, 2901u, 9465u, 20723u, 40772u, 40892u, 40893u})
        if (bot->HasItemCount(pick, 1))
            return true;
    return false;
}

uint32 SkillOf(Player* bot, uint32 skillId) { return bot->HasSkill(skillId) ? bot->GetSkillValue(skillId) : 0; }

std::string ZoneNameOf(uint32 zoneId)
{
    AreaTableEntry const* zone = sAreaTableStore.LookupEntry(zoneId);
    std::string const name = zone ? PlayerbotAI::GetLocalizedAreaName(zone) : "";
    return name.empty() ? "?" : name;
}

constexpr uint32 PROGRESS_CHECK_MS = 3 * MINUTE * IN_MILLISECONDS;
constexpr uint32 NODE_GIVE_UP_MS = MINUTE * IN_MILLISECONDS;
constexpr uint32 NO_NODE_MS = 10 * MINUTE * IN_MILLISECONDS;
constexpr uint32 SAVE_EVERY_MS = MINUTE * IN_MILLISECONDS;
constexpr uint32 MAX_FAILS = 3;
}  // namespace

bool CoaMissionAction::Execute(Event event)
{
    Player* requester = event.getOwner();

    std::string param = event.getParam();
    std::transform(param.begin(), param.end(), param.begin(), [](unsigned char c) { return std::tolower(c); });
    std::istringstream in(param);
    std::string word;
    in >> word;

    // Any other word stays unanswered: "mission" also starts sentences said in party chat.
    if (!word.empty() && word != "status" && word != "stop" && word != "gather" && word != "help")
        return true;

    if (!sPlayerbotAIConfig.missionsEnable)
    {
        botAI->TellMasterNoFacing("Mission refused: missions are disabled on this server.");
        return true;
    }
    // Other bots of the group answer only a real request, not a status asked to the whole party.
    if (!CoaMissionMgr::IsMissionBot(bot, requester))
    {
        if (word == "gather")
            botAI->TellMasterNoFacing("Mission refused: missions are only for your own characters, asked by you.");
        return true;
    }

    if (word.empty() || word == "status")
    {
        botAI->TellMasterNoFacing(sCoaMissionMgr.StatusLine(bot));
        return true;
    }

    if (word == "stop")
    {
        if (sCoaMissionMgr.IsOnMission(bot))
            sCoaMissionMgr.Finish(bot, "stopped by you");
        else
            botAI->TellMasterNoFacing(sCoaMissionMgr.StatusLine(bot));
        return true;
    }

    if (word == "help")
    {
        botAI->TellMasterNoFacing("Usage: mission gather [minutes] [mining|herbalism], mission status, mission stop");
        return true;
    }

    uint32 minutes = 0;
    uint32 skillId = 0;
    std::string token;
    while (in >> token)
    {
        if (!token.empty() && std::all_of(token.begin(), token.end(), [](unsigned char c) { return std::isdigit(c); }))
            minutes = token.size() > 5 ? 99999 : uint32(std::stoul(token));
        else if (token == "mining" || token == "mine" || token == "ore")
            skillId = SKILL_MINING;
        else if (token == "herbalism" || token == "herb" || token == "herbs")
            skillId = SKILL_HERBALISM;
    }

    std::string error;
    if (!Start(requester, minutes, skillId, error))
        botAI->TellMasterNoFacing("Mission refused: " + error + ".");
    return true;
}

bool CoaMissionAction::Start(Player* requester, uint32 minutes, uint32 skillId, std::string& error)
{
    if (sCoaMissionMgr.IsOnMission(bot))
    {
        error = "I am already on a mission (whisper \"mission stop\" first)";
        return false;
    }
    if (!bot->IsAlive())
        error = "I am dead";
    else if (bot->IsInCombat())
        error = "I am in combat";
    else if (bot->IsInFlight())
        error = "I am on a flight";
    else if (!bot->GetMap() || bot->GetMap()->Instanceable())
        error = "I am in an instance";
    else if (bot->InBattleground() || bot->InArena() || bot->InBattlegroundQueue())
        error = "I am in a battleground or its queue";
    else if (sLFGMgr->GetState(bot->GetGUID()) != lfg::LFG_STATE_NONE)
        error = "I am in the dungeon finder";
    else if (bot->HasAura(MISSION_HIGH_RISK_AURA))
        error = "I am in High Risk";
    if (!error.empty())
        return false;

    // The profession: the one asked, else the best of mining (with a pick) and herbalism. The professions of CoA's
    // own (woodcutting...) are not gathered.
    bool const pick = HasMiningPick(bot);
    uint32 const mining = SkillOf(bot, SKILL_MINING);
    uint32 const herbalism = SkillOf(bot, SKILL_HERBALISM);
    if (skillId == SKILL_MINING && !mining)
        error = "I do not know mining";
    else if (skillId == SKILL_MINING && !pick)
        error = "I have no mining pick";
    else if (skillId == SKILL_HERBALISM && !herbalism)
        error = "I do not know herbalism";
    else if (!skillId)
    {
        bool const canMine = mining && pick;
        if (canMine && (!herbalism || mining >= herbalism))
            skillId = SKILL_MINING;
        else if (herbalism)
            skillId = SKILL_HERBALISM;
        else
            error = mining ? "I have no mining pick" : "I know neither mining nor herbalism";
    }
    if (!error.empty())
        return false;

    uint32 const skillValue = SkillOf(bot, skillId);
    TravelMgr::GatherCell const* cell = sTravelMgr.SelectGatherCell(bot, skillId, skillValue);
    if (!cell)
    {
        error = "I know no " + CoaMissionMgr::SkillName(skillId) + " spot for my level and skill on this continent";
        return false;
    }

    if (!minutes)
        minutes = sPlayerbotAIConfig.missionsDefaultMinutes;
    minutes = std::min(minutes, sPlayerbotAIConfig.missionsMaxMinutes);

    CoaMission mission;
    mission.owner = requester->GetGUID().GetCounter();
    mission.skillId = skillId;
    mission.state = CoaMissionState::Travel;
    mission.remainingMs = int32(minutes * MINUTE * IN_MILLISECONDS);
    mission.totalMinutes = minutes;
    mission.mapId = cell->mapId;
    mission.x = cell->x;
    mission.y = cell->y;
    mission.z = cell->z;
    mission.zoneId = cell->zoneId;
    mission.cell = cell;
    mission.prevFlags = CoaMissionMgr::ReadStrategyFlags(botAI);

    if (!sCoaMissionMgr.Begin(bot, mission, error))
        return false;

    // Out of the group: neither Dungeon Clear nor the dungeon finder takes a bot that is not in it. The alt keeps its
    // master all the same.
    botAI->LeaveOrDisbandGroup();
    botAI->ChangeStrategy("-follow,-stay,-lfg,+loot,+gather,+gather all,+mission", BOT_STATE_NON_COMBAT);
    sCoaMissionMgr.Save(bot);

    botAI->TellMasterNoFacing("Mission started: " + CoaMissionMgr::SkillName(skillId) + " in " +
                              ZoneNameOf(cell->zoneId) + " for " + std::to_string(minutes) +
                              " min. I will mail you what I gather. Whisper \"mission status\" or \"mission stop\".");
    return true;
}

bool CoaMissionUpdateAction::isUseful() { return bot->IsAlive() && sCoaMissionMgr.IsOnMission(bot); }

bool CoaMissionUpdateAction::Execute(Event /*event*/)
{
    CoaMission mission;
    if (!sCoaMissionMgr.Get(bot, mission))
        return false;

    uint32 const now = getMSTime();
    if (mission.lastTickMs)
        mission.remainingMs -= int32(std::min<uint32>(getMSTimeDiff(mission.lastTickMs, now), 10 * MINUTE * IN_MILLISECONDS));
    mission.lastTickMs = now;
    if (!mission.lastSaveMs)
        mission.lastSaveMs = now;

    // Safety: only for the alt of the real player who gave the mission, only in the open world.
    Player* master = botAI->GetMaster();
    if (!sPlayerbotAIConfig.missionsEnable)
    {
        sCoaMissionMgr.Finish(bot, "missions are disabled on this server");
        return true;
    }
    if (!master || !CoaMissionMgr::IsMissionBot(bot, master) || master->GetGUID().GetCounter() != mission.owner)
    {
        sCoaMissionMgr.Finish(bot, "my owner changed");
        return true;
    }
    if (!bot->GetMap() || bot->GetMap()->Instanceable() || bot->InBattleground() || bot->InArena())
    {
        sCoaMissionMgr.Finish(bot, "I entered an instance");
        return true;
    }

    // Invited back meanwhile, or put back at login: out again, at most every 5 s.
    if (bot->GetGroup() && (!mission.lastGroupLeaveMs || getMSTimeDiff(mission.lastGroupLeaveMs, now) > 5000))
    {
        botAI->LeaveOrDisbandGroup();
        mission.lastGroupLeaveMs = now;
    }
    if (botAI->HasStrategy("follow", BOT_STATE_NON_COMBAT) || botAI->HasStrategy("stay", BOT_STATE_NON_COMBAT))
        botAI->ChangeStrategy("-follow,-stay", BOT_STATE_NON_COMBAT);

    if (!mission.cell)
        mission.cell = sTravelMgr.FindGatherCell(mission.mapId, mission.skillId, mission.x, mission.y);

    if (mission.state != CoaMissionState::Deliver)
    {
        if (mission.remainingMs <= 0)
        {
            mission.state = CoaMissionState::Deliver;
            mission.endReason = "the time is up";
        }
        else if (!bot->GetFreeInventorySpace())
        {
            mission.state = CoaMissionState::Deliver;
            mission.endReason = "my bags are full";
        }
        else if (!mission.cell)
        {
            mission.state = CoaMissionState::Deliver;
            mission.endReason = "my gathering spot is gone";
        }
    }

    if (mission.state == CoaMissionState::Deliver)
    {
        std::string const reason = mission.endReason.empty() ? "it is over" : mission.endReason;
        sCoaMissionMgr.Put(bot, mission);
        sCoaMissionMgr.Finish(bot, reason);
        return true;
    }

    CoaMissionState const before = mission.state;
    bool const acted = mission.state == CoaMissionState::Travel ? DoTravel(mission, now) : DoWork(mission, now);

    bool const save = mission.state != before || getMSTimeDiff(mission.lastSaveMs, now) >= SAVE_EVERY_MS;
    if (save)
        mission.lastSaveMs = now;
    sCoaMissionMgr.Put(bot, mission);
    if (save)
        sCoaMissionMgr.Save(bot);
    return acted;
}

void CoaMissionUpdateAction::TeleportToSpot(CoaMission const& mission)
{
    bot->StopMoving();
    bot->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TELEPORTED | AURA_INTERRUPT_FLAG_CHANGE_MAP);
    bot->TeleportTo(mission.mapId, mission.x, mission.y, mission.z + 0.5f, bot->GetOrientation());
}

bool CoaMissionUpdateAction::DoTravel(CoaMission& mission, uint32 now)
{
    if (bot->IsInFlight())
        return false;

    // The spot is always on the bot's continent; after a login elsewhere only a teleport gets it there.
    if (bot->GetMapId() != mission.mapId)
    {
        if (!sPlayerbotAIConfig.missionsTeleportFallback)
        {
            mission.state = CoaMissionState::Deliver;
            mission.endReason = "my gathering spot is on another continent";
            return false;
        }
        TeleportToSpot(mission);
        return true;
    }

    float const dist = bot->GetExactDist2d(mission.x, mission.y);
    // As when an alt is summoned: straight to the gathering spot, the mission time is spent gathering.
    if (dist >= 30.0f && sPlayerbotAIConfig.missionsTeleportToSpot)
    {
        TeleportToSpot(mission);
        return true;
    }
    // Arrived: near the spot, or already in the mission's zone (an alt teleported there by its player gathers there).
    if (dist < 30.0f || (mission.zoneId && bot->GetZoneId() == mission.zoneId))
    {
        mission.state = CoaMissionState::Work;
        mission.workSinceMs = now;
        mission.progressCheckMs = 0;
        return false;
    }

    // No 20 yards gained in 3 minutes: a failure. Teleport if allowed, give up after the third.
    if (!mission.progressCheckMs)
    {
        mission.progressCheckMs = now;
        mission.progressDist = dist;
    }
    else if (getMSTimeDiff(mission.progressCheckMs, now) >= PROGRESS_CHECK_MS)
    {
        if (dist + 20.0f > mission.progressDist)
        {
            if (++mission.fails >= MAX_FAILS)
            {
                mission.state = CoaMissionState::Deliver;
                mission.endReason = "I could not reach my gathering spot";
                return false;
            }
            if (sPlayerbotAIConfig.missionsTeleportFallback)
            {
                mission.progressCheckMs = 0;
                TeleportToSpot(mission);
                return true;
            }
        }
        mission.progressCheckMs = now;
        mission.progressDist = dist;
    }

    return MoveFarTo(WorldPosition(mission.mapId, mission.x, mission.y, mission.z));
}

bool CoaMissionUpdateAction::DoWork(CoaMission& mission, uint32 now)
{
    TravelMgr::GatherCell const* cell = mission.cell;
    if (!cell || cell->points.empty())
        return false;

    // The loot strategy is on a node: let it gather.
    if (bot->IsNonMeleeSpellCast(false) || bot->GetLootGUID())
        return false;
    LootObject target = AI_VALUE(LootObject, "loot target");
    if (!target.IsEmpty() && target.IsLootPossible(bot))
        return false;

    // The nearest node of the mission's profession in sight.
    GameObject* best = nullptr;
    float bestDist = 0.0f;
    for (ObjectGuid const& guid : AI_VALUE(GuidVector, "nearest game objects no los"))
    {
        if (!guid.IsGameObject() || mission.skippedNodes.count(guid))
            continue;
        GameObject* go = botAI->GetGameObject(guid);
        if (!go)
            continue;
        LootObject node(bot, guid);
        if (node.IsEmpty() || node.skillId != mission.skillId || !node.IsLootPossible(bot))
            continue;
        float const dist = bot->GetExactDist(go);
        if (!best || dist < bestDist)
        {
            best = go;
            bestDist = dist;
        }
    }

    if (best)
    {
        // A node it cannot reach in a minute (under water, up a cliff) is left for good.
        if (best->GetGUID() != mission.node)
        {
            mission.node = best->GetGUID();
            mission.nodeSinceMs = now;
        }
        else if (getMSTimeDiff(mission.nodeSinceMs, now) >= NODE_GIVE_UP_MS)
        {
            if (mission.skippedNodes.size() >= 50)
                mission.skippedNodes.clear();
            mission.skippedNodes.insert(mission.node);
            mission.node.Clear();
            return false;
        }

        if (bestDist <= sPlayerbotAIConfig.lootDistance)
            AI_VALUE(LootObjectStack*, "available loot")->Add(best->GetGUID());
        if (bestDist > INTERACTION_DISTANCE - 2.0f)
            return MoveNear(best, 2.0f);
        return false;
    }

    // Nothing gathered for 10 minutes: another spot, a failure.
    uint32 const since = std::max(mission.lastLootMs, mission.workSinceMs);
    if (since && getMSTimeDiff(since, now) >= NO_NODE_MS)
    {
        TravelMgr::GatherCell const* next =
            ++mission.fails < MAX_FAILS
                ? sTravelMgr.SelectGatherCell(bot, mission.skillId, SkillOf(bot, mission.skillId), cell)
                : nullptr;
        if (!next)
        {
            mission.state = CoaMissionState::Deliver;
            mission.endReason = "I found nothing more to gather";
            return false;
        }
        mission.cell = next;
        mission.mapId = next->mapId;
        mission.x = next->x;
        mission.y = next->y;
        mission.z = next->z;
        mission.zoneId = next->zoneId;
        mission.point = 0;
        mission.state = CoaMissionState::Travel;
        mission.progressCheckMs = 0;
        mission.workSinceMs = 0;
        mission.skippedNodes.clear();
        return false;
    }

    // Walk from spawn point to spawn point: only part of them is up at a time.
    if (mission.point >= cell->points.size())
        mission.point = 0;
    TravelMgr::GatherPoint const& point = cell->points[mission.point];
    if (bot->GetExactDist(point.x, point.y, point.z) < 8.0f)
    {
        mission.point = (mission.point + 1) % cell->points.size();
        return false;
    }
    return MoveFarTo(WorldPosition(cell->mapId, point.x, point.y, point.z));
}
