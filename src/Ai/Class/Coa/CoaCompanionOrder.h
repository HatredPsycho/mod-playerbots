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
