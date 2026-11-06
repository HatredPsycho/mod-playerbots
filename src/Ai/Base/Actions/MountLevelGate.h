/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MOUNTLEVELGATE_H
#define PLAYERBOTS_MOUNTLEVELGATE_H

#include <cstdint>

[[nodiscard]] inline bool MountLevelGateAllows(std::uint32_t level, std::uint32_t minLevel, bool mounted)
{
    return level >= minLevel || mounted;
}

#endif
