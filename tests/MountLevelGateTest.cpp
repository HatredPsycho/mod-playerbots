/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MountLevelGate.h"
#include "gtest/gtest.h"

namespace
{
constexpr std::uint32_t GroundMountMinLevel = 20;
constexpr std::uint32_t AltLevel = 10;
}

TEST(MountLevelGateTest, BotAtMinimumLevelPasses)
{
    EXPECT_TRUE(MountLevelGateAllows(GroundMountMinLevel, GroundMountMinLevel, false));
}

TEST(MountLevelGateTest, LowLevelBotOnFootStops)
{
    EXPECT_FALSE(MountLevelGateAllows(AltLevel, GroundMountMinLevel, false));
}

TEST(MountLevelGateTest, LowLevelBotStillMountedPassesToDismount)
{
    EXPECT_TRUE(MountLevelGateAllows(AltLevel, GroundMountMinLevel, true));
}
