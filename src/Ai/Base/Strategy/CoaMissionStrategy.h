/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_COAMISSIONSTRATEGY_H
#define PLAYERBOTS_COAMISSIONSTRATEGY_H

#include "Strategy.h"

class PlayerbotAI;

// Non-combat strategy of an alt on a gathering mission (CoaMissionMgr): set and removed by the mission itself.
// Below the loot actions (5-8), so that the bot gathers the node it walked to before walking on.
class CoaMissionStrategy : public Strategy
{
public:
    CoaMissionStrategy(PlayerbotAI* botAI) : Strategy(botAI) {}

    std::string const getName() override { return "mission"; }
    std::vector<NextAction> getDefaultActions() override { return { NextAction("mission update", 4.0f) }; }
};

#endif
