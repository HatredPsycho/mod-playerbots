/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "MailTakeRules.h"
#include "gtest/gtest.h"

namespace
{
Mail MakeMail(uint32 money, uint32 itemCount)
{
    Mail mail{};
    mail.money = money;
    for (uint32 i = 1; i <= itemCount; ++i)
        mail.AddItem(i, 25);

    return mail;
}
}

TEST(MailTakeRules, ItemsLeftBehindByFullBagsKeepTheMail)
{
    Mail const mail = MakeMail(0, 1);
    EXPECT_FALSE(CanRemoveAfterTake(mail, true));
}

TEST(MailTakeRules, ItemsLeftAfterTheMoneyWasTakenKeepTheMail)
{
    Mail mail = MakeMail(5000, 2);
    bool const hadContent = HasTakeableContent(mail);
    mail.money = 0;
    EXPECT_FALSE(CanRemoveAfterTake(mail, hadContent));
}

TEST(MailTakeRules, MoneyLeftBehindKeepsTheMail)
{
    Mail const mail = MakeMail(5000, 0);
    EXPECT_FALSE(CanRemoveAfterTake(mail, true));
}

TEST(MailTakeRules, FullyTakenMailIsRemoved)
{
    Mail mail = MakeMail(5000, 2);
    bool const hadContent = HasTakeableContent(mail);
    mail.money = 0;
    mail.RemoveItem(1);
    mail.RemoveItem(2);
    EXPECT_TRUE(CanRemoveAfterTake(mail, hadContent));
}

TEST(MailTakeRules, TextOnlyMailIsKept)
{
    Mail const mail = MakeMail(0, 0);
    EXPECT_FALSE(CanRemoveAfterTake(mail, HasTakeableContent(mail)));
}
