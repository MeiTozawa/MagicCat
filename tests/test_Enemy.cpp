#include <gtest/gtest.h>

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

} // namespace
} // namespace mc
