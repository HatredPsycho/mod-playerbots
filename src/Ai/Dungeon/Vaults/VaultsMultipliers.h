/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_VAULTSMULTIPLIERS_H
#define PLAYERBOTS_VAULTSMULTIPLIERS_H

#include "Multiplier.h"

class VaultsHazardMultiplier : public Multiplier
{
public:
    VaultsHazardMultiplier(PlayerbotAI* botAI) : Multiplier(botAI, "vaults hazard") {}
    float GetValue(Action* action) override;
};

#endif
