/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "LfgRoleAction.h"

#include "Event.h"
#include "LFG.h"
#include "Player.h"
#include "Playerbots.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace
{
    constexpr char const* SettingSource = "mod-playerbots-lfg";
    constexpr uint32 RoleIndex = 0;

    char const* RoleName(uint8 role)
    {
        switch (role)
        {
            case lfg::PLAYER_ROLE_TANK:   return "tank";
            case lfg::PLAYER_ROLE_HEALER: return "heal";
            case lfg::PLAYER_ROLE_DAMAGE: return "dps";
            default:                      return "auto (from the specialization)";
        }
    }
}

uint8 GetLfgRoleOverride(Player* bot)
{
    if (!bot)
        return 0;
    uint32 const role = bot->GetPlayerSetting(SettingSource, RoleIndex).value;
    return role == lfg::PLAYER_ROLE_TANK || role == lfg::PLAYER_ROLE_HEALER || role == lfg::PLAYER_ROLE_DAMAGE
        ? uint8(role) : 0;
}

bool LfgRoleAction::Execute(Event event)
{
    std::string value = event.getParam();
    value.erase(0, value.find_first_not_of(' '));
    value.erase(value.find_last_not_of(' ') + 1);
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return std::tolower(c); });

    uint8 role;
    if (value.empty() || value == "?")
    {
        botAI->TellMaster(std::string("LFG role: ") + RoleName(GetLfgRoleOverride(bot)));
        return true;
    }
    else if (value == "tank")
        role = lfg::PLAYER_ROLE_TANK;
    else if (value == "heal" || value == "healer")
        role = lfg::PLAYER_ROLE_HEALER;
    else if (value == "dps" || value == "damage")
        role = lfg::PLAYER_ROLE_DAMAGE;
    else if (value == "auto" || value == "reset")
        role = 0;
    else
    {
        botAI->TellMaster("Usage: lfgrole <tank|heal|dps|auto>");
        return true;
    }

    bot->UpdatePlayerSetting(SettingSource, RoleIndex, role);
    botAI->TellMaster(std::string("LFG role: ") + RoleName(role));
    return true;
}
