/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef _PLAYERBOT_LFGROLEACTION_H
#define _PLAYERBOT_LFGROLEACTION_H

#include "Action.h"

class Player;
class PlayerbotAI;

// The role a bot offers in the Dungeon Finder, set by its player: "lfgrole tank|heal|dps|auto".
// Below level 10 a CoA character has no specialization and stock classes have no talents, so every
// such bot offered damage and a party of alts could never pass the role check ("not viable").
// 0 when the role follows the specialization as before; otherwise a PLAYER_ROLE_* value.
uint8 GetLfgRoleOverride(Player* bot);

class LfgRoleAction : public Action
{
public:
    LfgRoleAction(PlayerbotAI* botAI) : Action(botAI, "lfgrole") {}

    bool Execute(Event event) override;
};

#endif
