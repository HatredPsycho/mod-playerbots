/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "ShareGearAction.h"

#include "ChatHelper.h"
#include "Event.h"
#include "Group.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "ItemVisitors.h"
#include "Player.h"
#include "Playerbots.h"
#include "StatsWeightCalculator.h"

#include <algorithm>
#include <sstream>

namespace
{
    // The item changes hands without a trade window, so the two have to stand together the way
    // they would for one.
    constexpr float SHARE_GEAR_RANGE = 30.0f;

    class CarriedGearVisitor : public IterateItemsVisitor
    {
    public:
        CarriedGearVisitor(std::vector<Item*>& out) : IterateItemsVisitor(), items(out) {}

        bool Visit(Item* item) override
        {
            ItemTemplate const* proto = item ? item->GetTemplate() : nullptr;
            if (!proto)
                return true;

            if (proto->Class != ITEM_CLASS_WEAPON && proto->Class != ITEM_CLASS_ARMOR)
                return true;

            items.push_back(item);
            return true;
        }

    private:
        std::vector<Item*>& items;
    };
}

float ShareGearAction::Gain(Player* player, Item* item)
{
    uint16 dest = 0;
    if (player->CanEquipItem(NULL_SLOT, dest, item, false) != EQUIP_ERR_OK)
        return 0.0f;

    StatsWeightCalculator calculator(player);
    float const offered = calculator.CalculateItem(item->GetEntry(), item->GetItemRandomPropertyId());

    float worn = 0.0f;
    if (Item* equipped = player->GetItemByPos(dest))
        worn = calculator.CalculateItem(equipped->GetEntry(), equipped->GetItemRandomPropertyId());

    return offered - worn;
}

std::vector<Item*> ShareGearAction::CarriedGear()
{
    // Collected first and moved afterwards: moving an item while the bags are being walked
    // invalidates the iteration.
    std::vector<Item*> items;
    CarriedGearVisitor visitor(items);
    IterateItems(&visitor, ITERATE_ITEMS_IN_BAGS);
    return items;
}

std::vector<Player*> ShareGearAction::Candidates()
{
    std::vector<Player*> out;

    Group* group = bot->GetGroup();
    if (!group)
        return out;

    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || member == bot || !member->IsInWorld() || !member->IsAlive())
            continue;

        if (member->GetMapId() != bot->GetMapId() || bot->GetDistance(member) > SHARE_GEAR_RANGE)
            continue;

        if (member->GetSession() && member->GetSession()->isLogingOut())
            continue;

        out.push_back(member);
    }

    return out;
}

Player* ShareGearAction::BestRecipient(Item* item, std::vector<Player*> const& candidates)
{
    float const keep = Gain(bot, item);

    Player* best = nullptr;
    float bestGain = 0.0f;

    for (Player* candidate : candidates)
    {
        if (candidate->CanUseItem(item->GetTemplate()) != EQUIP_ERR_OK)
            continue;

        float const gain = Gain(candidate, item);
        if (gain <= 0.0f || gain <= keep || gain <= bestGain)
            continue;

        ItemPosCountVec dest;
        if (candidate->CanStoreItem(NULL_BAG, NULL_SLOT, dest, item, false) != EQUIP_ERR_OK)
            continue;

        best = candidate;
        bestGain = gain;
    }

    return best;
}

bool ShareGearAction::Transfer(Item* item, Player* receiver)
{
    ItemPosCountVec dest;
    if (receiver->CanStoreItem(NULL_BAG, NULL_SLOT, dest, item, false) != EQUIP_ERR_OK)
        return false;

    uint8 const bag = item->GetBagSlot();
    uint8 const slot = item->GetSlot();

    bot->MoveItemFromInventory(bag, slot, true);
    receiver->MoveItemToInventory(dest, item, true, true);

    return true;
}

bool ShareGearAction::Execute(Event event)
{
    if (!bot->GetGroup())
    {
        botAI->TellError("I am not in a group.");
        return false;
    }

    std::vector<Player*> const candidates = Candidates();
    if (candidates.empty())
    {
        botAI->TellError("Nobody of the group is close enough to hand gear to.");
        return false;
    }

    uint32 handed = 0;
    std::ostringstream out;

    for (Item* item : CarriedGear())
    {
        // Strict on purpose. A soulbound item is refused even inside its two hour trade window:
        // that window only opens for the players the loot was rolled among, and the list behind it
        // is not readable from here, so the item stays where it is.
        if (item->IsSoulBound() || item->IsInTrade() || !item->CanBeTraded())
            continue;

        if (item->GetTemplate()->Bonding == BIND_QUEST_ITEM)
            continue;

        Player* receiver = BestRecipient(item, candidates);
        if (!receiver)
            continue;

        std::string const name = ChatHelper::FormatItem(item->GetTemplate());
        if (!Transfer(item, receiver))
            continue;

        ++handed;
        out << name << " -> " << receiver->GetName() << ". ";
    }

    if (!handed)
    {
        botAI->TellMaster("Nothing in my bags is worth more to anybody else.");
        return false;
    }

    botAI->TellMaster(out.str());
    return true;
}
