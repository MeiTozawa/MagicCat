#include <gtest/gtest.h>

import Character;
import AssetService;

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
        
        EXPECT_EQ(c.GetBaseDamage(EAttackType::Rock), 1);
        EXPECT_EQ(c.GetBaseDamage(EAttackType::Scissors), 2);
        EXPECT_EQ(c.GetBaseDamage(EAttackType::Paper), 3);
    }

    TEST(CharacterTest, GetName_ReturnsCorrectName) {
        DummyCharacter c;
        EXPECT_EQ(c.GetName(), L"Dummy");
    }

    TEST(CharacterTest, GetTags_IsEmptyForBaseCharacter) {
        DummyCharacter c;
        // Base DummyCharacter adds no tags
        EXPECT_TRUE(c.GetTags().empty());
    }

    TEST(CharacterTest, LosesTo_DrawCases_ReturnFalse) {
        // Same type is a draw — should never lose to itself
        EXPECT_FALSE(LosesTo(EAttackType::Rock,     EAttackType::Rock));
        EXPECT_FALSE(LosesTo(EAttackType::Scissors, EAttackType::Scissors));
        EXPECT_FALSE(LosesTo(EAttackType::Paper,    EAttackType::Paper));
    }

    TEST(CharacterTest, LosesTo_WinCases_ReturnFalse) {
        // Winning cases: should return false (these do NOT lose)
        EXPECT_FALSE(LosesTo(EAttackType::Rock,     EAttackType::Scissors));
        EXPECT_FALSE(LosesTo(EAttackType::Scissors, EAttackType::Paper));
        EXPECT_FALSE(LosesTo(EAttackType::Paper,    EAttackType::Rock));
    }

} // namespace
} // namespace mc
