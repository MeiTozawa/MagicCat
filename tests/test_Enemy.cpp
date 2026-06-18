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
        
        EXPECT_FALSE(enemy.IsExposed());

        EventBus::Publish<MagicEvent>({EMagic::Clairvoyance});

        EXPECT_TRUE(enemy.IsExposed());
    }

    TEST_F(EnemyTest, CombatEvent_TakesDamageWhenLosing) {
        Enemy enemy(0, 1, 1, 1, L"Test", ESprite::Null);
        int initialHealth = enemy.GetHealthComponent().GetHealth();

        // Player plays Rock, Enemy plays Scissors -> Enemy loses
        EventBus::Publish<CombatEvent>({EAttackType::Rock, EAttackType::Scissors, 5, 2});

        // Should take 5 damage
        EXPECT_EQ(enemy.GetHealthComponent().GetHealth(), initialHealth - 5);
    }

    TEST_F(EnemyTest, AddWeightEvent_IncreasesWeight) {
        Enemy enemy;
        
        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Rock), 0);

        EventBus::Publish<AddWeightEvent>({EAttackType::Rock, 10});

        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Rock), 10);
    }

    TEST_F(EnemyTest, CombatEvent_ResetsWeightOffsets) {
        Enemy enemy;
        EventBus::Publish<AddWeightEvent>({EAttackType::Rock, 10});
        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Rock), 10);

        EventBus::Publish<CombatEvent>({EAttackType::Rock, EAttackType::Scissors, 1, 1});

        EXPECT_EQ(enemy.GetWeightOffset(EAttackType::Rock), 0);
    }

} // namespace
} // namespace mc
