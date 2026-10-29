/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_RAIDBOSSSCRIPTS_H
#define PLAYERBOTS_RAIDBOSSSCRIPTS_H

#include <string>
#include <vector>

#include "Action.h"
#include "AttackAction.h"
#include "MovementActions.h"
#include "Trigger.h"

class PlayerbotAI;

// Main tank keeps the skull mark maintained while the named boss is active;
// which unit gets skulled is the paired RaidKillOrderMarkAction's decision.
class RaidKillOrderMarkTrigger : public Trigger
{
public:
    RaidKillOrderMarkTrigger(PlayerbotAI* botAI, std::string const name, std::string const bossName)
        : Trigger(botAI, name), bossName(bossName) {}
    bool IsActive() override;

protected:
    std::string const bossName;
};

// Adds-first kill order via the skull mark: tiers of add entries die in
// order (most-damaged first within a tier, sticky so the mark doesn't flap),
// then the boss.
class RaidKillOrderMarkAction : public Action
{
public:
    RaidKillOrderMarkAction(PlayerbotAI* botAI, std::string const name, std::string const bossName,
                            std::vector<uint32> const addEntries)
        : Action(botAI, name), bossName(bossName), addEntries(addEntries) {};
    Unit* GetTarget() override;
    bool Execute(Event event) override;

protected:
    std::string const bossName;
    std::vector<uint32> const addEntries;
};

// Bot is standing in a damaging ground effect (identified by the periodic
// aura it applies) and should step out.
class RaidGroundEffectAuraTrigger : public Trigger
{
public:
    RaidGroundEffectAuraTrigger(PlayerbotAI* botAI, std::string const name, uint32 spellId)
        : Trigger(botAI, name), spellId(spellId) {}
    bool IsActive() override;

protected:
    uint32 const spellId;
};

class RaidMoveFromGroundEffectAction : public MovementAction
{
public:
    RaidMoveFromGroundEffectAction(PlayerbotAI* botAI, std::string const name)
        : MovementAction(botAI, name) {};
    bool Execute(Event event) override;
};

// Bot is within range of a hostile creature of the given entry (a mobile
// hazard such as an orb) and should step away from it.
class RaidNearCreatureTrigger : public Trigger
{
public:
    RaidNearCreatureTrigger(PlayerbotAI* botAI, std::string const name, uint32 entry, float range)
        : Trigger(botAI, name), entry(entry), range(range) {}
    bool IsActive() override;

protected:
    uint32 const entry;
    float const range;
};

class RaidMoveFromCreatureAction : public MovementAction
{
public:
    RaidMoveFromCreatureAction(PlayerbotAI* botAI, std::string const name, uint32 entry, float range)
        : MovementAction(botAI, name), entry(entry), range(range) {}
    bool Execute(Event event) override;

protected:
    uint32 const entry;
    float const range;
};

// Ranged and healers hold a minimum range band from the named boss (point-blank
// fears and AoEs). Tanks, melee, and the boss's current victim are exempt.
class RaidStandoffTrigger : public Trigger
{
public:
    RaidStandoffTrigger(PlayerbotAI* botAI, std::string const name, std::string const bossName, float minRange)
        : Trigger(botAI, name), bossName(bossName), minRange(minRange) {}
    bool IsActive() override;

protected:
    std::string const bossName;
    float const minRange;
};

class RaidStandoffAction : public MovementAction
{
public:
    RaidStandoffAction(PlayerbotAI* botAI, std::string const name, std::string const bossName, float minRange)
        : MovementAction(botAI, name), bossName(bossName), minRange(minRange) {}
    bool Execute(Event event) override;

protected:
    std::string const bossName;
    float const minRange;
};

// The named boss is attacking a non-tank (fear, knockback threat drop): the
// first living bot tank in group order that is not its victim takes it back.
// Every bot computes the same election.
class RaidBackupTauntTrigger : public Trigger
{
public:
    RaidBackupTauntTrigger(PlayerbotAI* botAI, std::string const name, std::string const bossName)
        : Trigger(botAI, name), bossName(bossName) {}
    bool IsActive() override;

protected:
    std::string const bossName;
};

class RaidBackupTauntAction : public AttackAction
{
public:
    RaidBackupTauntAction(PlayerbotAI* botAI, std::string const name, std::string const bossName)
        : AttackAction(botAI, name), bossName(bossName) {}
    bool Execute(Event event) override;

protected:
    std::string const bossName;
};

#endif
