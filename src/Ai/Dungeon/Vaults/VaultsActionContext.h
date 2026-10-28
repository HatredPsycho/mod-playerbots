/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_VAULTSACTIONCONTEXT_H
#define PLAYERBOTS_VAULTSACTIONCONTEXT_H

#include "NamedObjectContext.h"
#include "VaultsActions.h"

class CoaDungeonVaultsActionContext : public NamedObjectContext<Action>
{
public:
    CoaDungeonVaultsActionContext()
    {
        creators["vaults leave hazard"] = &CoaDungeonVaultsActionContext::leave_hazard;
        creators["vaults soak"] = &CoaDungeonVaultsActionContext::soak;
        creators["vaults take shelter"] = &CoaDungeonVaultsActionContext::take_shelter;
        creators["vaults click crystal"] = &CoaDungeonVaultsActionContext::click_crystal;
        creators["vaults break seal"] = &CoaDungeonVaultsActionContext::break_seal;
    }
private:
    static Action* leave_hazard(PlayerbotAI* botAI) { return new VaultsLeaveHazardAction(botAI); }
    static Action* soak(PlayerbotAI* botAI) { return new VaultsSoakAction(botAI); }
    static Action* take_shelter(PlayerbotAI* botAI) { return new VaultsTakeShelterAction(botAI); }
    static Action* click_crystal(PlayerbotAI* botAI) { return new VaultsClickCrystalAction(botAI); }
    static Action* break_seal(PlayerbotAI* botAI) { return new VaultsBreakSealAction(botAI); }
};

#endif
