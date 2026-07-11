// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before rapidcheck includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <fstream>
#include <filesystem>

import ConfigService;
import PersistenceService;

namespace mc {
namespace {

    class ConfigServiceTest : public ::testing::Test {
    protected:
        std::string cardConfigPath = "test_card_config.json";
        std::string enemyConfigPath = "test_enemy_config.json";
        std::string gameConfigPath = "test_game_config.json";

        void SetUp() override {
            std::ofstream cardOut(cardConfigPath);
            cardOut << R"([
                {"type": 0, "value": 2},
                {"type": 1, "value": 3}
            ])";
            cardOut.close();

            std::ofstream enemyOut(enemyConfigPath);
            enemyOut << R"([{
                "name": "Test Enemy",
                "sprite": "cat.png",
                "baseWeight": 10,
                "rockDamage": 1,
                "scissorsDamage": 2,
                "paperDamage": 3,
                "hp": 42
            }])";
            enemyOut.close();

            std::ofstream gameOut(gameConfigPath);
            gameOut << R"({
                "stage": {
                    "battleCount": 5
                },
                "player": {
                    "initialHp": 20,
                    "maxMp": 12,
                    "sprite": "HeroCat",
                    "damage": {
                        "rock": 3,
                        "scissors": 3,
                        "paper": 3
                    },
                    "magic": {
                        "clairvoyance": {
                            "mpCost": 8
                        },
                        "powerBoost": {
                            "mpCost": 6,
                            "damageOffset": 4
                        },
                        "heal": {
                            "mpCost": 4,
                            "healAmount": 5,
                            "maxUses": 2
                        }
                    }
                }
            })";
            gameOut.close();
        }

        void TearDown() override {
            std::filesystem::remove(cardConfigPath);
            std::filesystem::remove(enemyConfigPath);
            std::filesystem::remove(gameConfigPath);
        }
    };

    TEST_F(ConfigServiceTest, LoadGameConfig_ParsesJsonFiles) {
        auto configService = CreateConfigService(cardConfigPath, enemyConfigPath, gameConfigPath);
        IConfigService& cfg = *configService;

        auto& cardConfigs = cfg.GetCardConfigs();
        EXPECT_EQ(cardConfigs.size(), 2);
        EXPECT_EQ(cardConfigs[0].type, 0);
        EXPECT_EQ(cardConfigs[0].value, 2);
        EXPECT_EQ(cardConfigs[1].type, 1);
        EXPECT_EQ(cardConfigs[1].value, 3);

        auto& enemyConfigs = cfg.GetEnemyConfigs();
        EXPECT_EQ(enemyConfigs.size(), 1);
        EXPECT_EQ(enemyConfigs[0].hp, 42);
        EXPECT_EQ(enemyConfigs[0].baseWeight, 10);
        EXPECT_EQ(enemyConfigs[0].rockDamage, 1);
        EXPECT_EQ(enemyConfigs[0].scissorsDamage, 2);
        EXPECT_EQ(enemyConfigs[0].paperDamage, 3);
        EXPECT_EQ(enemyConfigs[0].spriteName, "cat.png");
        EXPECT_EQ(enemyConfigs[0].name, L"Test Enemy");

        auto& playerConfig = cfg.GetPlayerConfig();
        EXPECT_EQ(playerConfig.initialHp, 20);
        EXPECT_EQ(playerConfig.maxMp, 12);
        EXPECT_EQ(playerConfig.spriteName, "HeroCat");
        EXPECT_EQ(playerConfig.rockDamage, 3);
        EXPECT_EQ(playerConfig.scissorsDamage, 3);
        EXPECT_EQ(playerConfig.paperDamage, 3);
        EXPECT_EQ(playerConfig.clairvoyanceMpCost, 8);
        EXPECT_EQ(playerConfig.powerBoostMpCost, 6);
        EXPECT_EQ(playerConfig.powerBoostDamageOffset, 4);
        EXPECT_EQ(playerConfig.healMpCost, 4);
        EXPECT_EQ(playerConfig.healAmount, 5);
        EXPECT_EQ(playerConfig.maxHealUses, 2);

        auto& gameConfig = cfg.GetGameConfig();
        EXPECT_EQ(gameConfig.battleCount, 5);
    }

} // namespace
} // namespace mc

// =============================================================================
// Property-based test: SaveGame / LoadGame round-trip
//
// Feature: code-readability-refactor, Property 2:
// For any valid GameState, SaveGame(slot, state) → LoadGame(slot) returns
// an equivalent state — all scalar fields and vector contents are preserved.
//
// Validates: Requirements 3.5
// =============================================================================

namespace mc {
namespace {

/// Minimal config JSON files for the property test (written per-test-run via SetUp).
static const std::string kPropCardConfig  = "prop_cs_card_config.json";
static const std::string kPropEnemyConfig = "prop_cs_enemy_config.json";
static const std::string kPropGameConfig  = "prop_cs_game_config.json";

/// Write the minimal config files expected by ConfigService constructor.
static void WritePropConfigFiles()
{
    {
        std::ofstream f(kPropCardConfig);
        f << R"([{"type":0,"value":2}])";
    }
    {
        std::ofstream f(kPropEnemyConfig);
        f << R"([{"name":"Prop Enemy","sprite":"Bunny","baseWeight":5,
                  "rockDamage":1,"scissorsDamage":2,"paperDamage":3,"hp":20}])";
    }
    {
        std::ofstream f(kPropGameConfig);
        f << R"({"stage":{"battleCount":3},"player":{"initialHp":15,"maxMp":10,
                "sprite":"HeroCat","damage":{"rock":3,"scissors":3,"paper":3},
                "magic":{"clairvoyance":{"mpCost":8},"powerBoost":{"mpCost":6,
                "damageOffset":4},"heal":{"mpCost":4,"healAmount":5,"maxUses":2}}}})";
    }
}

static void RemovePropConfigFiles()
{
    std::filesystem::remove(kPropCardConfig);
    std::filesystem::remove(kPropEnemyConfig);
    std::filesystem::remove(kPropGameConfig);
}

/// RC_GTEST_PROP: Property 2 — GameState のセーブ/ロードラウンドトリップが値を保持する
///
/// For any arbitrarily generated GameState, saving to a slot and immediately
/// loading it back must yield a state with identical field values.
///
/// Validates: Requirements 3.5
RC_GTEST_PROP(ConfigService_Property, SaveGame_LoadGame_RoundTrip, ())
{
    // ── Setup: create resource dir and minimal config files ───────────────
    std::filesystem::create_directories("resource");
    WritePropConfigFiles();

    auto configService = CreateConfigService(kPropCardConfig, kPropEnemyConfig, kPropGameConfig);
    auto persistenceService = CreatePersistenceService();
    IPersistenceService& persistence = *persistenceService;

    // ── Generate: arbitrary GameState ─────────────────────────────────────
    // Slot: use [1, SAVE_SLOT_COUNT-1] to avoid interfering with auto-save (slot 0)
    const int slot = *rc::gen::inRange(1, SAVE_SLOT_COUNT);

    GameState state;

    // Scalar player fields
    state.playerMaxHp               = *rc::gen::inRange(1, 100);
    state.playerHp                  = *rc::gen::inRange(0, state.playerMaxHp);
    state.playerMp                  = *rc::gen::inRange(0, 50);
    state.playerSprite              = *rc::gen::inRange(-1, 200);
    state.playerHealUses            = *rc::gen::inRange(0, 5);
    state.playerHasUsedClairvoyance = *rc::gen::arbitrary<bool>();

    // Scalar enemy fields
    state.enemyMaxHp          = *rc::gen::inRange(1, 100);
    state.enemyHp             = *rc::gen::inRange(0, state.enemyMaxHp);
    state.enemySprite         = *rc::gen::inRange(-1, 200);
    state.enemyRockOffset     = *rc::gen::inRange(-10, 10);
    state.enemyScissorsOffset = *rc::gen::inRange(-10, 10);
    state.enemyPaperOffset    = *rc::gen::inRange(-10, 10);

    // Battle progress — keep sequence short (0–5 elements)
    state.currentIndex = *rc::gen::inRange(0, 5);
    const int seqLen = *rc::gen::inRange(0, 5);
    state.sequence.resize(seqLen);
    for (int& v : state.sequence)
        v = *rc::gen::inRange(0, 200);

    // Card piles — keep short (0–5 cards each)
    auto genCardData = [&]() -> CardData {
        return CardData{
            *rc::gen::inRange(0, 5),
            *rc::gen::inRange(0, 10)
        };
    };

    const int handSize    = *rc::gen::inRange(0, 5);
    const int drawSize    = *rc::gen::inRange(0, 5);
    const int discardSize = *rc::gen::inRange(0, 5);
    state.hand.resize(handSize);
    state.drawPile.resize(drawSize);
    state.discardPile.resize(discardSize);
    for (auto& cd : state.hand)        cd = genCardData();
    for (auto& cd : state.drawPile)    cd = genCardData();
    for (auto& cd : state.discardPile) cd = genCardData();

    // ── Act: save then load ───────────────────────────────────────────────
    RC_ASSERT(persistence.SaveGame(slot, state));

    auto loadedOpt = persistence.LoadGame(slot);
    RC_ASSERT(loadedOpt.has_value());

    const GameState& g = *loadedOpt;

    // ── Assert: all scalar fields match ───────────────────────────────────
    RC_ASSERT(g.playerHp                  == state.playerHp);
    RC_ASSERT(g.playerMaxHp               == state.playerMaxHp);
    RC_ASSERT(g.playerMp                  == state.playerMp);
    RC_ASSERT(g.playerSprite              == state.playerSprite);
    RC_ASSERT(g.playerHealUses            == state.playerHealUses);
    RC_ASSERT(g.playerHasUsedClairvoyance == state.playerHasUsedClairvoyance);

    RC_ASSERT(g.enemyHp             == state.enemyHp);
    RC_ASSERT(g.enemyMaxHp          == state.enemyMaxHp);
    RC_ASSERT(g.enemySprite         == state.enemySprite);
    RC_ASSERT(g.enemyRockOffset     == state.enemyRockOffset);
    RC_ASSERT(g.enemyScissorsOffset == state.enemyScissorsOffset);
    RC_ASSERT(g.enemyPaperOffset    == state.enemyPaperOffset);

    RC_ASSERT(g.currentIndex == state.currentIndex);

    // Assert: sequence vector contents match
    RC_ASSERT(g.sequence.size() == state.sequence.size());
    for (std::size_t i = 0; i < state.sequence.size(); ++i)
        RC_ASSERT(g.sequence[i] == state.sequence[i]);

    // Assert: card pile contents match
    RC_ASSERT(g.hand.size() == state.hand.size());
    for (std::size_t i = 0; i < state.hand.size(); ++i) {
        RC_ASSERT(g.hand[i].type  == state.hand[i].type);
        RC_ASSERT(g.hand[i].power == state.hand[i].power);
    }

    RC_ASSERT(g.drawPile.size() == state.drawPile.size());
    for (std::size_t i = 0; i < state.drawPile.size(); ++i) {
        RC_ASSERT(g.drawPile[i].type  == state.drawPile[i].type);
        RC_ASSERT(g.drawPile[i].power == state.drawPile[i].power);
    }

    RC_ASSERT(g.discardPile.size() == state.discardPile.size());
    for (std::size_t i = 0; i < state.discardPile.size(); ++i) {
        RC_ASSERT(g.discardPile[i].type  == state.discardPile[i].type);
        RC_ASSERT(g.discardPile[i].power == state.discardPile[i].power);
    }

    // ── Teardown: remove slot save file and config files ──────────────────
    std::filesystem::remove("resource/save" + std::to_string(slot) + ".json");
    RemovePropConfigFiles();
}

} // namespace
} // namespace mc
