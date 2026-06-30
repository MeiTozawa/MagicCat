#include <gtest/gtest.h>

import HealthComponent;
import Character;
import EventBus;
import AssetService;

namespace mc {
namespace {

    class DummyCharacter : public Character {
    public:
        DummyCharacter() {
            name = L"Dummy";
            rockDamage = 1;
            scissorsDamage = 1;
            paperDamage = 1;
            sprite = ESprite::Null;
        }
        void TakeDamage(int) const override {}
    };

    TEST(HealthComponentTest, TakeDamage_ReducesHealth) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        EXPECT_EQ(health.GetHealth(), 10);
        health.TakeDamage(3);
        EXPECT_EQ(health.GetHealth(), 7);
    }

    TEST(HealthComponentTest, TakeDamage_FiresHealthChangedEvent) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        bool eventFired = false;
        int receivedHealth = -1;
        auto handle = EventBus::Subscribe<HealthChangedEvent>([&](const HealthChangedEvent& e) {
            eventFired = true;
            receivedHealth = e.CurrentHealth;
        });

        health.TakeDamage(4);

        EXPECT_TRUE(eventFired);
        EXPECT_EQ(receivedHealth, 6);

        EventBus::Unsubscribe(handle);
    }

    TEST(HealthComponentTest, TakeDamage_Fatal_FiresDeathEvent) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        bool deathEventFired = false;
        auto handle = EventBus::Subscribe<DeathEvent>([&](const DeathEvent& e) {
            deathEventFired = true;
            EXPECT_EQ(e.Victim, &character);
        });

        EXPECT_FALSE(health.IsDead());
        
        health.TakeDamage(10); // initial health is 10

        EXPECT_TRUE(health.IsDead());
        EXPECT_TRUE(deathEventFired);

        EventBus::Unsubscribe(handle);
    }

    TEST(HealthComponentTest, TakeDamage_WhenAlreadyDead_DoesNothing) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        health.TakeDamage(10); // Kill the character
        ASSERT_TRUE(health.IsDead());
        EXPECT_EQ(health.GetHealth(), 0);

        health.TakeDamage(5); // Should be ignored after death
        EXPECT_EQ(health.GetHealth(), 0); // HP should not go negative
    }

    TEST(HealthComponentTest, GetMaxHealth_ReturnsDefaultTen) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        EXPECT_EQ(health.GetMaxHealth(), 10);
    }

    // --- Reset tests (Requirement 2.4) ---

    TEST(HealthComponentTest, Reset_SetsHealthToMaxHp) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        health.Reset(25);

        EXPECT_EQ(health.GetHealth(), 25);
        EXPECT_EQ(health.GetMaxHealth(), 25);
    }

    TEST(HealthComponentTest, Reset_ClearsDeadState) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        // Kill the character first
        health.TakeDamage(10);
        ASSERT_TRUE(health.IsDead());

        health.Reset(10);

        EXPECT_FALSE(health.IsDead());
    }

    TEST(HealthComponentTest, Reset_AfterPartialDamage_RestoresFullHealth) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        health.TakeDamage(6);
        ASSERT_EQ(health.GetHealth(), 4);

        health.Reset(20);

        EXPECT_EQ(health.GetHealth(), 20);
        EXPECT_FALSE(health.IsDead());
    }

    TEST(HealthComponentTest, Reset_DoesNotFireDeathEvent) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        // Kill first so Reset has something meaningful to reset
        health.TakeDamage(10);
        ASSERT_TRUE(health.IsDead());

        bool deathEventFired = false;
        auto handle = EventBus::Subscribe<DeathEvent>([&](const DeathEvent&) {
            deathEventFired = true;
        });

        health.Reset(10);

        EXPECT_FALSE(deathEventFired);

        EventBus::Unsubscribe(handle);
    }

    TEST(HealthComponentTest, Reset_DoesNotFireHealthChangedEvent) {
        DummyCharacter character;
        HealthComponent health(&character, 10);

        bool healthChangedFired = false;
        auto handle = EventBus::Subscribe<HealthChangedEvent>([&](const HealthChangedEvent&) {
            healthChangedFired = true;
        });

        health.Reset(15);

        EXPECT_FALSE(healthChangedFired);

        EventBus::Unsubscribe(handle);
    }

} // namespace
} // namespace mc
