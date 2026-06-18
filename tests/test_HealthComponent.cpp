#include <gtest/gtest.h>

import HealthComponent;
import Character;
import EventBus;

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
    };

    TEST(HealthComponentTest, TakeDamage_ReducesHealth) {
        DummyCharacter character;
        HealthComponent health(&character);

        EXPECT_EQ(health.GetHealth(), 10);
        health.TakeDamage(3);
        EXPECT_EQ(health.GetHealth(), 7);
    }

    TEST(HealthComponentTest, TakeDamage_FiresHealthChangedEvent) {
        DummyCharacter character;
        HealthComponent health(&character);

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
        HealthComponent health(&character);

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

} // namespace
} // namespace mc
