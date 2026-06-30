#include <gtest/gtest.h>
#include <algorithm>

import Enemy;
import Player;
import Character;
import HealthComponent;
import EventBus;

namespace mc {
namespace {

    class EnemyTest : public ::testing::Test {
    protected:
        void SetUp() override {
        }
        void TearDown() override {
        }
    };

    TEST_F(EnemyTest, MagicEvent_Clairvoyance_SetsIsExposed) {
        Enemy enemy;

        enemy.SetExposed(true);

        EXPECT_TRUE(enemy.IsExposed());
    }

    TEST_F(EnemyTest, CombatEvent_TakesDamageWhenLosing) {
        Enemy enemy;
        int initialHealth = enemy.GetHealthComponent().GetHealth();

        enemy.TakeDamage(5);

        EXPECT_EQ(enemy.GetHealthComponent().GetHealth(), initialHealth - 5);
    }

    TEST_F(EnemyTest, AddWeightEvent_IncreasesWeight) {
        Enemy enemy;

        enemy.AddWeight(EAttackType::Rock, 10);

        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Rock), 10);
    }

    TEST_F(EnemyTest, CombatEvent_ResetsWeightOffsets) {
        Enemy enemy;

        enemy.AddWeight(EAttackType::Rock, 10);
        
        enemy.ResetWeights();

        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Rock), 0);
    }

    TEST_F(EnemyTest, Enemy_HasEnemyTag) {
        Enemy enemy;

        const auto& tags = enemy.GetTags();
        auto it = std::find(tags.begin(), tags.end(), ETag::Enemy);
        EXPECT_NE(it, tags.end());
    }

    TEST_F(EnemyTest, AddWeight_AllTypes_AffectCorrectOffset) {
        Enemy enemy;

        enemy.AddWeight(EAttackType::Rock,     5);
        enemy.AddWeight(EAttackType::Scissors, 7);
        enemy.AddWeight(EAttackType::Paper,    9);

        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Rock),     5);
        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Scissors), 7);
        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Paper),    9);
    }

    TEST_F(EnemyTest, GetBaseWeight_ReturnsConstructorValue) {
        Enemy enemy(42, 1, 1, 1, L"TestEnemy");

        // In _DEBUG builds, Enemy::Enemy() forces baseWeight = 1 regardless of the
        // constructor argument. In release, the constructor argument is used.
#ifdef _DEBUG
        EXPECT_EQ(enemy.GetBaseWeight(), 1);
#else
        EXPECT_EQ(enemy.GetBaseWeight(), 42);
#endif
    }

    TEST_F(EnemyTest, EqualityOperator_SameAttributes_ReturnsTrue) {
        Enemy a(10, 1, 2, 3, L"Wolf");
        Enemy b(10, 1, 2, 3, L"Wolf");

        EXPECT_TRUE(a == b);
    }

    TEST_F(EnemyTest, EqualityOperator_DifferentName_ReturnsFalse) {
        Enemy a(10, 1, 2, 3, L"Wolf");
        Enemy b(10, 1, 2, 3, L"Bunny");

        EXPECT_FALSE(a == b);
    }

    TEST_F(EnemyTest, EqualityOperator_DifferentDamage_ReturnsFalse) {
        Enemy a(10, 1, 2, 3, L"Wolf");
        Enemy b(10, 9, 2, 3, L"Wolf");

        EXPECT_FALSE(a == b);
    }

    TEST_F(EnemyTest, GetAttackIntent_BalancedWeights_ReturnsValidType) {
        // baseWeight = 1 for each type, so all three are equally probable
        Enemy enemy(1, 0, 0, 0, L"BalancedEnemy");

        EAttackType result = enemy.GetAttackIntent();

        EXPECT_TRUE(result == EAttackType::Rock ||
                    result == EAttackType::Scissors ||
                    result == EAttackType::Paper);
    }

    TEST_F(EnemyTest, ResetWeights_ClearsAllOffsets) {
        Enemy enemy;

        enemy.AddWeight(EAttackType::Rock,     10);
        enemy.AddWeight(EAttackType::Scissors,  5);
        enemy.AddWeight(EAttackType::Paper,      3);

        enemy.ResetWeights();

        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Rock),     0);
        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Scissors), 0);
        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Paper),    0);
    }

} // namespace
} // namespace mc
