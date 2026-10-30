/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_VAULTSTRIGGERCONTEXT_H
#define PLAYERBOTS_VAULTSTRIGGERCONTEXT_H

#include "NamedObjectContext.h"
#include "VaultsTriggers.h"

class CoaDungeonVaultsTriggerContext : public NamedObjectContext<Trigger>
{
public:
    CoaDungeonVaultsTriggerContext()
    {
        creators["vaults in hazard"] = &CoaDungeonVaultsTriggerContext::in_hazard;
        creators["vaults soak"] = &CoaDungeonVaultsTriggerContext::soak;
        creators["vaults shelter"] = &CoaDungeonVaultsTriggerContext::shelter;
        creators["vaults holy crystal"] = &CoaDungeonVaultsTriggerContext::holy_crystal;
        creators["vaults seal crystal"] = &CoaDungeonVaultsTriggerContext::seal_crystal;
    }
private:
    static Trigger* in_hazard(PlayerbotAI* botAI) { return new VaultsInHazardTrigger(botAI); }
    static Trigger* soak(PlayerbotAI* botAI) { return new VaultsSoakTrigger(botAI); }
    static Trigger* shelter(PlayerbotAI* botAI) { return new VaultsShelterTrigger(botAI); }
    static Trigger* holy_crystal(PlayerbotAI* botAI) { return new VaultsHolyCrystalTrigger(botAI); }
    static Trigger* seal_crystal(PlayerbotAI* botAI) { return new VaultsSealCrystalTrigger(botAI); }
};

#endif
