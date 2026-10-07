/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_TRADESTATUSACTION_H
#define PLAYERBOTS_TRADESTATUSACTION_H

#include "QueryItemUsageAction.h"

#include <atomic>

class Player;
class PlayerbotAI;

// A module that trades for a bot itself - the hand-to-hand deals of mod-playerbots-auctions - installs this; the
// trade actions stand back for every bot it answers true for. Nothing is installed by default, so without such a
// module the bots trade by their own rules as always.
using TradeHandoffResolver = bool (*)(Player*);
extern std::atomic<TradeHandoffResolver> TradeHandoffOwner;

inline bool IsTradeHandedOff(Player* bot)
{
    TradeHandoffResolver const owner = TradeHandoffOwner.load(std::memory_order_relaxed);
    return owner && owner(bot);
}

class TradeStatusAction : public QueryItemUsageAction
{
public:
    TradeStatusAction(PlayerbotAI* botAI) : QueryItemUsageAction(botAI, "accept trade") {}

    bool Execute(Event event) override;

private:
    void BeginTrade();
    void CancelTrade();
    bool CheckTrade();
    int32 CalculateCost(Player* player, bool sell);
};

#endif
