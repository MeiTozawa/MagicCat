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

    TEST(ToAttackTypeTest, Magic_FallsBackTo_AttackTypeRock) {
        // Magic has no EAttackType counterpart; implementation falls back to Rock
        EXPECT_EQ(ToAttackType(ECardType::Magic), EAttackType::Rock);
    }

    TEST(ToAttackTypeTest, Null_FallsBackTo_AttackTypeRock) {
        // Null has no EAttackType counterpart; implementation falls back to Rock
        EXPECT_EQ(ToAttackType(ECardType::Null), EAttackType::Rock);
    }

} // namespace
} // namespace mc
