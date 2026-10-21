/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Chat.h"
#include "DBCStores.h"
#include "GameObjectData.h"
#include "Group.h"
#include "Map.h"
#include "ObjectMgr.h"
#include "PlayerScript.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "QuestDef.h"
#include "StringFormat.h"

// The quests near the player that only some of his bots can take, for the CoA Companion addon (BotQuests.lua).
// The addon whispers itself "CoAQ\tSCAN\t<0|1>"; the answer is a run of LANG_ADDON whispers:
//   CoAQ\tBEGIN\t<mode>
//   CoAQ\tZONE\t<zone name>\t<width yards>\t<height yards>
//   CoAQ\tQ\t<bot>\t<quest id>\t<level>\t<c|r|s>\t<distance>\t<zone name>\t<x %>\t<y %>\t<giver>\t<title>
//   CoAQ\tEND\t<count>
// Mode 0 lists class quests only; mode 1 adds race and profession quests. A quest is listed for a bot when that
// bot can take it now (Player::CanTakeQuest) and it is restricted to something the bot has, so the quests every
// character gets stay out.
namespace
{
    constexpr char const* AddonPrefix = "CoAQ";
    constexpr std::size_t MaximumMessage = 250;
    constexpr std::size_t MaximumPerBot = 40;
    constexpr uint32 AllianceRaceMask = 1101;
    constexpr uint32 HordeRaceMask = 690;

    struct Giver
    {
        uint32 entry;
        bool creature;
        float x;
        float y;
        float z;
        float distance;
    };

    struct Offer
    {
        Quest const* quest;
        char kind;
        Giver const* giver;
    };

    void Send(Player* player, std::string text)
    {
        if (text.size() > MaximumMessage)
            text.resize(MaximumMessage);
        WorldPacket data;
        ChatHandler::BuildChatPacket(data, CHAT_MSG_WHISPER, LANG_ADDON, player, player, text);
        player->GetSession()->SendPacket(&data);
    }

    std::string Clipped(std::string text, std::size_t length)
    {
        std::replace(text.begin(), text.end(), '\t', ' ');
        if (text.size() > length)
            text.resize(length);
        return text;
    }

    std::vector<Giver> NearbyGivers(Player* player, float radius)
    {
        std::vector<Giver> givers;
        uint32 const mapId = player->GetMapId();
        auto consider = [&](uint32 entry, bool creature, uint16 spawnMap, float x, float y, float z)
        {
            if (spawnMap != mapId)
                return;
            float const distance = player->GetExactDist2d(x, y);
            if (distance <= radius)
                givers.push_back({entry, creature, x, y, z, distance});
        };

        for (auto const& [guid, data] : sObjectMgr->GetAllCreatureData())
        {
            auto const quests = sObjectMgr->GetCreatureQuestRelationBounds(data.id);
            if (quests.first != quests.second)
                consider(data.id, true, data.mapid, data.posX, data.posY, data.posZ);
        }
        for (auto const& [guid, data] : sObjectMgr->GetAllGOData())
        {
            auto const quests = sObjectMgr->GetGOQuestRelationBounds(data.id);
            if (quests.first != quests.second)
                consider(data.id, false, data.mapid, data.posX, data.posY, data.posZ);
        }
        std::sort(givers.begin(), givers.end(), [](Giver const& a, Giver const& b) { return a.distance < b.distance; });
        return givers;
    }

    char Restriction(Player* bot, Quest const* quest, bool all)
    {
        uint32 const classes = quest->GetRequiredClasses();
        if (classes && (classes & bot->getClassMask()))
            return 'c';
        if (!all)
            return 0;

        uint32 const races = quest->GetAllowableRaces();
        uint32 const faction = bot->GetTeamId() == TEAM_ALLIANCE ? AllianceRaceMask : HordeRaceMask;
        if (races && (races & bot->getRaceMask()) && (races & faction) != faction)
            return 'r';
        if (quest->GetRequiredSkill())
            return 's';
        return 0;
    }

    std::vector<Offer> OffersFor(Player* bot, std::vector<Giver> const& givers, bool all)
    {
        std::vector<Offer> offers;
        std::set<uint32> seen;
        for (Giver const& giver : givers)
        {
            auto const quests = giver.creature ? sObjectMgr->GetCreatureQuestRelationBounds(giver.entry)
                                               : sObjectMgr->GetGOQuestRelationBounds(giver.entry);
            for (auto itr = quests.first; itr != quests.second && offers.size() < MaximumPerBot; ++itr)
            {
                Quest const* quest = sObjectMgr->GetQuestTemplate(itr->second);
                if (!quest || seen.count(quest->GetQuestId()))
                    continue;
                char const kind = Restriction(bot, quest, all);
                if (!kind || !bot->CanTakeQuest(quest, false))
                    continue;
                seen.insert(quest->GetQuestId());
                offers.push_back({quest, kind, &giver});
            }
        }
        return offers;
    }

    std::string GiverName(Giver const& giver, LocaleConstant locale)
    {
        if (giver.creature)
        {
            if (CreatureTemplate const* info = sObjectMgr->GetCreatureTemplate(giver.entry))
            {
                if (CreatureLocale const* local = sObjectMgr->GetCreatureLocale(giver.entry))
                    if (local->Name.size() > size_t(locale) && !local->Name[locale].empty())
                        return local->Name[locale];
                return info->Name;
            }
        }
        else if (GameObjectTemplate const* info = sObjectMgr->GetGameObjectTemplate(giver.entry))
            return info->name;
        return "?";
    }

    struct ZoneView
    {
        uint32 zone = 0;
        std::string name;
        float x = -1.0f;
        float y = -1.0f;
    };

    // Map2ZoneCoordinates turns world coordinates into the client's zone percent; it leaves them untouched for
    // a zone without a world map (instances), which the range check catches.
    ZoneView ViewOf(Player* player, Giver const& giver, LocaleConstant locale)
    {
        ZoneView view;
        view.zone = player->GetMap()->GetZoneId(player->GetPhaseMask(), giver.x, giver.y, giver.z);
        if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(view.zone))
            view.name = area->area_name[locale];
        float x = giver.x;
        float y = giver.y;
        Map2ZoneCoordinates(x, y, view.zone);
        if (x >= 0.0f && x <= 100.0f && y >= 0.0f && y <= 100.0f && (x != giver.x || y != giver.y))
        {
            view.x = x;
            view.y = y;
        }
        return view;
    }

    // Yards across the zone's world map, from how far its percent moves over ten yards each way.
    bool ZoneSize(uint32 zone, float& width, float& height)
    {
        float x0 = 0.0f, y0 = 0.0f, x1 = 0.0f, y1 = 10.0f, x2 = 10.0f, y2 = 0.0f;
        Map2ZoneCoordinates(x0, y0, zone);
        Map2ZoneCoordinates(x1, y1, zone);
        Map2ZoneCoordinates(x2, y2, zone);
        float const perTenYardsX = std::fabs(x1 - x0);
        float const perTenYardsY = std::fabs(y2 - y0);
        if (perTenYardsX < 1e-6f || perTenYardsY < 1e-6f || (x1 == 0.0f && y1 == 10.0f))
            return false;
        width = 1000.0f / perTenYardsX;
        height = 1000.0f / perTenYardsY;
        return true;
    }

    void Scan(Player* player, bool all)
    {
        Send(player, Acore::StringFormat("{}\tBEGIN\t{}", AddonPrefix, all ? 1 : 0));
        uint32 count = 0;
        Group* group = player->GetGroup();
        if (group)
        {
            LocaleConstant const locale = player->GetSession()->GetSessionDbcLocale();
            std::vector<Giver> const givers = NearbyGivers(player, sPlayerbotAIConfig.nearbyQuestRadius);
            std::set<uint32> zonesSent;
            for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
            {
                Player* bot = ref->GetSource();
                if (!bot || bot == player || !bot->IsInWorld() || bot->GetMap() != player->GetMap())
                    continue;
                PlayerbotAI* botAI = GET_PLAYERBOT_AI(bot);
                if (!botAI || botAI->GetMaster() != player)
                    continue;

                for (Offer const& offer : OffersFor(bot, givers, all))
                {
                    ZoneView const view = ViewOf(player, *offer.giver, locale);
                    float width = 0.0f, height = 0.0f;
                    if (view.x >= 0.0f && zonesSent.insert(view.zone).second && ZoneSize(view.zone, width, height))
                        Send(player, Acore::StringFormat("{}\tZONE\t{}\t{:.0f}\t{:.0f}", AddonPrefix,
                                                         Clipped(view.name, 40), width, height));

                    std::string title = offer.quest->GetTitle();
                    if (QuestLocale const* local = sObjectMgr->GetQuestLocale(offer.quest->GetQuestId()))
                        ObjectMgr::GetLocaleString(local->Title, locale, title);
                    Send(player, Acore::StringFormat("{}\tQ\t{}\t{}\t{}\t{}\t{:.0f}\t{}\t{:.1f}\t{:.1f}\t{}\t{}",
                                                     AddonPrefix, bot->GetName(), offer.quest->GetQuestId(),
                                                     offer.quest->GetQuestLevel(), offer.kind,
                                                     offer.giver->distance, Clipped(view.name, 40), view.x, view.y,
                                                     Clipped(GiverName(*offer.giver, locale), 40),
                                                     Clipped(title, 70)));
                    ++count;
                }
            }
        }
        Send(player, Acore::StringFormat("{}\tEND\t{}", AddonPrefix, count));
    }
}

class PlayerbotsNearbyQuests : public PlayerScript
{
public:
    PlayerbotsNearbyQuests() : PlayerScript("PlayerbotsNearbyQuests", {PLAYERHOOK_ON_BEFORE_SEND_CHAT_MESSAGE}) {}

    void OnPlayerBeforeSendChatMessage(Player* player, uint32& /*type*/, uint32& lang, std::string& msg) override
    {
        static std::string const request = std::string(AddonPrefix) + "\tSCAN";
        if (lang != LANG_ADDON || !player || GET_PLAYERBOT_AI(player) || msg.compare(0, request.size(), request) != 0)
            return;
        Scan(player, msg.size() > request.size() + 1 && msg[request.size() + 1] == '1');
    }
};

void AddPlayerbotsNearbyQuestsScripts()
{
    new PlayerbotsNearbyQuests();
}
