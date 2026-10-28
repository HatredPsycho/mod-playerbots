/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "VaultsStrategy.h"
#include "VaultsMultipliers.h"

void CoaDungeonVaultsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // The Echo's chain zones and Spectral Strike lines, Darkandle's fire circles and Earthquake, the Deceiver's
    // Voidflare and Shadowy Rings, and the trash casts that hit around a spot (Dark Wax Explosion, Psychic
    // Scream, Shadowfury).
    triggers.push_back(new TriggerNode("vaults in hazard", {
        NextAction("vaults leave hazard", ACTION_EMERGENCY + 10) }));

    // The Deceiver: one player stands in the soak circle, the Holy Crystal gets clicked and the whole group
    // takes Void Wrath in its shelter.
    triggers.push_back(new TriggerNode("vaults soak", {
        NextAction("vaults soak", ACTION_EMERGENCY + 6) }));
    triggers.push_back(new TriggerNode("vaults shelter", {
        NextAction("vaults take shelter", ACTION_EMERGENCY + 5) }));
    triggers.push_back(new TriggerNode("vaults holy crystal", {
        NextAction("vaults click crystal", ACTION_EMERGENCY + 4) }));

    // Before the fight the three Holy Crystals that seal him are broken.
    triggers.push_back(new TriggerNode("vaults seal crystal", {
        NextAction("vaults break seal", ACTION_EMERGENCY + 1) }));
}

void CoaDungeonVaultsStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new VaultsHazardMultiplier(botAI));
}
