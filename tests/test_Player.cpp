#include <gtest/gtest.h>

import Player;
import Character;
import HealthComponent;
import EventBus;

namespace mc {
namespace {

    class PlayerTest : public ::testing::Test {
    protected:
        void SetUp() override {
            // Ensure any previous events are cleared or reset if necessary
        }
        void TearDown() override {
        }
    };

    TEST_F(PlayerTest, ChangeMp_ClampsBetweenZeroAndMax) {
        Player player;
        
        int initialMp = player.GetMp();
        int maxMp = player.GetMaxMp();

        // Drain MP completely
        player.ChangeMp(-100);
        EXPECT_EQ(player.GetMp(), 0);

        // Overfill MP
        player.ChangeMp(100);
        EXPECT_EQ(player.GetMp(), maxMp);
    }

    TEST_F(PlayerTest, UseMagic_ConsumesMpAndFiresEvent) {
        Player player;
        
        // Ensure we have enough MP
        player.ChangeMp(100);
        int initialMp = player.GetMp();

        bool magicEventFired = false;
        auto handle = EventBus::Subscribe<MagicEvent>([&](const MagicEvent& e) {
            magicEventFired = true;
            EXPECT_EQ(e.magic, EMagic::Clairvoyance);
        });

        player.UseMagic(EMagic::Clairvoyance);

        EXPECT_TRUE(magicEventFired);
        EXPECT_EQ(player.GetMp(), initialMp - 5); // CLAIRVOYANCE_MP_COST is 5

        EventBus::Unsubscribe(handle);
    }

    TEST_F(PlayerTest, UseMagic_LacksMp_FiresLackOfMpEvent) {
        Player player;

        // Drain MP completely
        player.ChangeMp(-100);
        
        bool lackOfMpEventFired = false;
        auto handle = EventBus::Subscribe<LackOfMpEvent>([&](const LackOfMpEvent& e) {
            lackOfMpEventFired = true;
        });

        player.UseMagic(EMagic::Clairvoyance);

        EXPECT_TRUE(lackOfMpEventFired);
        EXPECT_EQ(player.GetMp(), 0);

        EventBus::Unsubscribe(handle);
    }

    TEST_F(PlayerTest, TakeDamage_ReducesHealth) {
        Player player;
        int initialHealth = player.GetHealthComponent().GetHealth();

        player.TakeDamage(3);

        int currentHealth = player.GetHealthComponent().GetHealth();
        EXPECT_EQ(currentHealth, initialHealth - 3);
    }

} // namespace
} // namespace mc
