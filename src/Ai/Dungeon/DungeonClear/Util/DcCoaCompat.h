/*
 * mod-dungeon-clear — Conquest of Azeroth (CoA) compatibility.
 *
 * CoA's 21 custom classes (ids 12-32) are not in the module's per-class tables:
 * their tank specializations have no stock pull opener, their healers no stock
 * resurrect class, and CoA reset many vanilla instance doors to lock 0. These
 * helpers derive the same answers from the spells a character actually knows and
 * from the realm configuration, so the stock behaviour stays untouched.
 */

#pragma once

#include "CoaAbilities.h"
#include "AscensionSpecialization.h"
#include "Config.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

#include <cstdint>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>

namespace DcCoaCompat
{
    inline bool IsCoaClass(Player const* player)
    {
        return player && IsAscensionCustomClassId(player->getClass());
    }

    struct PullOpener
    {
        uint32 spellId;
        float minRange;
        float maxRange;
    };

    // The best ranged single-target opener a CoA tank knows: an instant taunt first,
    // then an instant damaging spell, the longer reach and the shorter cooldown
    // breaking ties. Spells on cooldown are skipped so the next pack still gets one.
    // Which spells taunt or damage comes from playerbots' CoA class kit (CoaAbilities).
    inline std::optional<PullOpener> ResolvePullOpener(Player* bot)
    {
        if (!IsCoaClass(bot))
            return std::nullopt;

        constexpr float MinimumReach = 20.0f;
        constexpr uint32 LongestCooldownMs = 30000;

        std::optional<PullOpener> best;
        int bestScore = 0;
        uint32 bestCooldown = 0;
        auto consider = [&](SpellInfo const* info, int score)
        {
            if (!info->NeedsExplicitUnitTarget() || info->IsChanneled() || info->CalcCastTime() > 0)
                return;
            uint32 const cooldown = std::max(info->RecoveryTime, info->CategoryRecoveryTime);
            if (cooldown > LongestCooldownMs || bot->HasSpellCooldown(info->Id))
                return;
            float const maxRange = info->GetMaxRange(false);
            if (maxRange < MinimumReach)
                return;
            bool const better = !best || score > bestScore ||
                (score == bestScore && (maxRange > best->maxRange ||
                                        (maxRange == best->maxRange && cooldown < bestCooldown)));
            if (better)
            {
                best = PullOpener{info->Id, std::max(info->GetMinRange(false), 8.0f), maxRange};
                bestScore = score;
                bestCooldown = cooldown;
            }
        };
        for (SpellInfo const* info : CoaAbilities::KnownSingleTargetTaunts(bot))
            consider(info, 2);
        for (SpellInfo const* info : CoaAbilities::KnownSingleTargetAttacks(bot))
            consider(info, 1);
        return best;
    }

    // Whether a CoA character can bring a dead ally back: its class kit has a resurrect it knows.
    inline bool CanResurrect(Player const* player)
    {
        return IsCoaClass(player) && CoaAbilities::CanResurrect(const_cast<Player*>(player));
    }

    // DungeonClear.LockFreeClickableDoors: comma-separated gameobject entries the
    // realm vouches for as plain click doors although their lock is 0 (CoA PR #6477
    // reset 74 vanilla instance doors that way). Re-read when the value changes, so
    // .reload config applies it.
    inline bool IsConfiguredLockFreeDoor(uint32 goEntry)
    {
        static std::mutex lock;
        static std::string parsedFrom;
        static std::unordered_set<uint32> entries;

        std::string const value =
            sConfigMgr->GetOption<std::string>("DungeonClear.LockFreeClickableDoors", "17153,17154", false);
        std::lock_guard<std::mutex> guard(lock);
        if (value != parsedFrom)
        {
            entries.clear();
            std::stringstream stream(value);
            std::string item;
            while (std::getline(stream, item, ','))
            {
                try
                {
                    if (item.find_first_not_of(" \t") != std::string::npos)
                        entries.insert(static_cast<uint32>(std::stoul(item)));
                }
                catch (...)
                {
                }
            }
            parsedFrom = value;
        }
        return entries.count(goEntry) != 0;
    }
}
