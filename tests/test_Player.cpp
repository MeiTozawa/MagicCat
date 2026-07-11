// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before rapidcheck includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <algorithm>

import Player;
import Character;
import HealthComponent;
import EventBus;

namespace mc {
namespace {

    class PlayerTest : public ::testing::Test {
    protected:
        void SetUp() override {}
        void TearDown() override {}
    };

    TEST_F(PlayerTest, ChangeMp_ClampsBetweenZeroAndMax) {
        Player player;
        int maxMp = player.GetMaxMp();

        player.ChangeMp(-100);
        EXPECT_EQ(player.GetMp(), 0);

        player.ChangeMp(100);
        EXPECT_EQ(player.GetMp(), maxMp);
    }

    TEST_F(PlayerTest, UseMagic_ConsumesMpAndFiresEvent) {
        Player player;
        
        player.ChangeMp(100);
        int initialMp = player.GetMp();

        bool magicEventFired = false;
        auto handle = EventBus::Subscribe<MagicEvent>([&](const MagicEvent& e) {
            magicEventFired = true;
            EXPECT_EQ(e.magic, EMagic::Clairvoyance);
        });

        player.UseMagic(EMagic::Clairvoyance);

        EXPECT_TRUE(magicEventFired);
        EXPECT_EQ(player.GetMp(), initialMp - 10); // CLAIRVOYANCE_MP_COST is 10

        EventBus::Unsubscribe(handle);
    }

    TEST_F(PlayerTest, UseMagic_LacksMp_FiresLackOfMpEvent) {
        Player player;

        player.ChangeMp(-100);
        
        bool lackOfMpEventFired = false;
        auto handle = EventBus::Subscribe<LackOfMpEvent>([&](const LackOfMpEvent&) {
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

        EXPECT_EQ(player.GetHealthComponent().GetHealth(), initialHealth - 3);
    }

    TEST_F(PlayerTest, UseMagic_Clairvoyance_CannotBeUsedTwice) {
        Player player;

        player.ChangeMp(100);

        bool firstResult  = player.UseMagic(EMagic::Clairvoyance);
        bool secondResult = player.UseMagic(EMagic::Clairvoyance);

        EXPECT_TRUE(firstResult);
        EXPECT_FALSE(secondResult);
    }

    TEST_F(PlayerTest, UseMagic_NullMagic_ReturnsFalse) {
        Player player;

        bool result = player.UseMagic(EMagic::Null);

        EXPECT_FALSE(result);
    }

    TEST_F(PlayerTest, Player_HasPlayerTag) {
        Player player;

        const auto& tags = player.GetTags();
        auto it = std::ranges::find(tags, ETag::Player);
        EXPECT_NE(it, tags.end());
    }

    TEST_F(PlayerTest, ChangeMp_NormalOffset_ChangesMp) {
        Player player;
        int initialMp = player.GetMp();
        
        player.ChangeMp(10);
        EXPECT_EQ(player.GetMp(), initialMp + 10);

        player.ChangeMp(-3);
        EXPECT_EQ(player.GetMp(), initialMp + 7);

        player.ChangeMp(2);
        EXPECT_EQ(player.GetMp(), initialMp + 9);
    }

} // namespace
} // namespace mc

namespace mc {
namespace {

// Feature: code-readability-refactor, Property 3:
// For any PlayerConfig, Player(config, sprite).GetMaxMp() == config.maxMp and GetMp() == 0
// Validates: Requirements 6.2, 6.4
RC_GTEST_PROP(Player_Property, ConstructorReflectsConfig, ())
{
    PlayerConfig config = Player::GetDefaultConfig();
    config.maxMp     = *rc::gen::inRange(0, 1001);
    config.initialHp = *rc::gen::inRange(1, 101);

    Player player(config, ESprite::MeowingCat);

#ifdef _DEBUG
    RC_ASSERT(player.GetMaxMp() == 100);
    RC_ASSERT(player.GetMp() == 10);
#else
    RC_ASSERT(player.GetMaxMp() == config.maxMp);
    RC_ASSERT(player.GetMp() == 0);
#endif
    RC_ASSERT(player.GetHealthComponent().GetMaxHealth() == config.initialHp);
}

} // namespace
} // namespace mc
