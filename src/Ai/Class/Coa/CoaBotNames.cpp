/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "CoaBotNames.h"

#include "CharacterCache.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "PlayerbotAIConfig.h"
#include "Random.h"
#include "SharedDefines.h"

#include <algorithm>
#include <cctype>
#include <unordered_set>
#include <vector>

namespace
{
// Conquest of Azeroth names take two words, a first name and a surname (client revision 8). A fixed
// surname tells a bot from a player at a glance; a generated one gives the bot a family name of its people.
constexpr char const* DefaultSurname = "Bot";
constexpr uint32 SurnameDraws = 20;

// Lore-style surnames are two parts joined, "Blood" + "fang": a few dozen parts per race make hundreds of
// names, and the name checks drop the joins that run too long or read wrong.
struct SurnameParts
{
    std::vector<char const*> first;
    std::vector<char const*> second;
};

SurnameParts const Human = {
    { "Ash", "Black", "Bright", "Stone", "Silver", "Wind", "Iron", "Oak", "Raven", "Hawk", "Thorn", "Gold",
      "Green", "Marsh", "Hill", "Red", "Swift", "Wolf", "North", "West", "Fair", "Grey", "Lion", "Kings" },
    { "ford", "wood", "more", "well", "ton", "field", "brook", "crest", "worth", "hart", "shire", "ridge",
      "bane", "mere", "gate", "holt", "wick", "vale", "shield", "ward", "mont", "son" }
};

SurnameParts const Orc = {
    { "Blood", "Black", "Bone", "Skull", "War", "Doom", "Iron", "Thunder", "Rage", "Storm", "Wolf", "Grim",
      "Dark", "Rock", "Gore", "Steel", "Burning", "Shatter", "Frost", "Dragon", "Red" },
    { "fang", "hammer", "maul", "fist", "tusk", "howl", "rage", "blade", "cleaver", "crusher", "eye", "jaw",
      "scar", "hide", "axe", "ripper", "mane", "brand", "gash", "skull" }
};

SurnameParts const Dwarf = {
    { "Bronze", "Iron", "Stone", "Thunder", "Fire", "Granite", "Anvil", "Steel", "Coal", "Deep", "Hammer",
      "Frost", "Copper", "Rock", "Strong", "Wild", "Battle", "Mountain", "Gold", "Ale" },
    { "beard", "forge", "hammer", "mantle", "brew", "fist", "shield", "axe", "helm", "delve", "mane", "brand",
      "born", "braid", "bellow", "mug", "anvil", "bottom", "keg" }
};

SurnameParts const NightElf = {
    { "Moon", "Star", "Shadow", "Silver", "Night", "Leaf", "Wild", "Dawn", "Mist", "Raven", "Swift", "Whisper",
      "Dream", "Thistle", "Sky", "Storm", "Feather", "Glade", "Wind", "Ever" },
    { "whisper", "song", "shade", "leaf", "breeze", "runner", "bough", "grove", "wind", "fall", "glade",
      "strider", "blossom", "dew", "bow", "rage", "weaver", "shadow", "feather", "brook" }
};

SurnameParts const Undead = {
    { "Grave", "Death", "Bone", "Dread", "Gloom", "Grim", "Rot", "Plague", "Ghast", "Dusk", "Cold", "Black",
      "Night", "Ash", "Mourn", "Vile", "Hollow", "Bleak", "Pale", "Blight" },
    { "whisper", "shroud", "bane", "grave", "rot", "moor", "wail", "veil", "soul", "born", "crypt", "fall",
      "thorn", "marrow", "rest", "wither", "gloom", "shade", "hollow", "walker" }
};

SurnameParts const Tauren = {
    { "Thunder", "Storm", "Grim", "Earth", "Rain", "Sky", "Bright", "Swift", "High", "Plain", "Mist", "Sun",
      "Stone", "Wind", "Great", "Cloud", "Blood", "Rune", "Wild", "River" },
    { "horn", "hoof", "mane", "hide", "totem", "runner", "walker", "strider", "born", "hunt", "song",
      "seeker", "mother", "father", "spirit", "bull", "drum", "watcher" }
};

SurnameParts const Gnome = {
    { "Cog", "Gear", "Spark", "Fizz", "Tinker", "Sprocket", "Bolt", "Wobble", "Twist", "Nim", "Copper", "Pinch",
      "Fiddle", "Gizmo", "Tock", "Spin", "Buzz", "Whirl", "Fuse", "Clank" },
    { "spanner", "wrench", "whistle", "cog", "bolt", "sprocket", "gadget", "fuse", "spring", "tock", "spindle",
      "wizzle", "coil", "gear", "nozzle", "bonk", "widget", "sprung", "tinker" }
};

SurnameParts const Troll = {
    { "Shadow", "Blood", "Witch", "Spirit", "Hex", "Jungle", "Bone", "Mojo", "Venom", "Spear", "Tusk", "Zul",
      "Raptor", "Bat", "Snake", "Loa", "Dark", "Wild", "Skull", "Fire" },
    { "tusk", "hex", "mojo", "fang", "spear", "claw", "drum", "eye", "skull", "jinx", "stalker", "hunter",
      "doctor", "dancer", "speaker", "bite", "tooth", "shaker", "caller" }
};

SurnameParts const BloodElf = {
    { "Sun", "Dawn", "Bright", "Ember", "Silver", "Gold", "Blaze", "Flame", "Spell", "Ever", "Crimson", "Star",
      "Phoenix", "Moon", "High", "Rose", "Swift", "Light", "Fel", "Mana" },
    { "strider", "fire", "weaver", "blade", "song", "bloom", "spark", "glow", "wing", "heart", "flare",
      "sorrow", "ward", "thorn", "fury", "spire", "dawn", "bough", "seeker" }
};

SurnameParts const Draenei = {
    { "Light", "Star", "Crystal", "Dawn", "Sun", "Azure", "Prism", "Shine", "Bright", "Exodar", "Naaru",
      "Holy", "Hope", "Ever", "Vigil", "True", "Radiant", "Sky", "Glimmer" },
    { "forge", "ward", "gem", "song", "glow", "born", "seeker", "keeper", "light", "mender", "bringer",
      "shard", "heart", "guard", "sworn", "wright", "ray", "path", "caller" }
};

SurnameParts const* PartsFor(uint8 race)
{
    switch (race)
    {
        case RACE_HUMAN:         return &Human;
        case RACE_ORC:           return &Orc;
        case RACE_DWARF:         return &Dwarf;
        case RACE_NIGHTELF:      return &NightElf;
        case RACE_UNDEAD_PLAYER: return &Undead;
        case RACE_TAUREN:        return &Tauren;
        case RACE_GNOME:         return &Gnome;
        case RACE_TROLL:         return &Troll;
        case RACE_BLOODELF:      return &BloodElf;
        case RACE_DRAENEI:       return &Draenei;
        default:                 return nullptr;
    }
}

std::string DrawSurname(uint8 race)
{
    SurnameParts const* parts = PartsFor(race);
    if (!parts)
        return {};

    for (uint32 draw = 0; draw < SurnameDraws; ++draw)
    {
        std::string const first = parts->first[urand(0, parts->first.size() - 1)];
        std::string const second = parts->second[urand(0, parts->second.size() - 1)];
        if (first.size() == second.size() && std::equal(first.begin(), first.end(), second.begin(),
                [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); }))
            continue;

        std::string const surname = first + second;
        if (ObjectMgr::CheckPlayerName(surname) == CHAR_NAME_SUCCESS)
            return surname;
    }
    return {};
}

// The configured fixed surname, or "Bot" when it is not a single valid name word.
std::string const& FixedSurname()
{
    static std::string checkedFrom;
    static std::string surname;
    std::string const& configured = sPlayerbotAIConfig.coaBotSurnameText;
    if (surname.empty() || checkedFrom != configured)
    {
        checkedFrom = configured;
        if (configured.find(' ') == std::string::npos && ObjectMgr::CheckPlayerName(configured) == CHAR_NAME_SUCCESS)
            surname = configured;
        else
        {
            LOG_ERROR("server.loading", "AiPlayerbot.CoaBotSurnameText \"{}\" is not a valid name word; "
                      "random bots use \"{}\" instead.", configured, DefaultSurname);
            surname = DefaultSurname;
        }
    }
    return surname;
}

std::string FirstName(std::string const& name)
{
    return name.substr(0, name.find(' '));
}

std::string Surname(std::string const& name)
{
    std::size_t const space = name.find(' ');
    return space == std::string::npos ? std::string() : name.substr(space + 1);
}

bool ValidWholeName(std::string const& name)
{
    return ObjectMgr::CheckPlayerName(name) == CHAR_NAME_SUCCESS;
}
}  // namespace

CoaBotSurnameMode CoaBotSurnameSetting()
{
    uint32 const value = sPlayerbotAIConfig.coaBotSurname;
    return value > uint32(CoaBotSurnameMode::Generated) ? CoaBotSurnameMode::Fixed : CoaBotSurnameMode(value);
}

std::string CoaBotName(std::string const& firstName, uint8 race)
{
    CoaBotSurnameMode const mode = CoaBotSurnameSetting();
    if (mode == CoaBotSurnameMode::Off || firstName.find(' ') != std::string::npos)
        return firstName;

    if (mode == CoaBotSurnameMode::Fixed)
    {
        std::string const name = firstName + ' ' + FixedSurname();
        return ValidWholeName(name) ? name : firstName;
    }

    for (uint32 draw = 0; draw < SurnameDraws; ++draw)
    {
        std::string const surname = DrawSurname(race);
        if (surname.empty())
            break;
        std::string const name = firstName + ' ' + surname;
        if (ValidWholeName(name))
            return name;
    }
    return firstName;
}

void ApplyCoaBotSurnames()
{
    // At startup only: a config reload runs this again with the bots online, whose names would
    // then differ from the ones the database and the name cache hold.
    static bool applied = false;
    if (applied)
        return;
    applied = true;

    CoaBotSurnameMode const mode = CoaBotSurnameSetting();

    std::unordered_set<uint32> accounts;
    if (QueryResult result = LoginDatabase.Query("SELECT id FROM account WHERE username LIKE '{}%'",
                                                 sPlayerbotAIConfig.randomBotAccountPrefix))
        do
            accounts.insert(result->Fetch()[0].Get<uint32>());
        while (result->NextRow());
    if (accounts.empty())
        return;

    struct Row { uint32 guid; uint32 account; uint8 race; std::string name; };
    std::vector<Row> rows;
    std::unordered_set<std::string> names;
    if (QueryResult result = CharacterDatabase.Query("SELECT guid, account, race, name FROM characters"))
        do
        {
            Field* fields = result->Fetch();
            rows.push_back({ fields[0].Get<uint32>(), fields[1].Get<uint32>(), fields[2].Get<uint8>(),
                             fields[3].Get<std::string>() });
            names.insert(rows.back().name);
        } while (result->NextRow());

    std::string const fixed = mode == CoaBotSurnameMode::Fixed ? FixedSurname() : std::string();
    uint32 renamed = 0, skipped = 0;
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    for (Row const& row : rows)
    {
        if (!accounts.count(row.account))
            continue;

        std::string const first = FirstName(row.name);
        std::string const current = Surname(row.name);
        std::string renamedTo;
        switch (mode)
        {
            case CoaBotSurnameMode::Off:
                renamedTo = first;
                break;
            case CoaBotSurnameMode::Fixed:
                renamedTo = first + ' ' + fixed;
                break;
            case CoaBotSurnameMode::Generated:
                // A surname of its own stays; the fixed one, or none, makes way for a drawn one.
                if (!current.empty() && current != DefaultSurname && current != sPlayerbotAIConfig.coaBotSurnameText)
                    continue;
                for (uint32 draw = 0; draw < SurnameDraws; ++draw)
                {
                    std::string const candidate = CoaBotName(first, row.race);
                    if (candidate != first && !names.count(candidate))
                    {
                        renamedTo = candidate;
                        break;
                    }
                }
                if (renamedTo.empty())
                {
                    ++skipped;
                    continue;
                }
                break;
        }

        if (renamedTo == row.name)
            continue;

        // A player, or another bot, may hold the name already: that bot keeps its own.
        if (names.count(renamedTo) || (renamedTo != first && !ValidWholeName(renamedTo)))
        {
            ++skipped;
            continue;
        }

        names.erase(row.name);
        names.insert(renamedTo);
        sCharacterCache->UpdateCharacterData(ObjectGuid::Create<HighGuid::Player>(row.guid), renamedTo);
        CharacterDatabase.EscapeString(renamedTo);
        trans->Append("UPDATE characters SET name = '{}' WHERE guid = {}", renamedTo, row.guid);
        ++renamed;
    }
    CharacterDatabase.CommitTransaction(trans);

    if (renamed || skipped)
        LOG_INFO("server.loading",
                 ">> Surnames of random bots ({}): {} renamed, {} kept their names (taken or invalid)",
                 mode == CoaBotSurnameMode::Off ? "off" : mode == CoaBotSurnameMode::Fixed ? fixed : "generated",
                 renamed, skipped);
}
