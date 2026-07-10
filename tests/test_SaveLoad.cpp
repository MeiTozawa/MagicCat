// =============================================================================
// test_SaveLoad.cpp
// Unit tests for the ConfigService save/load system.
//
// Task 13.1 — ConfigService SaveGame/LoadGame round-trip and GetSaveMetadata
//   Tests: ConfigService_SaveGame_LoadGame_RoundTrip
//          ConfigService_GetSaveMetadata_ValidSlot
//          ConfigService_GetSaveMetadata_AbsentSlot
//          ConfigService_LoadSoundSettings_SaveSoundSettings_RoundTrip
//          ConfigService_LoadSoundSettings_AbsentFile
//
// Later tasks will append to this file:
//   SECTION_13_2 — Player state accessor round-trip via BattleService
//   SECTION_13_3 — Enemy state accessor round-trip via BattleService
//   SECTION_13_4 — CardService accessor round-trip via BattleService
//   SECTION_13_5 — BattleService SaveState/LoadState via MockConfigService
//   SECTION_13_6 — AudioService construction and volume behaviour via MockConfigService
// =============================================================================

#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

import ConfigService;
import BattleService;
import CardService;
import Enemy;
import Player;
import HealthComponent;

#include "MockServices.h"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

namespace mc {
namespace {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Paths to minimal game-config JSON files required to construct ConfigService.
/// We re-use the same minimal configs for every test in this file.
static const std::string kCardConfigPath = "test_saveload_card_config.json";
static const std::string kEnemyConfigPath = "test_saveload_enemy_config.json";
static const std::string kGameConfigPath  = "test_saveload_game_config.json";

/// Save slot used by these tests. Choose 3 (last manual slot) to avoid
/// interfering with real auto-save (slot 0) and mid-game saves (1, 2).
static constexpr int kTestSlot = 3;

/// Path that ConfigService writes when using kTestSlot.
static const std::string kTestSaveFile = "resource/save3.json";

/// Path that ConfigService writes for sound settings.
static const std::string kSoundSettingsFile = "resource/sound_settings.json";

/// Write the minimal JSON config files used to construct a ConfigService.
static void WriteMinimalConfigFiles()
{
    {
        std::ofstream f(kCardConfigPath);
        f << R"([{"type":0,"value":2},{"type":1,"value":3}])";
    }
    {
        std::ofstream f(kEnemyConfigPath);
        f << R"([{
            "name":"TestEnemy","sprite":"Bunny",
            "baseWeight":10,"rockDamage":1,"scissorsDamage":2,"paperDamage":3,"hp":20
        }])";
    }
    {
        std::ofstream f(kGameConfigPath);
        f << R"({
            "stage":{"battleCount":3},
            "player":{
                "initialHp":15,"maxMp":10,"sprite":"HeroCat",
                "damage":{"rock":3,"scissors":3,"paper":3},
                "magic":{
                    "clairvoyance":{"mpCost":8},
                    "powerBoost":{"mpCost":6,"damageOffset":4},
                    "heal":{"mpCost":4,"healAmount":5,"maxUses":2}
                }
            }
        })";
    }
}

/// Remove all temporary files created by tests in this file.
static void RemoveMinimalConfigFiles()
{
    std::filesystem::remove(kCardConfigPath);
    std::filesystem::remove(kEnemyConfigPath);
    std::filesystem::remove(kGameConfigPath);
}

/// Build a fully-populated GameState for round-trip tests.
static GameState MakeTestGameState()
{
    GameState s;
    s.playerHp                   = 10;
    s.playerMaxHp                = 15;
    s.playerMp                   = 3;
    s.playerSprite               = 107;
    s.playerHealUses             = 1;
    s.playerHasUsedClairvoyance  = true;

    s.enemyHp             = 5;
    s.enemyMaxHp          = 20;
    s.enemySprite         = 101;
    s.enemyRockOffset     = 2;
    s.enemyScissorsOffset = -1;
    s.enemyPaperOffset    = 0;

    s.currentIndex = 1;
    s.sequence     = { 101, 102, 103 };

    s.hand        = { CardData{0, 2} };
    s.drawPile    = { CardData{1, 1}, CardData{3, 3} };
    s.discardPile = {};

    return s;
}

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------

class ConfigServiceSaveLoadTest : public ::testing::Test
{
protected:
    std::unique_ptr<IConfigService> configService;

    void SetUp() override
    {
        // Ensure the resource/ directory exists (tests run from repo root where
        // it may not exist; the game's resource/ lives under MagicCat/resource/).
        std::filesystem::create_directories("resource");

        WriteMinimalConfigFiles();
        configService = CreateConfigService(kCardConfigPath, kEnemyConfigPath, kGameConfigPath);
        // Remove any leftover save file from a previous run.
        std::filesystem::remove(kTestSaveFile);
        std::filesystem::remove(kSoundSettingsFile);
    }

    void TearDown() override
    {
        std::filesystem::remove(kTestSaveFile);
        std::filesystem::remove(kSoundSettingsFile);
        RemoveMinimalConfigFiles();
    }
};

// =============================================================================
// SECTION_13_1: ConfigService SaveGame / LoadGame round-trip and GetSaveMetadata
// =============================================================================

/// Requirement 1.4, 1.6: SaveGame writes and LoadGame reads back identical GameState.
TEST_F(ConfigServiceSaveLoadTest, ConfigService_SaveGame_LoadGame_RoundTrip)
{
    const GameState original = MakeTestGameState();

    ASSERT_TRUE(configService->SaveGame(kTestSlot, original));

    auto loaded = configService->LoadGame(kTestSlot);
    ASSERT_TRUE(loaded.has_value()) << "LoadGame returned nullopt for a slot that was just saved";

    const GameState& g = *loaded;

    // Player fields
    EXPECT_EQ(g.playerHp,                  original.playerHp);
    EXPECT_EQ(g.playerMaxHp,               original.playerMaxHp);
    EXPECT_EQ(g.playerMp,                  original.playerMp);
    EXPECT_EQ(g.playerSprite,              original.playerSprite);
    EXPECT_EQ(g.playerHealUses,            original.playerHealUses);
    EXPECT_EQ(g.playerHasUsedClairvoyance, original.playerHasUsedClairvoyance);

    // Enemy fields
    EXPECT_EQ(g.enemyHp,             original.enemyHp);
    EXPECT_EQ(g.enemyMaxHp,          original.enemyMaxHp);
    EXPECT_EQ(g.enemySprite,         original.enemySprite);
    EXPECT_EQ(g.enemyRockOffset,     original.enemyRockOffset);
    EXPECT_EQ(g.enemyScissorsOffset, original.enemyScissorsOffset);
    EXPECT_EQ(g.enemyPaperOffset,    original.enemyPaperOffset);

    // Battle progress
    EXPECT_EQ(g.currentIndex, original.currentIndex);
    ASSERT_EQ(g.sequence.size(), original.sequence.size());
    for (std::size_t i = 0; i < original.sequence.size(); ++i)
        EXPECT_EQ(g.sequence[i], original.sequence[i]) << "sequence mismatch at index " << i;

    // Card piles
    ASSERT_EQ(g.hand.size(), original.hand.size());
    for (std::size_t i = 0; i < original.hand.size(); ++i) {
        EXPECT_EQ(g.hand[i].type,  original.hand[i].type);
        EXPECT_EQ(g.hand[i].power, original.hand[i].power);
    }

    ASSERT_EQ(g.drawPile.size(), original.drawPile.size());
    for (std::size_t i = 0; i < original.drawPile.size(); ++i) {
        EXPECT_EQ(g.drawPile[i].type,  original.drawPile[i].type);
        EXPECT_EQ(g.drawPile[i].power, original.drawPile[i].power);
    }

    EXPECT_EQ(g.discardPile.size(), original.discardPile.size());
}

/// Requirement 1.9, 1.10: GetSaveMetadata extracts correct fields from a valid save.
TEST_F(ConfigServiceSaveLoadTest, ConfigService_GetSaveMetadata_ValidSlot)
{
    const GameState original = MakeTestGameState();
    ASSERT_TRUE(configService->SaveGame(kTestSlot, original));

    const SaveMetadata meta = configService->GetSaveMetadata(kTestSlot);

    EXPECT_TRUE(meta.exists);

    // currentBattle = currentIndex + 1 (1-based display index) — Requirement 1.10
    EXPECT_EQ(meta.currentBattle, original.currentIndex + 1);
    EXPECT_EQ(meta.totalBattles,  static_cast<int>(original.sequence.size()));

    EXPECT_EQ(meta.playerHp,     original.playerHp);
    EXPECT_EQ(meta.playerMaxHp,  original.playerMaxHp);
    EXPECT_EQ(meta.playerSprite, original.playerSprite);

    EXPECT_EQ(meta.enemyHp,     original.enemyHp);
    EXPECT_EQ(meta.enemyMaxHp,  original.enemyMaxHp);
    EXPECT_EQ(meta.enemySprite, original.enemySprite);
}

/// Requirement 1.8, 14.5: GetSaveMetadata returns exists=false for an absent slot.
TEST_F(ConfigServiceSaveLoadTest, ConfigService_GetSaveMetadata_AbsentSlot)
{
    // No save file written — ensure it really is absent.
    std::filesystem::remove(kTestSaveFile);

    const SaveMetadata meta = configService->GetSaveMetadata(kTestSlot);

    EXPECT_FALSE(meta.exists);
}

/// Requirement 1.11, 1.12, 1.13, 1.14: SaveSoundSettings then LoadSoundSettings
/// restores exact values (out-param round-trip).
TEST_F(ConfigServiceSaveLoadTest, ConfigService_LoadSoundSettings_SaveSoundSettings_RoundTrip)
{
    const int masterIn = 3;
    const int bgmIn    = 1;
    const int sfxIn    = 4;

    ASSERT_TRUE(configService->SaveSoundSettings(masterIn, bgmIn, sfxIn));

    int masterOut = 99, bgmOut = 99, sfxOut = 99;
    const bool loaded = configService->LoadSoundSettings(masterOut, bgmOut, sfxOut);

    EXPECT_TRUE(loaded);
    EXPECT_EQ(masterOut, masterIn);
    EXPECT_EQ(bgmOut,    bgmIn);
    EXPECT_EQ(sfxOut,    sfxIn);
}

/// Requirement 1.13: LoadSoundSettings returns false for absent file and
/// does NOT modify the out-params.
TEST_F(ConfigServiceSaveLoadTest, ConfigService_LoadSoundSettings_AbsentFile)
{
    // Ensure no sound settings file exists.
    std::filesystem::remove(kSoundSettingsFile);

    const int sentinel = 42;
    int masterOut = sentinel, bgmOut = sentinel, sfxOut = sentinel;

    const bool result = configService->LoadSoundSettings(masterOut, bgmOut, sfxOut);

    EXPECT_FALSE(result);
    // Out-params must be unmodified on failure.
    EXPECT_EQ(masterOut, sentinel);
    EXPECT_EQ(bgmOut,    sentinel);
    EXPECT_EQ(sfxOut,    sentinel);
}

// =============================================================================
// END SECTION_13_1
// =============================================================================

// =============================================================================
// SECTION_13_2: Player state accessor round-trip via BattleService
// =============================================================================

// ---------------------------------------------------------------------------
// Stub config service shared by SECTION_13_2 and later sections
// (Defined once; later sections may extend or reuse it)
// ---------------------------------------------------------------------------

class StubConfigForSaveLoad : public IConfigService
{
    std::vector<EnemyConfig> enemies_;
    std::vector<CardConfig>  cards_;
    PlayerConfig playerConfig_ = Player::GetDefaultConfig();
    GameConfig   gameConfig_{ 3 };
public:
    explicit StubConfigForSaveLoad(
        std::vector<EnemyConfig> enemies = {
            EnemyConfig{ 20, 5, 1, 2, 3, L"TestBunny", "Bunny" },
            EnemyConfig{ 18, 4, 2, 1, 2, L"TestWolf",  "Wolf"  },
            EnemyConfig{ 22, 6, 3, 1, 1, L"TestBunny2","Bunny" },
        },
        std::vector<CardConfig> cards = {
            { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 },
        })
        : enemies_(std::move(enemies))
        , cards_(std::move(cards))
    {}

    const std::vector<CardConfig>&  GetCardConfigs()  const override { return cards_;   }
    const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemies_; }
    const PlayerConfig& GetPlayerConfig() const override { return playerConfig_; }
    const GameConfig&   GetGameConfig()   const override { return gameConfig_;   }
    bool LoadSoundSettings(int&, int&, int&) override { return false; }
    bool SaveSoundSettings(int, int, int)    override { return true;  }

    // For SECTION_13_2 round-trip test we forward save/load to a real ConfigService
    // stored alongside this stub.  We initialise it lazily once a slot is used.
    bool SaveGame(int slot, const GameState& state) override
    {
        return realConfig_->SaveGame(slot, state);
    }
    std::optional<GameState> LoadGame(int slot) override
    {
        return realConfig_->LoadGame(slot);
    }
    SaveMetadata GetSaveMetadata(int slot) override
    {
        return realConfig_->GetSaveMetadata(slot);
    }

    void SetRealConfig(IConfigService* real) { realConfig_ = real; }

private:
    IConfigService* realConfig_ = nullptr;
};

// ---------------------------------------------------------------------------
// Fixture for SECTION_13_2  (uses real disk I/O through ConfigService)
// ---------------------------------------------------------------------------

class PlayerRoundTripTest : public ::testing::Test
{
protected:
    static constexpr int kSlot = 2;
    static const std::string kSaveFile;  // resource/save2.json

    std::unique_ptr<IConfigService>  realConfig;
    StubConfigForSaveLoad            stubConfig;
    NiceMock<MockAssetService>       mockAsset;
    std::unique_ptr<ICardService>    cardService;
    std::unique_ptr<IBattleService>  battleService;

    void SetUp() override
    {
        std::filesystem::create_directories("resource");

        // Write minimal config files (reuse the same helper as SECTION_13_1)
        {
            std::ofstream f("test_s132_card.json");
            f << R"([{"type":0,"value":2},{"type":1,"value":3}])";
        }
        {
            std::ofstream f("test_s132_enemy.json");
            f << R"([
                {"name":"TestBunny","sprite":"Bunny",
                 "baseWeight":5,"rockDamage":1,"scissorsDamage":2,"paperDamage":3,"hp":20},
                {"name":"TestWolf","sprite":"Wolf",
                 "baseWeight":4,"rockDamage":2,"scissorsDamage":1,"paperDamage":2,"hp":18}
            ])";
        }
        {
            std::ofstream f("test_s132_game.json");
            f << R"({
                "stage":{"battleCount":3},
                "player":{
                    "initialHp":15,"maxMp":10,"sprite":"MeowingCat",
                    "damage":{"rock":3,"scissors":3,"paper":3},
                    "magic":{
                        "clairvoyance":{"mpCost":8},
                        "powerBoost":{"mpCost":6,"damageOffset":4},
                        "heal":{"mpCost":4,"healAmount":5,"maxUses":2}
                    }
                }
            })";
        }

        realConfig = CreateConfigService(
            "test_s132_card.json", "test_s132_enemy.json", "test_s132_game.json");
        stubConfig.SetRealConfig(realConfig.get());

        // MockAssetService: return deterministic ESprite values by name
        ON_CALL(mockAsset, ParseSprite(_))
            .WillByDefault(Return(ESprite::Null));
        ON_CALL(mockAsset, ParseSprite("Bunny"))
            .WillByDefault(Return(ESprite::Bunny));
        ON_CALL(mockAsset, ParseSprite("Wolf"))
            .WillByDefault(Return(ESprite::Wolf));
        ON_CALL(mockAsset, ParseSprite("MeowingCat"))
            .WillByDefault(Return(ESprite::MeowingCat));

        cardService   = CreateCardService(stubConfig);
        battleService = CreateBattleService(stubConfig, *cardService, mockAsset);

        // Remove any leftover save file from a prior test run
        std::filesystem::remove(kSaveFile);
    }

    void TearDown() override
    {
        battleService.reset();
        cardService.reset();
        std::filesystem::remove(kSaveFile);
        std::filesystem::remove("test_s132_card.json");
        std::filesystem::remove("test_s132_enemy.json");
        std::filesystem::remove("test_s132_game.json");
    }
};

const std::string PlayerRoundTripTest::kSaveFile = "resource/save2.json";

/// Requirement 3.3, 3.4: All Player fields survive a SaveState/LoadState round-trip.
/// Fields verified: hp, maxHp, mp, sprite, healUses, hasUsedClairvoyance.
TEST_F(PlayerRoundTripTest, Player_AllFieldsSurviveSaveLoadRoundTrip)
{
    battleService->StartStage();

    // ── Arrange: put player into a known state ────────────────────────────
    Player& player = battleService->GetPlayer();

    // Damage player so hp != maxHp
    const int maxHp = player.GetHealthComponent().GetMaxHealth();
    player.GetHealthComponent().SetHealth(maxHp - 3);

    // Set MP (clamped accessor — we set via SetMp)
    player.SetMp(7);

    // Mark clairvoyance used and two heal uses consumed
    player.SetHasUsedClairvoyance(true);
    player.SetHealUses(1);

    // Capture expected values
    const int  expectedHp                  = player.GetHealthComponent().GetHealth();
    const int  expectedMaxHp               = player.GetHealthComponent().GetMaxHealth();
    const int  expectedMp                  = player.GetMp();
    const int  expectedSprite              = static_cast<int>(player.GetSprite());
    const int  expectedHealUses            = player.GetHealUses();
    const bool expectedHasUsedClairvoyance = player.GetHasUsedClairvoyance();

    // ── Act: save then load ───────────────────────────────────────────────
    battleService->SaveState(kSlot);
    ASSERT_TRUE(battleService->LoadState(kSlot)) << "LoadState must succeed";

    // ── Assert: player fields restored exactly ────────────────────────────
    const Player& loaded = battleService->GetPlayer();

    EXPECT_EQ(loaded.GetHealthComponent().GetHealth(),    expectedHp)
        << "hp must survive round-trip";
    EXPECT_EQ(loaded.GetHealthComponent().GetMaxHealth(), expectedMaxHp)
        << "maxHp must survive round-trip";
    EXPECT_EQ(loaded.GetMp(),                             expectedMp)
        << "mp must survive round-trip";
    EXPECT_EQ(static_cast<int>(loaded.GetSprite()),       expectedSprite)
        << "sprite must survive round-trip";
    EXPECT_EQ(loaded.GetHealUses(),                       expectedHealUses)
        << "healUses must survive round-trip";
    EXPECT_EQ(loaded.GetHasUsedClairvoyance(),            expectedHasUsedClairvoyance)
        << "hasUsedClairvoyance must survive round-trip";
}

/// Requirement 3.5: When the saved hp exceeds maxHp (corrupted save), LoadState
/// must clamp hp down to maxHp.
TEST(PlayerHpClampTest, Player_HpClamped_WhenLoadedHpExceedsMaxHp)
{
    // ── Arrange: build a GameState where playerHp > playerMaxHp ──────────
    const int kMaxHp       = 15;
    const int kCorruptedHp = kMaxHp + 5;  // intentionally out-of-range

    GameState corruptedState;
    corruptedState.playerHp                  = kCorruptedHp;
    corruptedState.playerMaxHp               = kMaxHp;
    corruptedState.playerMp                  = 3;
    corruptedState.playerSprite              = static_cast<int>(ESprite::MeowingCat);
    corruptedState.playerHealUses            = 0;
    corruptedState.playerHasUsedClairvoyance = false;

    corruptedState.enemyHp             = 10;
    corruptedState.enemyMaxHp          = 20;
    corruptedState.enemySprite         = static_cast<int>(ESprite::Bunny);
    corruptedState.enemyRockOffset     = 0;
    corruptedState.enemyScissorsOffset = 0;
    corruptedState.enemyPaperOffset    = 0;

    corruptedState.currentIndex = 0;
    corruptedState.sequence     = { static_cast<int>(ESprite::Bunny) };

    corruptedState.hand        = {};
    corruptedState.drawPile    = { CardData{0, 2}, CardData{1, 3} };
    corruptedState.discardPile = {};

    // ── Arrange: inline stub config service that returns the corrupted state ─
    struct ClampTestConfig : public IConfigService
    {
        std::vector<EnemyConfig> enemies = {
            EnemyConfig{ 20, 5, 1, 2, 3, L"Bunny", "Bunny" },
        };
        std::vector<CardConfig>  cards = { { 0, 2 }, { 1, 2 } };
        PlayerConfig             playerCfg = Player::GetDefaultConfig();
        GameConfig               gameCfg{ 1 };
        std::optional<GameState> savedState;

        const std::vector<CardConfig>&  GetCardConfigs()  const override { return cards;     }
        const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemies;   }
        const PlayerConfig& GetPlayerConfig() const override { return playerCfg; }
        const GameConfig&   GetGameConfig()   const override { return gameCfg;   }
        bool LoadSoundSettings(int&, int&, int&) override { return false; }
        bool SaveSoundSettings(int, int, int)    override { return true;  }
        bool SaveGame(int, const GameState& s)   override { savedState = s; return true; }
        std::optional<GameState> LoadGame(int)   override { return savedState; }
        SaveMetadata GetSaveMetadata(int)        override { return SaveMetadata{}; }
    };

    ClampTestConfig stubConfig;
    stubConfig.savedState = corruptedState;  // pre-populate slot 0

    NiceMock<MockAssetService> mockAsset;
    ON_CALL(mockAsset, ParseSprite(_))           .WillByDefault(Return(ESprite::Null));
    ON_CALL(mockAsset, ParseSprite("Bunny"))     .WillByDefault(Return(ESprite::Bunny));
    ON_CALL(mockAsset, ParseSprite("MeowingCat")).WillByDefault(Return(ESprite::MeowingCat));

    auto cardService   = CreateCardService(stubConfig);
    auto battleService = CreateBattleService(stubConfig, *cardService, mockAsset);

    // ── Act ───────────────────────────────────────────────────────────────
    const bool loaded = battleService->LoadState(0);

    // ── Assert ────────────────────────────────────────────────────────────
    ASSERT_TRUE(loaded) << "LoadState must succeed with a valid (if corrupt-hp) save";

    const Player& player = battleService->GetPlayer();
    EXPECT_EQ(player.GetHealthComponent().GetHealth(),    kMaxHp)
        << "hp must be clamped to maxHp when save contained hp > maxHp";
    EXPECT_EQ(player.GetHealthComponent().GetMaxHealth(), kMaxHp)
        << "maxHp must be restored exactly";
}

// =============================================================================
// END SECTION_13_2
// =============================================================================

// =============================================================================
// SECTION_13_3: Enemy state accessor round-trip via BattleService
// =============================================================================

// ---------------------------------------------------------------------------
// Fixture for SECTION_13_3 — reuses the same minimal config infrastructure
// from PlayerRoundTripTest (StubConfigForSaveLoad + real disk ConfigService).
// ---------------------------------------------------------------------------

class EnemyRoundTripTest : public ::testing::Test
{
protected:
    static constexpr int kSlot = 2;
    static const std::string kSaveFile;  // resource/save2.json

    std::unique_ptr<IConfigService>  realConfig;
    StubConfigForSaveLoad            stubConfig;
    NiceMock<MockAssetService>       mockAsset;
    std::unique_ptr<ICardService>    cardService;
    std::unique_ptr<IBattleService>  battleService;

    void SetUp() override
    {
        std::filesystem::create_directories("resource");

        {
            std::ofstream f("test_s133_card.json");
            f << R"([{"type":0,"value":2},{"type":1,"value":3}])";
        }
        {
            std::ofstream f("test_s133_enemy.json");
            f << R"([
                {"name":"TestBunny","sprite":"Bunny",
                 "baseWeight":5,"rockDamage":1,"scissorsDamage":2,"paperDamage":3,"hp":20},
                {"name":"TestWolf","sprite":"Wolf",
                 "baseWeight":4,"rockDamage":2,"scissorsDamage":1,"paperDamage":2,"hp":18},
                {"name":"TestBunny2","sprite":"Bunny",
                 "baseWeight":6,"rockDamage":3,"scissorsDamage":1,"paperDamage":1,"hp":22}
            ])";
        }
        {
            std::ofstream f("test_s133_game.json");
            f << R"({
                "stage":{"battleCount":3},
                "player":{
                    "initialHp":15,"maxMp":10,"sprite":"MeowingCat",
                    "damage":{"rock":3,"scissors":3,"paper":3},
                    "magic":{
                        "clairvoyance":{"mpCost":8},
                        "powerBoost":{"mpCost":6,"damageOffset":4},
                        "heal":{"mpCost":4,"healAmount":5,"maxUses":2}
                    }
                }
            })";
        }

        realConfig = CreateConfigService(
            "test_s133_card.json", "test_s133_enemy.json", "test_s133_game.json");
        stubConfig.SetRealConfig(realConfig.get());

        ON_CALL(mockAsset, ParseSprite(_))
            .WillByDefault(Return(ESprite::Null));
        ON_CALL(mockAsset, ParseSprite("Bunny"))
            .WillByDefault(Return(ESprite::Bunny));
        ON_CALL(mockAsset, ParseSprite("Wolf"))
            .WillByDefault(Return(ESprite::Wolf));
        ON_CALL(mockAsset, ParseSprite("MeowingCat"))
            .WillByDefault(Return(ESprite::MeowingCat));

        cardService   = CreateCardService(stubConfig);
        battleService = CreateBattleService(stubConfig, *cardService, mockAsset);

        std::filesystem::remove(kSaveFile);
    }

    void TearDown() override
    {
        battleService.reset();
        cardService.reset();
        std::filesystem::remove(kSaveFile);
        std::filesystem::remove("test_s133_card.json");
        std::filesystem::remove("test_s133_enemy.json");
        std::filesystem::remove("test_s133_game.json");
    }
};

const std::string EnemyRoundTripTest::kSaveFile = "resource/save2.json";

/// Requirement 4.3, 4.4: All Enemy fields survive a SaveState/LoadState round-trip.
/// Fields verified: rockOffset, scissorsOffset, paperOffset, hp, maxHp, sprite.
TEST_F(EnemyRoundTripTest, Enemy_AllFieldsSurviveSaveLoadRoundTrip)
{
    battleService->StartStage();

    // ── Arrange: put enemy into a known mutated state ─────────────────────
    Enemy& enemy = battleService->GetEnemy();

    // Set non-default weight offsets
    enemy.SetRockOffset(3);
    enemy.SetScissorsOffset(-2);
    enemy.SetPaperOffset(5);

    // Damage the enemy so hp != maxHp
    IDamageable& health = enemy.GetHealthComponent();
    const int maxHp = health.GetMaxHealth();
    health.SetHealth(maxHp - 7);

    // Capture expected values
    const int expectedRockOffset     = enemy.GetRockOffset();
    const int expectedScissorsOffset = enemy.GetScissorsOffset();
    const int expectedPaperOffset    = enemy.GetPaperOffset();
    const int expectedHp             = health.GetHealth();
    const int expectedMaxHp          = health.GetMaxHealth();
    const int expectedSprite         = static_cast<int>(enemy.GetSprite());

    // ── Act: save then load ───────────────────────────────────────────────
    battleService->SaveState(kSlot);
    ASSERT_TRUE(battleService->LoadState(kSlot)) << "LoadState must succeed";

    // ── Assert: all enemy fields restored exactly ─────────────────────────
    const Enemy& loaded = battleService->GetEnemy();

    EXPECT_EQ(loaded.GetRockOffset(),     expectedRockOffset)
        << "rockOffset must survive round-trip";
    EXPECT_EQ(loaded.GetScissorsOffset(), expectedScissorsOffset)
        << "scissorsOffset must survive round-trip";
    EXPECT_EQ(loaded.GetPaperOffset(),    expectedPaperOffset)
        << "paperOffset must survive round-trip";
    EXPECT_EQ(loaded.GetHealthComponent().GetHealth(),    expectedHp)
        << "hp must survive round-trip";
    EXPECT_EQ(loaded.GetHealthComponent().GetMaxHealth(), expectedMaxHp)
        << "maxHp must survive round-trip";
    EXPECT_EQ(static_cast<int>(loaded.GetSprite()),       expectedSprite)
        << "sprite must survive round-trip";
}

// =============================================================================
// END SECTION_13_3
// =============================================================================

// =============================================================================
// SECTION_13_4: CardService accessor round-trip via BattleService
// =============================================================================

// ---------------------------------------------------------------------------
// Fixture for SECTION_13_4 — mirrors EnemyRoundTripTest setup.
// The ICardService pointer is kept externally so we can query pile contents
// directly without needing a getter on IBattleService.
// ---------------------------------------------------------------------------

class CardServiceRoundTripTest : public ::testing::Test
{
protected:
    static constexpr int kSlot = 2;
    static const std::string kSaveFile;  // resource/save2.json

    std::unique_ptr<IConfigService>  realConfig;
    StubConfigForSaveLoad            stubConfig;
    NiceMock<MockAssetService>       mockAsset;
    std::unique_ptr<ICardService>    cardService;
    std::unique_ptr<IBattleService>  battleService;

    void SetUp() override
    {
        std::filesystem::create_directories("resource");

        {
            std::ofstream f("test_s134_card.json");
            f << R"([{"type":0,"value":2},{"type":1,"value":3},{"type":2,"value":1},{"type":3,"value":4}])";
        }
        {
            std::ofstream f("test_s134_enemy.json");
            f << R"([
                {"name":"TestBunny","sprite":"Bunny",
                 "baseWeight":5,"rockDamage":1,"scissorsDamage":2,"paperDamage":3,"hp":20},
                {"name":"TestWolf","sprite":"Wolf",
                 "baseWeight":4,"rockDamage":2,"scissorsDamage":1,"paperDamage":2,"hp":18},
                {"name":"TestBunny2","sprite":"Bunny",
                 "baseWeight":6,"rockDamage":3,"scissorsDamage":1,"paperDamage":1,"hp":22}
            ])";
        }
        {
            std::ofstream f("test_s134_game.json");
            f << R"({
                "stage":{"battleCount":3},
                "player":{
                    "initialHp":15,"maxMp":10,"sprite":"MeowingCat",
                    "damage":{"rock":3,"scissors":3,"paper":3},
                    "magic":{
                        "clairvoyance":{"mpCost":8},
                        "powerBoost":{"mpCost":6,"damageOffset":4},
                        "heal":{"mpCost":4,"healAmount":5,"maxUses":2}
                    }
                }
            })";
        }

        realConfig = CreateConfigService(
            "test_s134_card.json", "test_s134_enemy.json", "test_s134_game.json");
        stubConfig.SetRealConfig(realConfig.get());

        ON_CALL(mockAsset, ParseSprite(_))
            .WillByDefault(Return(ESprite::Null));
        ON_CALL(mockAsset, ParseSprite("Bunny"))
            .WillByDefault(Return(ESprite::Bunny));
        ON_CALL(mockAsset, ParseSprite("Wolf"))
            .WillByDefault(Return(ESprite::Wolf));
        ON_CALL(mockAsset, ParseSprite("MeowingCat"))
            .WillByDefault(Return(ESprite::MeowingCat));

        cardService   = CreateCardService(stubConfig);
        battleService = CreateBattleService(stubConfig, *cardService, mockAsset);

        std::filesystem::remove(kSaveFile);
    }

    void TearDown() override
    {
        battleService.reset();
        cardService.reset();
        std::filesystem::remove(kSaveFile);
        std::filesystem::remove("test_s134_card.json");
        std::filesystem::remove("test_s134_enemy.json");
        std::filesystem::remove("test_s134_game.json");
    }
};

const std::string CardServiceRoundTripTest::kSaveFile = "resource/save2.json";

/// Requirements 5.3, 5.4: All three card piles are restored correctly after a
/// SaveState/LoadState round-trip.  We capture the CardData vectors for hand,
/// drawPile, and discardPile before saving and compare them element-by-element
/// after loading.
TEST_F(CardServiceRoundTripTest, CardService_AllPilesRestoredAfterSaveLoadRoundTrip)
{
    battleService->StartStage();

    // Draw some cards so the hand is non-empty and draw-pile has been reduced.
    cardService->DrawCard();
    cardService->DrawCard();

    // ── Capture pre-save state ────────────────────────────────────────────
    const std::vector<CardData> preSaveHand        = cardService->GetHand();
    const std::vector<CardData> preSaveDrawPile    = cardService->GetDrawPile();
    const std::vector<CardData> preSaveDiscardPile = cardService->GetDiscardPile();

    // ── Act: save then load ───────────────────────────────────────────────
    battleService->SaveState(kSlot);
    ASSERT_TRUE(battleService->LoadState(kSlot)) << "LoadState must succeed";

    // ── Assert: hand restored ─────────────────────────────────────────────
    const std::vector<CardData> loadedHand = cardService->GetHand();
    ASSERT_EQ(loadedHand.size(), preSaveHand.size())
        << "hand size must be preserved after round-trip";
    for (std::size_t i = 0; i < preSaveHand.size(); ++i)
    {
        EXPECT_EQ(loadedHand[i].type,  preSaveHand[i].type)
            << "hand[" << i << "].type must match after round-trip";
        EXPECT_EQ(loadedHand[i].power, preSaveHand[i].power)
            << "hand[" << i << "].power must match after round-trip";
    }

    // ── Assert: drawPile restored ─────────────────────────────────────────
    const std::vector<CardData> loadedDrawPile = cardService->GetDrawPile();
    ASSERT_EQ(loadedDrawPile.size(), preSaveDrawPile.size())
        << "drawPile size must be preserved after round-trip";
    for (std::size_t i = 0; i < preSaveDrawPile.size(); ++i)
    {
        EXPECT_EQ(loadedDrawPile[i].type,  preSaveDrawPile[i].type)
            << "drawPile[" << i << "].type must match after round-trip";
        EXPECT_EQ(loadedDrawPile[i].power, preSaveDrawPile[i].power)
            << "drawPile[" << i << "].power must match after round-trip";
    }

    // ── Assert: discardPile restored ──────────────────────────────────────
    const std::vector<CardData> loadedDiscardPile = cardService->GetDiscardPile();
    ASSERT_EQ(loadedDiscardPile.size(), preSaveDiscardPile.size())
        << "discardPile size must be preserved after round-trip";
    for (std::size_t i = 0; i < preSaveDiscardPile.size(); ++i)
    {
        EXPECT_EQ(loadedDiscardPile[i].type,  preSaveDiscardPile[i].type)
            << "discardPile[" << i << "].type must match after round-trip";
        EXPECT_EQ(loadedDiscardPile[i].power, preSaveDiscardPile[i].power)
            << "discardPile[" << i << "].power must match after round-trip";
    }
}

/// Requirement 5.5: Total card count (hand + drawPile + discardPile) is
/// preserved across a SaveState/LoadState round-trip regardless of how the
/// cards are distributed between the three piles.
TEST_F(CardServiceRoundTripTest, CardService_TotalCardCountPreservedAfterRoundTrip)
{
    battleService->StartStage();

    // Draw several cards to create a non-trivial distribution across piles.
    cardService->DrawCard();
    cardService->DrawCard();
    cardService->DrawCard();

    // ── Capture pre-save total ────────────────────────────────────────────
    const std::size_t preSaveTotal =
        cardService->GetHand().size() +
        cardService->GetDrawPile().size() +
        cardService->GetDiscardPile().size();

    // ── Act ───────────────────────────────────────────────────────────────
    battleService->SaveState(kSlot);
    ASSERT_TRUE(battleService->LoadState(kSlot)) << "LoadState must succeed";

    // ── Assert: total unchanged ───────────────────────────────────────────
    const std::size_t postLoadTotal =
        cardService->GetHand().size() +
        cardService->GetDrawPile().size() +
        cardService->GetDiscardPile().size();

    EXPECT_EQ(postLoadTotal, preSaveTotal)
        << "hand.size() + drawPile.size() + discardPile.size() must equal "
           "the pre-save total after a round-trip";
}

// =============================================================================
// END SECTION_13_4
// =============================================================================
// =============================================================================
// SECTION_13_5: BattleService SaveState/LoadState via MockConfigService
// =============================================================================
//
// All three tests use NiceMock<MockConfigService> so that only the calls we
// explicitly set up with EXPECT_CALL / ON_CALL matter.  Real disk I/O is
// completely eliminated — the mock intercepts SaveGame/LoadGame.
//
// Requirements: 6.3, 6.4, 6.5, 6.6, 6.7, 6.8
// =============================================================================

// ---------------------------------------------------------------------------
// Helper: build a PlayerConfig compatible with LoadState (needs a spriteName
// that ParseSprite can resolve so the player re-creation path works).
// ---------------------------------------------------------------------------
static PlayerConfig MakePlayerConfig()
{
    PlayerConfig cfg = Player::GetDefaultConfig();
    cfg.spriteName = "MeowingCat";
    return cfg;
}

// ---------------------------------------------------------------------------
// Fixture for SECTION_13_5
// ---------------------------------------------------------------------------

class BattleServiceMockConfigTest : public ::testing::Test
{
protected:
    // Constant configs returned by the mock — must outlive the services.
    std::vector<EnemyConfig>  enemies = {
        EnemyConfig{ 20, 5, 1, 2, 3, L"TestBunny", "Bunny" },
        EnemyConfig{ 18, 4, 2, 1, 2, L"TestWolf",  "Wolf"  },
    };
    std::vector<CardConfig>   cards = {
        { 0, 2 }, { 1, 3 },
    };
    PlayerConfig              playerCfg = MakePlayerConfig();
    GameConfig                gameCfg{ 2 };

    NiceMock<MockConfigService> mockConfig;
    NiceMock<MockAssetService>  mockAsset;
    std::unique_ptr<ICardService>   cardService;
    std::unique_ptr<IBattleService> battleService;

    void SetUp() override
    {
        // Wire static getters — called during construction / StartStage / LoadState.
        ON_CALL(mockConfig, GetEnemyConfigs())
            .WillByDefault(ReturnRef(enemies));
        ON_CALL(mockConfig, GetCardConfigs())
            .WillByDefault(ReturnRef(cards));
        ON_CALL(mockConfig, GetPlayerConfig())
            .WillByDefault(ReturnRef(playerCfg));
        ON_CALL(mockConfig, GetGameConfig())
            .WillByDefault(ReturnRef(gameCfg));

        // ParseSprite: Bunny → ESprite::Bunny, Wolf → ESprite::Wolf,
        // MeowingCat → ESprite::MeowingCat, anything else → Null.
        ON_CALL(mockAsset, ParseSprite(_))
            .WillByDefault(Return(ESprite::Null));
        ON_CALL(mockAsset, ParseSprite("Bunny"))
            .WillByDefault(Return(ESprite::Bunny));
        ON_CALL(mockAsset, ParseSprite("Wolf"))
            .WillByDefault(Return(ESprite::Wolf));
        ON_CALL(mockAsset, ParseSprite("MeowingCat"))
            .WillByDefault(Return(ESprite::MeowingCat));

        cardService   = CreateCardService(mockConfig);
        battleService = CreateBattleService(mockConfig, *cardService, mockAsset);
    }
};

/// Requirement 6.3, 6.4, 6.7:
/// Full state round-trip via MockConfigService — SaveState captures the
/// serialised GameState; LoadState restores currentIndex, player HP, enemy HP,
/// and all three card piles without any disk I/O.
TEST_F(BattleServiceMockConfigTest, BattleService_FullStateRoundTrip)
{
    // ── Arrange: start stage so player + enemy are alive ─────────────────
    battleService->StartStage();

    // Mutate player so we have non-default HP
    Player& player = battleService->GetPlayer();
    const int origMaxHp = player.GetHealthComponent().GetMaxHealth();
    player.GetHealthComponent().SetHealth(origMaxHp - 4);
    player.SetMp(5);

    // Mutate enemy offset
    Enemy& enemy = battleService->GetEnemy();
    enemy.SetRockOffset(2);

    // Draw a card so the hand pile is non-empty
    cardService->DrawCard();

    // Capture expected values before save
    const int  expectedCurrentIndex = battleService->GetCurrentEnemyIndex();
    const int  expectedPlayerHp     = player.GetHealthComponent().GetHealth();
    const int  expectedEnemyMaxHp   = enemy.GetHealthComponent().GetMaxHealth();
    const int  expectedRockOffset   = enemy.GetRockOffset();
    const std::size_t expectedHandSize    = cardService->GetHand().size();
    const std::size_t expectedDrawSize    = cardService->GetDrawPile().size();
    const std::size_t expectedDiscardSize = cardService->GetDiscardPile().size();

    // ── Wire mock: SaveGame captures the state; LoadGame returns it ───────
    GameState capturedState;
    ON_CALL(mockConfig, SaveGame(_, _))
        .WillByDefault(
            ::testing::DoAll(
                ::testing::SaveArg<1>(&capturedState),
                Return(true)
            ));
    ON_CALL(mockConfig, LoadGame(_))
        .WillByDefault([&capturedState](int) -> std::optional<GameState> {
            return capturedState;
        });

    // ── Act ───────────────────────────────────────────────────────────────
    battleService->SaveState(0);
    ASSERT_TRUE(battleService->LoadState(0))  << "LoadState must return true";

    // ── Assert ────────────────────────────────────────────────────────────
    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), expectedCurrentIndex)
        << "currentIndex must be restored after round-trip";

    const Player& loadedPlayer = battleService->GetPlayer();
    EXPECT_EQ(loadedPlayer.GetHealthComponent().GetHealth(), expectedPlayerHp)
        << "player HP must be restored after round-trip";

    const Enemy& loadedEnemy = battleService->GetEnemy();
    EXPECT_EQ(loadedEnemy.GetHealthComponent().GetMaxHealth(), expectedEnemyMaxHp)
        << "enemy maxHp must be restored after round-trip";
    EXPECT_EQ(loadedEnemy.GetRockOffset(), expectedRockOffset)
        << "enemy rockOffset must be restored after round-trip";

    EXPECT_EQ(cardService->GetHand().size(),        expectedHandSize)
        << "hand size must be restored after round-trip";
    EXPECT_EQ(cardService->GetDrawPile().size(),    expectedDrawSize)
        << "drawPile size must be restored after round-trip";
    EXPECT_EQ(cardService->GetDiscardPile().size(), expectedDiscardSize)
        << "discardPile size must be restored after round-trip";
}

/// Requirement 6.6:
/// LoadState returns false (and does not modify in-memory state) when
/// LoadGame returns std::nullopt (absent / corrupt save file).
TEST_F(BattleServiceMockConfigTest, BattleService_LoadState_ReturnsFalse_ForAbsentFile)
{
    // Start stage so there is valid in-memory state to leave unchanged.
    battleService->StartStage();
    const int indexBefore = battleService->GetCurrentEnemyIndex();

    // Mock: LoadGame returns nullopt (file absent)
    ON_CALL(mockConfig, LoadGame(_))
        .WillByDefault(Return(std::optional<GameState>(std::nullopt)));

    // ── Act ───────────────────────────────────────────────────────────────
    const bool result = battleService->LoadState(1);

    // ── Assert ────────────────────────────────────────────────────────────
    EXPECT_FALSE(result)
        << "LoadState must return false when LoadGame returns nullopt";

    // In-memory state must be unchanged (currentIndex not modified)
    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), indexBefore)
        << "currentIndex must be unchanged when LoadState fails";
}

/// Requirement 6.8:
/// LoadState returns false when the saved sequence contains a sprite integer
/// that ParseSprite cannot map to any EnemyConfig in the config pool.
/// We build a valid GameState with ESprite::Bunny in the sequence, but
/// make ParseSprite return ESprite::Null for every string so that
/// FindEnemyConfigBySprite finds no match.
TEST_F(BattleServiceMockConfigTest, BattleService_LoadState_ReturnsFalse_ForUnknownSprite)
{
    // Build a GameState whose sequence holds ESprite::Bunny (int).
    GameState stateWithUnknownSprite;
    stateWithUnknownSprite.playerHp                  = 10;
    stateWithUnknownSprite.playerMaxHp               = 15;
    stateWithUnknownSprite.playerMp                  = 3;
    stateWithUnknownSprite.playerSprite              = static_cast<int>(ESprite::MeowingCat);
    stateWithUnknownSprite.playerHealUses            = 0;
    stateWithUnknownSprite.playerHasUsedClairvoyance = false;

    stateWithUnknownSprite.enemyHp             = 10;
    stateWithUnknownSprite.enemyMaxHp          = 20;
    stateWithUnknownSprite.enemySprite         = static_cast<int>(ESprite::Bunny);
    stateWithUnknownSprite.enemyRockOffset     = 0;
    stateWithUnknownSprite.enemyScissorsOffset = 0;
    stateWithUnknownSprite.enemyPaperOffset    = 0;

    stateWithUnknownSprite.currentIndex = 0;
    // Sequence contains ESprite::Bunny — but we will tell ParseSprite to
    // return Null for all strings, so FindEnemyConfigBySprite won't match.
    stateWithUnknownSprite.sequence = { static_cast<int>(ESprite::Bunny) };

    stateWithUnknownSprite.hand        = {};
    stateWithUnknownSprite.drawPile    = { CardData{0, 2} };
    stateWithUnknownSprite.discardPile = {};

    // LoadGame returns the crafted state
    ON_CALL(mockConfig, LoadGame(_))
        .WillByDefault(Return(std::optional<GameState>(stateWithUnknownSprite)));

    // ParseSprite now returns Null for every sprite name — simulates the
    // situation where the asset service cannot resolve the sprite in the pool.
    ON_CALL(mockAsset, ParseSprite(_))
        .WillByDefault(Return(ESprite::Null));

    // ── Act ───────────────────────────────────────────────────────────────
    const bool result = battleService->LoadState(0);

    // ── Assert ────────────────────────────────────────────────────────────
    EXPECT_FALSE(result)
        << "LoadState must return false when a sprite in the sequence has no "
           "matching EnemyConfig (ParseSprite returns Null for all names)";
}

// =============================================================================
// END SECTION_13_5
// =============================================================================
// =============================================================================
// SECTION_13_6: AudioService construction and volume behaviour via MockConfigService
//
// Requirements: 7.2, 7.3, 7.5, 7.6, 7.7, 7.9
// =============================================================================

using ::testing::DoAll;
using ::testing::SetArgReferee;
using ::testing::StrictMock;
using ::testing::InSequence;
using ::testing::AtLeast;

// ---------------------------------------------------------------------------
// Fixture for SECTION_13_6
//
// AudioService is a private inner class; construction goes through the
// CreateAudioService(IConfigService&, IAssetService&, IBattleService&) factory.
//
// MockBattleService is wrapped in NiceMock<> so that calls to GetPlayer /
// GetEnemy (which may or may not occur during construction) don't emit
// uninteresting-call warnings.
// ---------------------------------------------------------------------------

class AudioServiceTest : public ::testing::Test
{
protected:
    // Volume levels returned by LoadSoundSettings when load succeeds.
    static constexpr int kLoadedMaster = 2;
    static constexpr int kLoadedBgm    = 1;
    static constexpr int kLoadedSfx    = 3;

    NiceMock<MockConfigService>  mockConfig;
    NiceMock<MockAssetService>   mockAsset;
    NiceMock<MockBattleService>  mockBattle;

    // Helper config stubs shared across all tests in this fixture.
    std::vector<EnemyConfig>  enemies = {
        EnemyConfig{ 20, 5, 1, 2, 3, L"TestBunny", "Bunny" },
    };
    std::vector<CardConfig>   cards   = { { 0, 2 } };
    PlayerConfig              playerCfg = Player::GetDefaultConfig();
    GameConfig                gameCfg{ 1 };

    void SetUp() override
    {
        // Wire the config getters required by MockConfigService.
        ON_CALL(mockConfig, GetEnemyConfigs())
            .WillByDefault(ReturnRef(enemies));
        ON_CALL(mockConfig, GetCardConfigs())
            .WillByDefault(ReturnRef(cards));
        ON_CALL(mockConfig, GetPlayerConfig())
            .WillByDefault(ReturnRef(playerCfg));
        ON_CALL(mockConfig, GetGameConfig())
            .WillByDefault(ReturnRef(gameCfg));

        // AudioService queries the asset service for the BGM sound handle in
        // its constructor.  Return -1 (invalid) so no real DxLib call happens.
        ON_CALL(mockAsset, GetSoundHandle(_))
            .WillByDefault(Return(-1));
    }

    /// Create an AudioService whose LoadSoundSettings succeeds with
    /// (kLoadedMaster, kLoadedBgm, kLoadedSfx).
    std::unique_ptr<IAudioService> MakeAudioServiceWithLoadSuccess()
    {
        ON_CALL(mockConfig, LoadSoundSettings(_, _, _))
            .WillByDefault(DoAll(
                SetArgReferee<0>(kLoadedMaster),
                SetArgReferee<1>(kLoadedBgm),
                SetArgReferee<2>(kLoadedSfx),
                Return(true)));
        return CreateAudioService(mockConfig, mockAsset, mockBattle);
    }

    /// Create an AudioService whose LoadSoundSettings fails (file absent).
    std::unique_ptr<IAudioService> MakeAudioServiceWithLoadFailure()
    {
        ON_CALL(mockConfig, LoadSoundSettings(_, _, _))
            .WillByDefault(Return(false));
        ON_CALL(mockConfig, SaveSoundSettings(_, _, _))
            .WillByDefault(Return(true));
        return CreateAudioService(mockConfig, mockAsset, mockBattle);
    }
};

// ---------------------------------------------------------------------------
// Test 1: Construction calls LoadSoundSettings exactly once
// Requirement 7.5
// ---------------------------------------------------------------------------

/// Requirement 7.5: The AudioService constructor calls IConfigService::LoadSoundSettings
/// exactly once (out-param overload) to restore persisted volume levels.
TEST_F(AudioServiceTest, AudioService_Construction_CallsLoadSoundSettings_Once)
{
    EXPECT_CALL(mockConfig, LoadSoundSettings(_, _, _))
        .Times(1)
        .WillOnce(DoAll(
            SetArgReferee<0>(kLoadedMaster),
            SetArgReferee<1>(kLoadedBgm),
            SetArgReferee<2>(kLoadedSfx),
            Return(true)));

    auto audio = CreateAudioService(mockConfig, mockAsset, mockBattle);
    (void)audio;
}

// ---------------------------------------------------------------------------
// Test 2: Construction when LoadSoundSettings returns false → defaults to 4,4,4
//         and calls SaveSoundSettings(4,4,4) exactly once.
// Requirements 7.6, 13.2
// ---------------------------------------------------------------------------

/// Requirement 7.6: When LoadSoundSettings returns false (file absent), the
/// AudioService SHALL default all three volume levels to 4 and call
/// IConfigService::SaveSoundSettings(4, 4, 4) exactly once.
TEST_F(AudioServiceTest, AudioService_Construction_WhenLoadFails_DefaultsTo4_AndSavesSoundSettings)
{
    constexpr int kDefault = VOLUME_LEVEL_COUNT - 1;  // 4

    ON_CALL(mockConfig, LoadSoundSettings(_, _, _))
        .WillByDefault(Return(false));

    EXPECT_CALL(mockConfig, SaveSoundSettings(kDefault, kDefault, kDefault))
        .Times(1)
        .WillOnce(Return(true));

    auto audio = CreateAudioService(mockConfig, mockAsset, mockBattle);

    EXPECT_EQ(audio->GetMasterVolume(), kDefault)
        << "master volume must default to 4 when LoadSoundSettings fails";
    EXPECT_EQ(audio->GetBgmVolume(), kDefault)
        << "bgm volume must default to 4 when LoadSoundSettings fails";
    EXPECT_EQ(audio->GetSfxVolume(), kDefault)
        << "sfx volume must default to 4 when LoadSoundSettings fails";
}

// ---------------------------------------------------------------------------
// Test 3: SetMasterVolume calls SaveSoundSettings with (v, bgm, sfx) once
// Requirement 7.3
// ---------------------------------------------------------------------------

/// Requirement 7.3: Each call to SetMasterVolume(v) SHALL call
/// IConfigService::SaveSoundSettings(v, bgmVolume, sfxVolume) exactly once.
TEST_F(AudioServiceTest, AudioService_SetMasterVolume_CallsSaveSoundSettings)
{
    auto audio = MakeAudioServiceWithLoadSuccess();

    constexpr int kNewMaster = 1;
    // After construction with (kLoadedMaster=2, kLoadedBgm=1, kLoadedSfx=3),
    // setting master to 1 should call SaveSoundSettings(1, 1, 3).
    EXPECT_CALL(mockConfig, SaveSoundSettings(kNewMaster, kLoadedBgm, kLoadedSfx))
        .Times(1)
        .WillOnce(Return(true));

    audio->SetMasterVolume(kNewMaster);

    EXPECT_EQ(audio->GetMasterVolume(), kNewMaster)
        << "GetMasterVolume must reflect the newly set value";
}

// ---------------------------------------------------------------------------
// Test 4: Volume clamping — SetMasterVolume(-1) → GetMasterVolume() == 0
// Requirement 7.2
// ---------------------------------------------------------------------------

/// Requirement 7.2: SetMasterVolume(-1) must clamp to 0 (lower bound).
TEST_F(AudioServiceTest, AudioService_VolumeClamping_SetMasterVolume_Negative)
{
    auto audio = MakeAudioServiceWithLoadSuccess();

    ON_CALL(mockConfig, SaveSoundSettings(_, _, _))
        .WillByDefault(Return(true));

    audio->SetMasterVolume(-1);

    EXPECT_EQ(audio->GetMasterVolume(), 0)
        << "SetMasterVolume(-1) must be clamped to 0";
}

// ---------------------------------------------------------------------------
// Test 5: Volume clamping — SetMasterVolume(99) → GetMasterVolume() == 4
// Requirement 7.2
// ---------------------------------------------------------------------------

/// Requirement 7.2: SetMasterVolume(99) must clamp to VOLUME_LEVEL_COUNT - 1 (= 4).
TEST_F(AudioServiceTest, AudioService_VolumeClamping_SetMasterVolume_TooHigh)
{
    auto audio = MakeAudioServiceWithLoadSuccess();

    ON_CALL(mockConfig, SaveSoundSettings(_, _, _))
        .WillByDefault(Return(true));

    audio->SetMasterVolume(99);

    EXPECT_EQ(audio->GetMasterVolume(), VOLUME_LEVEL_COUNT - 1)
        << "SetMasterVolume(99) must be clamped to VOLUME_LEVEL_COUNT - 1 (= 4)";
}

// =============================================================================
// END SECTION_13_6
// =============================================================================

} // namespace
} // namespace mc
