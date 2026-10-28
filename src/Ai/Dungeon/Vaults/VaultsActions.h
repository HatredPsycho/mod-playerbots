/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_VAULTSACTIONS_H
#define PLAYERBOTS_VAULTSACTIONS_H

#include "MovementActions.h"

class Creature;

class VaultsLeaveHazardAction : public MovementAction
{
public:
    VaultsLeaveHazardAction(PlayerbotAI* botAI) : MovementAction(botAI, "vaults leave hazard") {}
    bool Execute(Event event) override;
};

class VaultsSoakAction : public MovementAction
{
public:
    VaultsSoakAction(PlayerbotAI* botAI) : MovementAction(botAI, "vaults soak") {}
    bool Execute(Event event) override;
};

class VaultsTakeShelterAction : public MovementAction
{
public:
    VaultsTakeShelterAction(PlayerbotAI* botAI) : MovementAction(botAI, "vaults take shelter") {}
    bool Execute(Event event) override;
};

class VaultsClickCrystalAction : public MovementAction
{
public:
    VaultsClickCrystalAction(PlayerbotAI* botAI, std::string const name = "vaults click crystal")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;

protected:
    bool Click(Creature* crystal);
};

class VaultsBreakSealAction : public VaultsClickCrystalAction
{
public:
    VaultsBreakSealAction(PlayerbotAI* botAI) : VaultsClickCrystalAction(botAI, "vaults break seal") {}
    bool Execute(Event event) override;
};

#endif
