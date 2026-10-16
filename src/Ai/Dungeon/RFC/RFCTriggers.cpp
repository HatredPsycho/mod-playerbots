/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RFCTriggers.h"
#include "AiObjectContext.h"
#include "Playerbots.h"

namespace RagefireEscort
{
    Unit* Nearest(PlayerbotAI* botAI, Player* bot, AiObjectContext* context, std::string const& bossName,
                  std::initializer_list<uint32> entries)
    {
        Unit* boss = AI_VALUE2(Unit*, "find target", bossName);
        if (!boss)
            return nullptr;

        GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");
        for (uint32 entry : entries)
        {
            for (auto& guid : targets)
            {
                Unit* unit = botAI->GetUnit(guid);
                if (!unit || unit->GetEntry() != entry || !unit->IsAlive() || !unit->isTargetableForAttack())
                    continue;

                if (unit->GetExactDist2d(boss->GetPosition()) <= RFC_ESCORT_RANGE)
                    return unit;
            }
        }

        return nullptr;
    }

    bool StillUp(PlayerbotAI* botAI, Player* bot, AiObjectContext* context, std::string const& bossName,
                 std::initializer_list<uint32> entries)
    {
        if (botAI->IsTank(bot))
            return false;

        return Nearest(botAI, bot, context, bossName, entries) != nullptr;
    }
}

namespace TaragamanPillar
{
    Unit* Boss(PlayerbotAI* botAI, AiObjectContext* context)
    {
        Unit* boss = AI_VALUE2(Unit*, "find target", "taragaman the hungerer");
        return boss && boss->IsAlive() ? boss : nullptr;
    }

    Creature* Find(Player* bot)
    {
        Creature* pillar = bot->FindNearestCreature(NPC_RUMBLING_EARTH, EARTH_PILLAR_SEARCH);
        return pillar && pillar->IsAlive() ? pillar : nullptr;
    }

    bool ShouldHide(PlayerbotAI* botAI, Player* bot, AiObjectContext* context)
    {
        Unit* boss = Boss(botAI, context);
        if (!boss || !Find(bot))
            return false;

        // The tank keeps Taragaman where he is until the nova is actually being cast; leaving
        // earlier would only drag him along to the pillar.
        if (botAI->IsTank(bot))
            return boss->HasUnitState(UNIT_STATE_CASTING) && boss->FindCurrentSpellBySpellId(SPELL_MASSIVE_FIRE_NOVA);

        return true;
    }

    Position CoverSpot(Unit* boss, Creature* pillar)
    {
        float dx = pillar->GetPositionX() - boss->GetPositionX();
        float dy = pillar->GetPositionY() - boss->GetPositionY();
        float const length = std::sqrt(dx * dx + dy * dy);
        if (length < 0.1f)
        {
            dx = std::cos(pillar->GetOrientation());
            dy = std::sin(pillar->GetOrientation());
        }
        else
        {
            dx /= length;
            dy /= length;
        }

        float x = pillar->GetPositionX() + dx * EARTH_PILLAR_COVER;
        float y = pillar->GetPositionY() + dy * EARTH_PILLAR_COVER;
        float z = pillar->GetPositionZ();
        pillar->UpdateAllowedPositionZ(x, y, z);
        return Position(x, y, z, std::atan2(-dy, -dx));
    }
}

bool OggleflintAddsTrigger::IsActive()
{
    // The Shaman is named first on purpose: it is the one that heals, and the action takes the
    // first entry it finds alive.
    return RagefireEscort::StillUp(botAI, bot, context, "oggleflint",
                                   {NPC_RAGEFIRE_SHAMAN, NPC_RAGEFIRE_TROGG});
}

bool JergoshAddsTrigger::IsActive()
{
    return RagefireEscort::StillUp(botAI, bot, context, "jergosh the invoker",
                                   {NPC_SEARING_BLADE_CULTIST});
}

bool BazzalanDpsTrigger::IsActive()
{
    // Bazzalan is the opposite case: he hits far harder than his Cultists, so he goes down first.
    Unit* boss = AI_VALUE2(Unit*, "find target", "bazzalan");
    if (!boss || !boss->isTargetableForAttack())
        return false;

    return !botAI->IsTank(bot);
}

bool TaragamanFireNovaTrigger::IsActive()
{
    if (!botAI->IsRanged(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "taragaman the hungerer");
    if (!boss || !boss->IsAlive())
        return false;

    if (TaragamanPillar::ShouldHide(botAI, bot, context))
        return false;

    return bot->GetExactDist2d(boss->GetPosition()) < FIRE_NOVA_RADIUS + FIRE_NOVA_SAFETY;
}

bool TaragamanMassiveFireNovaTrigger::IsActive()
{
    return TaragamanPillar::ShouldHide(botAI, bot, context);
}
