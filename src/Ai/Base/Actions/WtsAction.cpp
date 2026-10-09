/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WtsAction.h"
#include "AiFactory.h"
#include "Config.h"
#include "Event.h"
#include "ItemUsageValue.h"
#include "ItemVisitors.h"
#include "Playerbots.h"

bool WtsAction::Execute(Event event)
{
    Player* owner = event.getOwner();
    if (!owner)
        return false;

    // With mod-playerbots-auctions the bots answer a "WTS" line themselves: one or two of them, with an
    // offer they stand by. This line - a price nobody means, from every bot that hears it - stays away.
    // A realm built without that module has neither setting and keeps the stock answer.
    if (sConfigMgr->GetOption<bool>("PlayerbotsAuctions.Enable", false, false) &&
        sConfigMgr->GetOption<bool>("PlayerbotsAuctions.Chat.Enable", true, false))
        return false;

    std::ostringstream out;
    std::string const text = event.getParam();

    if (!sRandomPlayerbotMgr.IsRandomBot(bot))
        return false;

    std::string const link = event.getParam();

    ItemIds itemIds = chat->parseItems(link);
    if (itemIds.empty())
        return false;

    for (ItemIds::iterator i = itemIds.begin(); i != itemIds.end(); i++)
    {
        uint32 itemId = *i;
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
        if (!proto)
            continue;

        std::ostringstream out;
        out << itemId;
        ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", out.str());
        if (usage == ITEM_USAGE_NONE)
            continue;

        int32 buyPrice = proto->BuyPrice * sRandomPlayerbotMgr.GetBuyMultiplier(bot);
        if (!buyPrice)
            continue;

        if (urand(0, 15) > 2)
            continue;

        std::ostringstream tell;
        tell << "I'll buy " << chat->FormatItem(proto) << " for " << chat->formatMoney(buyPrice);

        // ignore random bot chat filter
        bot->Whisper(tell.str(), LANG_UNIVERSAL, owner);
    }

    return true;
}
