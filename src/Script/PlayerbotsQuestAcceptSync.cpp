/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "Group.h"
#include "PlayerScript.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "QuestDef.h"

// Bots take the quests their master accepts (AiPlayerbot.SyncQuestAcceptWithPlayer). Stock playerbots only
// takes a quest the master shares, or every quest of a giver under "accept all quests"; SyncQuestWithPlayer
// covers completion, not acceptance.
class PlayerbotsQuestAcceptSync : public PlayerScript
{
public:
    PlayerbotsQuestAcceptSync() : PlayerScript("PlayerbotsQuestAcceptSync", {PLAYERHOOK_ON_PLAYER_QUEST_ACCEPT}) {}

    void OnPlayerQuestAccept(Player* player, Quest const* quest) override
    {
        // A bot accepting a quest reaches this hook too, including the ones mirrored below; only a real
        // player's acceptance is mirrored.
        if (!sPlayerbotAIConfig.syncQuestAcceptWithPlayer || !player || !quest || GET_PLAYERBOT_AI(player))
            return;

        Group* group = player->GetGroup();
        if (!group)
            return;

        // Same map only: group members on other maps update on other threads.
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (!member || member == player || !member->IsInWorld() || member->GetMap() != player->GetMap())
                continue;

            PlayerbotAI* botAI = GET_PLAYERBOT_AI(member);
            if (!botAI || botAI->GetMaster() != player)
                continue;

            Accept(member, botAI, quest);
        }
    }

private:
    static void Accept(Player* bot, PlayerbotAI* botAI, Quest const* quest)
    {
        uint32 const questId = quest->GetQuestId();
        if (bot->GetQuestStatus(questId) != QUEST_STATUS_NONE || !bot->CanTakeQuest(quest, false) ||
            !bot->SatisfyQuestLog(false) || !bot->CanAddQuest(quest, false))
            return;

        bot->AddQuestAndCheckCompletion(quest, nullptr);
        if (bot->GetQuestStatus(questId) != QUEST_STATUS_NONE)
            botAI->TellMaster("Accepted " + botAI->GetChatHelper()->FormatQuest(quest));
    }
};

void AddPlayerbotsQuestAcceptSyncScripts()
{
    new PlayerbotsQuestAcceptSync();
}
