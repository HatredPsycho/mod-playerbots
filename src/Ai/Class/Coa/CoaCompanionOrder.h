/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_COACOMPANIONORDER_H
#define PLAYERBOTS_COACOMPANIONORDER_H

#include <algorithm>
#include <cstdint>
#include <vector>

struct CoaCompanionCandidate
{
    std::uint32_t spell = 0;
    bool inRotation = false;
    float priority = 0.0f;
    std::uint8_t lifeForceCost = 0;
    bool replacesPet = false;
};

// A raise that occupies Life Force is a companion whatever properties its spell carries: the Necromancer's contract
// marks every raise as wild, and AscensionNecromancer::Summon hands the follower properties to the creature itself.
[[nodiscard]] inline bool IsCoaCompanionSummon(std::uint8_t lifeForceCost, bool propertiesFollowOwner)
{
    return lifeForceCost > 0 || propertiesFollowOwner;
}

// A raise stays wanted while Life Force is free, however many of its minions already stand; ChooseCoaCompanion passes
// it over once it no longer fits. Any other companion is wanted only until it stands.
[[nodiscard]] inline bool IsCoaCompanionWanted(std::uint8_t lifeForceCost, bool stands)
{
    return lifeForceCost > 0 || !stands;
}

// The rotation's own order first, then whatever else the bot knows. A companion that does not fit is passed over,
// never made to fit: a raise at full Life Force evicts the oldest minion, and two raises would then evict each other
// for ever. Likewise a pet spell replaces the pet, so one standing pet ends the matter for all of them.
[[nodiscard]] inline std::uint32_t ChooseCoaCompanion(std::vector<CoaCompanionCandidate> candidates,
                                                      std::uint8_t freeLifeForce, bool hasPet)
{
    std::stable_sort(candidates.begin(), candidates.end(),
                     [](CoaCompanionCandidate const& a, CoaCompanionCandidate const& b)
                     {
                         if (a.inRotation != b.inRotation)
                             return a.inRotation;
                         return a.inRotation && a.priority > b.priority;
                     });

    for (CoaCompanionCandidate const& candidate : candidates)
    {
        if (candidate.lifeForceCost > freeLifeForce || (candidate.replacesPet && hasPet))
            continue;
        return candidate.spell;
    }

    return 0;
}

#endif
