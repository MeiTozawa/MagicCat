// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before gtest includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

import BattleService;
import CardService;
import ConfigService;
import AssetService;
import EventBus;
import Enemy;
import Player;

#include "MockServices.h"

using ::testing::Return;
using ::testing::StrictMock;

namespace mc {
namespace {

/// Minimal config service that vends a single enemy entry.
class SingleEnemyConfigService : public IConfigService
{
    std::vector<EnemyConfig> enemies_;
    std::vector<CardConfig>  cards_;
    PlayerConfig playerConfig_ = Player::GetDefaultConfig();
    GameConfig gameConfig_{ 3 };
public:
    explicit SingleEnemyConfigService(EnemyConfig enemy,
                                      std::vector<CardConfig> cards = {
                                          { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 },
                                      })
        : enemies_{ std::move(enemy) }
        , cards_(std::move(cards))
    {}

    const std::vector<CardConfig>&  GetCardConfigs()  const override { return cards_;   }
    const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemies_; }
    const PlayerConfig& GetPlayerConfig() const override { return playerConfig_; }
    const GameConfig& GetGameConfig() const override { return gameConfig_; }
    bool LoadSoundSettings(int&, int&, int&) override { return false; }
    bool SaveSoundSettings(int, int, int) override { return true; }
};

// For any spriteName string, BattleService::LoadEnemy must call
// IAssetService::ParseSprite exactly once with that spriteName.
RC_GTEST_PROP(BattleServiceParseSpriteProperty,
              LoadEnemy_CallsParseSprite_ExactlyOnce_WithGivenSpriteName, ())
{
    // Filter out strings with embedded null characters (GMock string matching works on full value).
    auto spriteName = *rc::gen::suchThat(
        rc::gen::arbitrary<std::string>(),
        [](const std::string& s) {
            return s.find('\0') == std::string::npos;
        }
    );

    EnemyConfig config{
        /* hp            */ 10,
        /* baseWeight    */ 1,
        /* rockDamage    */ 1,
        /* scissorsDamage*/ 1,
        /* paperDamage   */ 1,
        /* name          */ L"TestEnemy",
        /* spriteName    */ spriteName
    };

    SingleEnemyConfigService configService(config);

    StrictMock<MockAssetService> mockAsset;

    // ESprite::Null (0) matches the fallback behaviour for unrecognised names.
    EXPECT_CALL(mockAsset, ParseSprite(spriteName))
        .Times(1)
        .WillOnce(Return(static_cast<ESprite>(0)));

    auto cardService   = CreateCardService(configService);
    auto battleService = CreateBattleService(configService, *cardService, mockAsset);

    battleService->LoadEnemy(config);
}

// StartStage calls LoadEnemy internally for the first enemy; verify ParseSprite
// is called at least once during StartStage.
RC_GTEST_PROP(BattleServiceParseSpriteProperty,
              StartStage_TriggersParseSprite_ForFirstEnemy, ())
{
    auto spriteName = *rc::gen::suchThat(
        rc::gen::arbitrary<std::string>(),
        [](const std::string& s) {
            return s.find('\0') == std::string::npos;
        }
    );

    // Use the same sprite name for all three enemies so any selection hits our expectation.
    std::vector<EnemyConfig> pool = {
        EnemyConfig{ 10, 1, 1, 1, 1, L"EnemyA", spriteName },
        EnemyConfig{ 12, 1, 2, 1, 1, L"EnemyB", spriteName },
        EnemyConfig{ 15, 1, 1, 3, 1, L"EnemyC", spriteName },
    };

    class MultiEnemyConfigService : public IConfigService
    {
        std::vector<EnemyConfig> enemies_;
        std::vector<CardConfig>  cards_ = { { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 } };
        PlayerConfig playerConfig_ = Player::GetDefaultConfig();
        GameConfig gameConfig_{ 3 };
    public:
        explicit MultiEnemyConfigService(std::vector<EnemyConfig> e)
            : enemies_(std::move(e)) {}
        const std::vector<CardConfig>&  GetCardConfigs()  const override { return cards_; }
        const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemies_; }
        const PlayerConfig& GetPlayerConfig() const override { return playerConfig_; }
        const GameConfig& GetGameConfig() const override { return gameConfig_; }
        bool LoadSoundSettings(int&, int&, int&) override { return false; }
        bool SaveSoundSettings(int, int, int) override { return true; }
    };

    MultiEnemyConfigService configService(pool);

    ::testing::NiceMock<MockAssetService> mockAsset;

    EXPECT_CALL(mockAsset, ParseSprite("MeowingCat"))
        .Times(::testing::AnyNumber())
        .WillRepeatedly(Return(static_cast<ESprite>(0)));

    EXPECT_CALL(mockAsset, ParseSprite(spriteName))
        .Times(::testing::AtLeast(1))
        .WillRepeatedly(Return(static_cast<ESprite>(0)));

    auto cardService   = CreateCardService(configService);
    auto battleService = CreateBattleService(configService, *cardService, mockAsset);

    battleService->StartStage();

    ::testing::Mock::VerifyAndClearExpectations(&mockAsset);
}

} // namespace
} // namespace mc
