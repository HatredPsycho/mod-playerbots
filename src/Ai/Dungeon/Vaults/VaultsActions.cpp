/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "VaultsActions.h"
#include "Playerbots.h"
#include "VaultsShared.h"

#include <cmath>

using namespace VaultsShared;

namespace
{
constexpr float CLICK_RANGE = 3.0f;
constexpr float SHELTER_SPREAD = 2.0f;
}

bool VaultsLeaveHazardAction::Execute(Event /*event*/)
{
    Hazards const hazards = CollectHazards(bot);
    if (!hazards.Contains(bot->GetPositionX(), bot->GetPositionY()))
        return false;

    // Already on the way to a spot that is still safe: no new search.
    Position spot;
    if (KeepRunning(bot, hazards, spot))
        return true;

    Circle hold;
    bool const holding = HoldCircle(bot, hold);
    if (!FindSafeSpot(bot, hazards, spot, holding ? &hold : nullptr) &&
        (!holding || !FindSafeSpot(bot, hazards, spot)) && !AwayFromHazard(bot, hazards, spot))
        return false;

    RememberRun(bot, spot);
    bot->CastStop();
    return MoveTo(bot->GetMapId(), spot.GetPositionX(), spot.GetPositionY(), spot.GetPositionZ(), false, false, false,
                  true, MovementPriority::MOVEMENT_COMBAT, true);
}

// The soak circle comes under a random player; when it is not that one's, the nearest bot steps in.
bool VaultsSoakAction::Execute(Event /*event*/)
{
    Creature* soak = SoakToTake(bot);
    if (!soak)
        return false;
    return MoveTo(bot->GetMapId(), soak->GetPositionX(), soak->GetPositionY(), soak->GetPositionZ(), false, false,
                  false, true, MovementPriority::MOVEMENT_COMBAT, true);
}

// Each bot takes its own place in the shelter, so they do not all stack on the middle.
bool VaultsTakeShelterAction::Execute(Event /*event*/)
{
    Creature* shelter = ShelterToReach(bot);
    if (!shelter)
        return false;
    float const angle = float(bot->GetGUID().GetCounter() % 8) * float(M_PI) / 4.0f;
    float x = shelter->GetPositionX() + SHELTER_SPREAD * std::cos(angle);
    float y = shelter->GetPositionY() + SHELTER_SPREAD * std::sin(angle);
    float z = shelter->GetPositionZ();
    shelter->GetMap()->CheckCollisionAndGetValidCoords(shelter, shelter->GetPositionX(), shelter->GetPositionY(),
                                                       shelter->GetPositionZ(), x, y, z);
    return MoveTo(bot->GetMapId(), x, y, z, false, false, false, true, MovementPriority::MOVEMENT_COMBAT, true);
}

bool VaultsClickCrystalAction::Click(Creature* crystal)
{
    if (!crystal)
        return false;
    if (bot->GetExactDist2d(crystal) > CLICK_RANGE)
        return MoveTo(crystal, CLICK_RANGE - 1.0f, MovementPriority::MOVEMENT_COMBAT);

    bot->StopMoving();
    bot->SetFacingToObject(crystal);
    return crystal->HandleSpellClick(bot);
}

// The Holy Crystal in the fight leaves the shelter against Void Wrath where it stood.
bool VaultsClickCrystalAction::Execute(Event /*event*/)
{
    return Click(FightCrystalToClick(bot));
}

// A seal crystal breaks after a 3 s channel.
bool VaultsBreakSealAction::Execute(Event /*event*/)
{
    return Click(SealCrystalToBreak(bot));
}
