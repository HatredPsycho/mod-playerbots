/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "VaultsTriggers.h"
#include "Playerbots.h"
#include "VaultsShared.h"

using namespace VaultsShared;

bool VaultsInHazardTrigger::IsActive()
{
    return InHazard(bot);
}

bool VaultsSoakTrigger::IsActive()
{
    return SoakToTake(bot) != nullptr;
}

bool VaultsShelterTrigger::IsActive()
{
    return ShelterToReach(bot) != nullptr;
}

bool VaultsHolyCrystalTrigger::IsActive()
{
    return FightCrystalToClick(bot) != nullptr;
}

bool VaultsSealCrystalTrigger::IsActive()
{
    return SealCrystalToBreak(bot) != nullptr;
}
