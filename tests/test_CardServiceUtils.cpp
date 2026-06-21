#include <gtest/gtest.h>

import CardService;
import Character;

namespace mc {
namespace {

    TEST(ToAttackTypeTest, Rock_MapsTo_AttackTypeRock) {
        EXPECT_EQ(ToAttackType(ECardType::Rock), EAttackType::Rock);
    }

    TEST(ToAttackTypeTest, Scissors_MapsTo_AttackTypeScissors) {
        EXPECT_EQ(ToAttackType(ECardType::Scissors), EAttackType::Scissors);
    }

    TEST(ToAttackTypeTest, Paper_MapsTo_AttackTypePaper) {
        EXPECT_EQ(ToAttackType(ECardType::Paper), EAttackType::Paper);
    }

    TEST(ToAttackTypeTest, Magic_ThrowsInvalidArgument) {
        EXPECT_THROW(ToAttackType(ECardType::Magic), std::invalid_argument);
    }

    TEST(ToAttackTypeTest, Null_ThrowsInvalidArgument) {
        EXPECT_THROW(ToAttackType(ECardType::Null), std::invalid_argument);
    }

} // namespace
} // namespace mc
