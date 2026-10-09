/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SHAREGEARACTION_H
#define PLAYERBOTS_SHAREGEARACTION_H

#include "InventoryAction.h"

#include <vector>

class Item;
class Player;
class PlayerbotAI;

// Hands a piece of gear in this bot's bags to the group member it is worth most to, the master
// included. Only what the bot carries itself is offered, so every member running the command
// covers its own bags and nothing is counted twice.
class ShareGearAction : public InventoryAction
{
public:
    ShareGearAction(PlayerbotAI* botAI, std::string const name = "share gear")
        : InventoryAction(botAI, name)
    {
    }

    bool Execute(Event event) override;

private:
    std::vector<Item*> CarriedGear();
    std::vector<Player*> Candidates();
    Player* BestRecipient(Item* item, std::vector<Player*> const& candidates);
    bool Transfer(Item* item, Player* receiver);

    static float Gain(Player* player, Item* item);
};

#endif
