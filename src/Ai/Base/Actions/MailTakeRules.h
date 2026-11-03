/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_MAILTAKERULES_H
#define PLAYERBOTS_MAILTAKERULES_H

#include "Mail.h"

[[nodiscard]] inline bool HasTakeableContent(Mail const& mail)
{
    return mail.money || mail.HasItems();
}

[[nodiscard]] inline bool CanRemoveAfterTake(Mail const& mailAfterTake, bool hadTakeableContent)
{
    return hadTakeableContent && !HasTakeableContent(mailAfterTake);
}

#endif
