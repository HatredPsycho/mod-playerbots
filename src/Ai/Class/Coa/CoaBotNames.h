/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_COABOTNAMES_H
#define _PLAYERBOT_COABOTNAMES_H

#include "Define.h"

#include <string>

// AiPlayerbot.CoaBotSurname: what random bots carry after their first name.
enum class CoaBotSurnameMode : uint8
{
    Off       = 0,  // "Kegarink"
    Fixed     = 1,  // "Kegarink Bot", the text of AiPlayerbot.CoaBotSurnameText
    Generated = 2   // "Kegarink Bloodfang", a surname drawn for the bot's race
};

CoaBotSurnameMode CoaBotSurnameSetting();

// The bot's whole name for its first name and race: unchanged when surnames are off, when the first
// name already has two words, or when no valid surname was found.
std::string CoaBotName(std::string const& firstName, uint8 race);

// Brings every random bot's surname in line with the setting: adds, replaces or takes it back. Run at
// startup, once the random bots exist and before any of them logs in.
void ApplyCoaBotSurnames();

#endif
