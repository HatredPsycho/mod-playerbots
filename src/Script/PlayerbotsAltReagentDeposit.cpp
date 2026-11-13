/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include <mutex>
#include <unordered_map>

#include "Chat.h"
#include "PlayerScript.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"

// Alt bots put their materials into the account vault (mod-abyssal-storage-plus) the way a played character does.
// The module vaults looted materials as they arrive; whatever else lands in an alt bot's bags - crafted, bought or
// mailed materials - stays there, because an alt bot has no client and no addon to deposit it. Each alt bot runs
// ".asp deposit" itself every AiPlayerbot.AltBotReagentDepositInterval seconds. The module decides what counts as
// a material; without the module the command is unknown and nothing happens. Random bots keep their loot: their
// vault is no player's.
class PlayerbotsAltReagentDeposit : public PlayerScript
{
public:
    PlayerbotsAltReagentDeposit()
        : PlayerScript("PlayerbotsAltReagentDeposit", {PLAYERHOOK_ON_AFTER_UPDATE, PLAYERHOOK_ON_LOGOUT}) {}

    void OnPlayerAfterUpdate(Player* player, uint32 diff) override
    {
        uint32 const interval = sPlayerbotAIConfig.altBotReagentDepositInterval * IN_MILLISECONDS;
        if (!interval || !player || !GET_PLAYERBOT_AI(player))
            return;

        {
            // Players on different maps update on different threads.
            std::lock_guard<std::mutex> guard(_mutex);
            uint32& elapsed = _elapsed[player->GetGUID()];
            elapsed += diff;
            if (elapsed < interval)
                return;
            elapsed = 0;
        }

        if (!player->IsInWorld() || !player->IsAlive() || player->IsInCombat() || player->GetTradeData() ||
            sRandomPlayerbotMgr.IsRandomBot(player))
            return;

        ChatHandler(player->GetSession()).ParseCommands(".asp deposit");
    }

    void OnPlayerLogout(Player* player) override
    {
        std::lock_guard<std::mutex> guard(_mutex);
        _elapsed.erase(player->GetGUID());
    }

private:
    std::mutex _mutex;
    std::unordered_map<ObjectGuid, uint32> _elapsed;
};

void AddPlayerbotsAltReagentDepositScripts()
{
    new PlayerbotsAltReagentDeposit();
}
