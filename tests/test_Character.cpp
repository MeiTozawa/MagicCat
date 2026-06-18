#include <gtest/gtest.h>

import Character;

namespace mc {
namespace {

    class DummyCharacter : public Character {
    public:
        DummyCharacter() {
            name = L"Dummy";
            rockDamage = 1;
            scissorsDamage = 2;
            paperDamage = 3;
            sprite = ESprite::Null;
        }
    };

    TEST(CharacterTest, LosesTo_LogicCheck) {
        // Rock loses to Paper
        EXPECT_TRUE(LosesTo(EAttackType::Rock, EAttackType::Paper));
        // Scissors loses to Rock
        EXPECT_TRUE(LosesTo(EAttackType::Scissors, EAttackType::Rock));
        // Paper loses to Scissors
        EXPECT_TRUE(LosesTo(EAttackType::Paper, EAttackType::Scissors));

        // False cases
        EXPECT_FALSE(LosesTo(EAttackType::Rock, EAttackType::Scissors));
        EXPECT_FALSE(LosesTo(EAttackType::Rock, EAttackType::Rock));
        EXPECT_FALSE(LosesTo(EAttackType::Paper, EAttackType::Rock));
    }

    TEST(CharacterTest, GetDamage_ReturnsCorrectDamage) {
        DummyCharacter c;
        
        EXPECT_EQ(c.GetDamage(EAttackType::Rock), 1);
        EXPECT_EQ(c.GetDamage(EAttackType::Scissors), 2);
        EXPECT_EQ(c.GetDamage(EAttackType::Paper), 3);
    }

} // namespace
} // namespace mc
