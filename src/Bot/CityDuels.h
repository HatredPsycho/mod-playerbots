/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef _PLAYERBOT_CITYDUELS_H
#define _PLAYERBOT_CITYDUELS_H

#include "Define.h"
#include "ObjectGuid.h"

// AiPlayerbot.CityDuels.*: a few free random bots duel in front of the main gates of Orgrimmar (Horde) and
// Stormwind (Alliance), pairs of close levels, one duel after another, and are handed back to their random life
// after a while. Nothing is saved: the duellists live in memory only, and a restart finds them as random bots.
namespace CityDuels
{
// World thread, every world tick; the work itself runs every few seconds.
void Update(uint32 diff);
// A bot the city duels hold: RandomPlayerbotMgr::ProcessBot leaves it alone (no randomize, no teleport).
bool IsDuellist(ObjectGuid guid);
// Every duellist back to its random life; at shutdown without the teleport home.
void ReleaseAll(bool sendBack);
}  // namespace CityDuels

#endif
