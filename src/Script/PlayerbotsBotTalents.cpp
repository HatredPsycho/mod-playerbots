/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>

#include "AscensionCoATalentData.h"
#include "AscensionSpecialization.h"
#include "Chat.h"
#include "CoaSpecialization.h"
#include "Group.h"
#include "PlayerScript.h"
#include "Playerbots.h"
#include "StringFormat.h"

// The talents of the player's CoA alts for the talent window of CoA Companion (BotTalents.lua), over LANG_ADDON
// whispers the addon sends to itself with the prefix CoAT:
//   GET\t<bot>                     the bot's state
//   LEARN\t<bot>\t<entry>,<entry>  one rank per listed entry, in as many passes as prerequisites need
//   SPEC\t<bot>\t<specialization>  switches the bot's specialization
//   RESET\t<bot>                   takes back every talent bought with points
// Every request is answered with the bot's state:
//   CoAT\tBEGIN\t<bot>\t<level>\t<specialization>
//   CoAT\tPTS\t<bot>\t<class spent>\t<class budget>\t<spec spent>\t<spec budget>
//   CoAT\tE\t<bot>\t<entry>:<rank>:<max rank>:<cost>:<c|s>:<level>,...   (several lines)
//   CoAT\tEND\t<bot>
// and a command also with CoAT\tMSG\t<bot>\t<text>. Only the player's own alts on the player's map are served.
namespace
{
    constexpr char const* AddonPrefix = "CoAT";
    constexpr std::size_t MaximumMessage = 250;

    void Send(Player* player, std::string text)
    {
        if (text.size() > MaximumMessage)
            text.resize(MaximumMessage);
        WorldPacket data;
        ChatHandler::BuildChatPacket(data, CHAT_MSG_WHISPER, LANG_ADDON, player, player, text);
        player->GetSession()->SendPacket(&data);
    }

    std::vector<std::string> Split(std::string const& text, char separator)
    {
        std::vector<std::string> parts;
        std::size_t start = 0;
        while (start <= text.size())
        {
            std::size_t const end = std::min(text.find(separator, start), text.size());
            parts.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        return parts;
    }

    Player* AltOf(Player* master, std::string const& name, PlayerbotAI*& botAI)
    {
        botAI = nullptr;
        Group* group = master->GetGroup();
        if (!group)
            return nullptr;
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* bot = ref->GetSource();
            if (!bot || bot == master || bot->GetName() != name || !bot->IsInWorld() || bot->GetMap() != master->GetMap())
                continue;
            PlayerbotAI* ai = GET_PLAYERBOT_AI(bot);
            if (!ai || ai->GetMaster() != master || !ai->IsAltBot() || !IsAscensionCustomClassId(bot->getClass()))
                return nullptr;
            botAI = ai;
            return bot;
        }
        return nullptr;
    }

    void SendState(Player* master, Player* bot)
    {
        std::string const name = bot->GetName();
        Send(master, Acore::StringFormat("{}\tBEGIN\t{}\t{}\t{}", AddonPrefix, name, bot->GetLevel(),
                                         GetAscensionActiveSpecialization(bot)));
        CoaPoints const points = PointsOf(bot);
        Send(master, Acore::StringFormat("{}\tPTS\t{}\t{}\t{}\t{}\t{}", AddonPrefix, name, points.classSpent,
                                         points.classBudget, points.specSpent, points.specBudget));

        std::string const head = Acore::StringFormat("{}\tE\t{}\t", AddonPrefix, name);
        std::string line;
        for (AscensionCompatData::CoATalentEntry const& entry : AscensionCompatData::CoATalentEntries)
        {
            if (entry.ClassId != bot->getClass() || !entry.SpellCount)
                continue;
            uint32 const rank = GetAscensionTalentRank(bot, entry.EntryId);
            if (!rank && !CoaEntryOffered(bot, entry))
                continue;
            std::string const item = Acore::StringFormat("{}:{}:{}:{}:{}:{}", entry.EntryId, rank,
                                                         uint32(entry.SpellCount),
                                                         uint32(entry.SpecId ? entry.TECost : entry.AECost),
                                                         entry.SpecId ? 's' : 'c', uint32(entry.RequiredLevel));
            if (!line.empty() && head.size() + line.size() + 1 + item.size() > MaximumMessage)
            {
                Send(master, head + line);
                line.clear();
            }
            line += (line.empty() ? "" : ",") + item;
        }
        if (!line.empty())
            Send(master, head + line);
        Send(master, Acore::StringFormat("{}\tEND\t{}", AddonPrefix, name));
    }

    void Tell(Player* master, Player* bot, std::string const& text)
    {
        Send(master, Acore::StringFormat("{}\tMSG\t{}\t{}", AddonPrefix, bot->GetName(), text));
    }

    void Learn(Player* master, Player* bot, PlayerbotAI* botAI, std::string const& list)
    {
        std::vector<uint32> pending;
        for (std::string const& part : Split(list, ','))
            if (uint32 const entryId = uint32(std::strtoul(part.c_str(), nullptr, 10)))
                pending.push_back(entryId);

        uint32 learned = 0;
        std::string refusal;
        bool progress = true;
        while (!pending.empty() && progress)
        {
            progress = false;
            for (auto itr = pending.begin(); itr != pending.end();)
            {
                AscensionCompatData::CoATalentEntry const* entry = FindCoaEntry(*itr);
                if (!entry || entry->ClassId != bot->getClass())
                {
                    itr = pending.erase(itr);
                    continue;
                }
                uint32 const rank = GetAscensionTalentRank(bot, entry->EntryId) + 1;
                std::string const why = RefusalFor(bot, *entry, rank);
                if (why.empty() && SetAscensionTalentRank(bot, entry->EntryId, rank) &&
                    GetAscensionTalentRank(bot, entry->EntryId) == rank)
                {
                    ++learned;
                    progress = true;
                    itr = pending.erase(itr);
                    continue;
                }
                refusal = why.empty() ? "refused by the server" : why;
                ++itr;
            }
        }

        if (learned)
            botAI->ResetStrategies();
        std::string text = "Learned " + std::to_string(learned) + " rank" + (learned == 1 ? "" : "s");
        if (!pending.empty())
            text += ", " + std::to_string(pending.size()) + " refused: " + refusal;
        Tell(master, bot, text);
    }

    void Handle(Player* master, std::vector<std::string> const& parts)
    {
        if (parts.size() < 2)
            return;
        PlayerbotAI* botAI = nullptr;
        Player* bot = AltOf(master, parts[1], botAI);
        if (!bot)
        {
            Send(master, Acore::StringFormat("{}\tNONE\t{}", AddonPrefix, parts[1]));
            return;
        }

        std::string const& command = parts[0];
        if (command == "LEARN" && parts.size() >= 3)
            Learn(master, bot, botAI, parts[2]);
        else if (command == "SPEC" && parts.size() >= 3)
        {
            uint32 const specializationId = uint32(std::strtoul(parts[2].c_str(), nullptr, 10));
            if (specializationId && SwitchAscensionSpecialization(bot, specializationId))
            {
                botAI->ResetStrategies();
                Tell(master, bot, "Specialization changed");
            }
            else
                Tell(master, bot, "The specialization could not be changed");
        }
        else if (command == "RESET")
        {
            uint32 const lowered = ResetCoaTalents(bot);
            botAI->ResetStrategies();
            Tell(master, bot, "Talents reset: " + std::to_string(lowered) + " entries");
        }
        else if (command != "GET")
            return;

        SendState(master, bot);
    }
}

class PlayerbotsBotTalents : public PlayerScript
{
public:
    PlayerbotsBotTalents() : PlayerScript("PlayerbotsBotTalents", {PLAYERHOOK_ON_BEFORE_SEND_CHAT_MESSAGE}) {}

    void OnPlayerBeforeSendChatMessage(Player* player, uint32& /*type*/, uint32& lang, std::string& msg) override
    {
        static std::string const prefix = std::string(AddonPrefix) + "\t";
        if (lang != LANG_ADDON || !player || GET_PLAYERBOT_AI(player) || msg.compare(0, prefix.size(), prefix) != 0)
            return;
        Handle(player, Split(msg.substr(prefix.size()), '\t'));
    }
};

void AddPlayerbotsBotTalentsScripts()
{
    new PlayerbotsBotTalents();
}
