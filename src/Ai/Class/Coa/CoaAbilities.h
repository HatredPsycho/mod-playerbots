/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef _PLAYERBOT_COAABILITIES_H
#define _PLAYERBOT_COAABILITIES_H

#include <vector>

class Player;
class SpellInfo;

// What a Conquest of Azeroth character can cast, read from its class kit (CoaAiObjectContext.cpp) rather
// than from every spell it happens to know: most CoA abilities are scripted, and the kit is what says
// which of them taunt, damage or resurrect. Only the highest rank the character has reached is listed.
namespace CoaAbilities
{
    // Taunts aimed at one enemy.
    std::vector<SpellInfo const*> KnownSingleTargetTaunts(Player* bot);
    // Damaging spells aimed at one enemy, taunts excluded.
    std::vector<SpellInfo const*> KnownSingleTargetAttacks(Player* bot);
    // Whether the character knows a spell that brings a dead ally back.
    bool CanResurrect(Player* bot);
}

#endif
