/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_VAULTSTRIGGERS_H
#define PLAYERBOTS_VAULTSTRIGGERS_H

#include "Trigger.h"

class VaultsInHazardTrigger : public Trigger
{
public:
    VaultsInHazardTrigger(PlayerbotAI* botAI) : Trigger(botAI, "vaults in hazard") {}
    bool IsActive() override;
};

class VaultsSoakTrigger : public Trigger
{
public:
    VaultsSoakTrigger(PlayerbotAI* botAI) : Trigger(botAI, "vaults soak") {}
    bool IsActive() override;
};

class VaultsShelterTrigger : public Trigger
{
public:
    VaultsShelterTrigger(PlayerbotAI* botAI) : Trigger(botAI, "vaults shelter") {}
    bool IsActive() override;
};

class VaultsHolyCrystalTrigger : public Trigger
{
public:
    VaultsHolyCrystalTrigger(PlayerbotAI* botAI) : Trigger(botAI, "vaults holy crystal") {}
    bool IsActive() override;
};

class VaultsSealCrystalTrigger : public Trigger
{
public:
    VaultsSealCrystalTrigger(PlayerbotAI* botAI) : Trigger(botAI, "vaults seal crystal") {}
    bool IsActive() override;
};

#endif
