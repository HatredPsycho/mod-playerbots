/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "CoaCompanionOrder.h"
#include "gtest/gtest.h"

namespace
{
constexpr std::uint32_t SkeletalRogue = 500969;
constexpr std::uint32_t Abomination = 500335;
constexpr std::uint32_t Ghoul = 500971;
constexpr std::uint32_t MechanoBear = 803405;
constexpr std::uint32_t Scrapmaw = 500242;

CoaCompanionCandidate Listed(std::uint32_t spell, float priority, std::uint8_t cost)
{
    return { spell, true, priority, cost, false };
}

CoaCompanionCandidate Unlisted(std::uint32_t spell, std::uint8_t cost)
{
    return { spell, false, 0.0f, cost, false };
}

CoaCompanionCandidate Pet(std::uint32_t spell, float priority)
{
    return { spell, true, priority, 0, true };
}
}

TEST(CoaCompanionSummon, RaiseOccupyingLifeForceCountsThoughItsPropertiesAreWild)
{
    EXPECT_TRUE(IsCoaCompanionSummon(1, false));
}

TEST(CoaCompanionSummon, FollowerPropertiesCountWithoutLifeForce)
{
    EXPECT_TRUE(IsCoaCompanionSummon(0, true));
}

TEST(CoaCompanionSummon, WildSummonWithoutLifeForceDoesNotCount)
{
    EXPECT_FALSE(IsCoaCompanionSummon(0, false));
}

TEST(CoaCompanionWanted, RaiseStaysWantedThoughOneOfItsMinionsStands)
{
    EXPECT_TRUE(IsCoaCompanionWanted(1, true));
}

TEST(CoaCompanionWanted, CompanionWithoutLifeForceIsWantedOnlyUntilItStands)
{
    EXPECT_TRUE(IsCoaCompanionWanted(0, false));
    EXPECT_FALSE(IsCoaCompanionWanted(0, true));
}

TEST(CoaCompanionOrder, RaisesRepeatUntilLifeForceIsSpent)
{
    std::vector<CoaCompanionCandidate> const known = { Listed(Abomination, 31.0f, 3), Unlisted(Ghoul, 1) };
    std::uint8_t freeLifeForce = 5;
    std::vector<std::uint32_t> raised;
    while (std::uint32_t const spell = ChooseCoaCompanion(known, freeLifeForce, false))
    {
        raised.push_back(spell);
        freeLifeForce -= spell == Abomination ? 3 : 1;
    }

    EXPECT_EQ(raised, (std::vector<std::uint32_t>{ Abomination, Ghoul, Ghoul }));
    EXPECT_EQ(freeLifeForce, 0);
}

TEST(CoaCompanionOrder, HighestRotationPriorityComesFirst)
{
    EXPECT_EQ(ChooseCoaCompanion({ Listed(Abomination, 30.0f, 3), Listed(SkeletalRogue, 31.0f, 1) }, 4, false),
              SkeletalRogue);
}

TEST(CoaCompanionOrder, RotationLinesComeBeforeWhatTheRotationDoesNotName)
{
    EXPECT_EQ(ChooseCoaCompanion({ Unlisted(Ghoul, 1), Listed(Abomination, 2.0f, 3) }, 4, false), Abomination);
}

TEST(CoaCompanionOrder, UnlistedCompanionsKeepTheirOrder)
{
    EXPECT_EQ(ChooseCoaCompanion({ Unlisted(Abomination, 3), Unlisted(Ghoul, 1) }, 4, false), Abomination);
}

TEST(CoaCompanionOrder, CompanionThatDoesNotFitIsPassedOver)
{
    EXPECT_EQ(ChooseCoaCompanion({ Listed(Abomination, 31.0f, 3), Listed(SkeletalRogue, 30.0f, 1) }, 1, false),
              SkeletalRogue);
}

TEST(CoaCompanionOrder, FullLifeForceEvictsNobody)
{
    EXPECT_EQ(ChooseCoaCompanion({ Listed(Abomination, 31.0f, 3), Unlisted(Ghoul, 1) }, 0, false), 0u);
}

TEST(CoaCompanionOrder, StandingPetEndsThePetSpells)
{
    EXPECT_EQ(ChooseCoaCompanion({ Pet(MechanoBear, 9.0f), Pet(Scrapmaw, 5.0f) }, 0, true), 0u);
}

TEST(CoaCompanionOrder, NoPetTakesTheHighestPetSpell)
{
    EXPECT_EQ(ChooseCoaCompanion({ Pet(Scrapmaw, 5.0f), Pet(MechanoBear, 9.0f) }, 0, false), MechanoBear);
}
