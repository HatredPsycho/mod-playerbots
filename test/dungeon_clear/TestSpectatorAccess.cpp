/*
 * The spectator camera is a game master's tool: players and moderators are refused, game masters
 * and administrators are not.
 */

#include "gtest/gtest.h"

#include "Util/DcSpectator.h"

TEST(DcSpectatorAccess, OnlyGameMastersMaySpectate)
{
    EXPECT_FALSE(DcSpectator::MaySpectate(SEC_PLAYER));
    EXPECT_FALSE(DcSpectator::MaySpectate(SEC_MODERATOR));
    EXPECT_TRUE(DcSpectator::MaySpectate(SEC_GAMEMASTER));
    EXPECT_TRUE(DcSpectator::MaySpectate(SEC_ADMINISTRATOR));
    EXPECT_TRUE(DcSpectator::MaySpectate(SEC_CONSOLE));
}
