// Property-based test for BattleService::LoadEnemy — sprite resolution routing.
// Verifies that LoadEnemy routes sprite name resolution through IAssetService::ParseSprite.
//
// Feature: code-review-fixes, Property 1: LoadEnemy routes sprite resolution
// Validates: Requirements 3.2, 3.4

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

// ---------------------------------------------------------------------------
// Stub helpers
// ---------------------------------------------------------------------------

/// Minimal config service that vends a single enemy entry.
class SingleEnemyConfigService : public IConfigService
{
    std::vector<EnemyConfig> enemies_;
    std::vector<CardConfig>  cards_;
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
};

// ---------------------------------------------------------------------------
// Property 1: LoadEnemy routes sprite resolution through IAssetService
//
// For any spriteName string, BattleService::LoadEnemy must call
// IAssetService::ParseSprite exactly once with that spriteName and use the
// returned ESprite value.
// ---------------------------------------------------------------------------

// **Validates: Requirements 3.2, 3.4**
RC_GTEST_PROP(BattleServiceParseSpriteProperty,
              LoadEnemy_CallsParseSprite_ExactlyOnce_WithGivenSpriteName, ())
{
    // Generate an arbitrary sprite name string.
    // Filter out strings with embedded null characters as std::string equality
    // matching in GMock works on the full string value.
    auto spriteName = *rc::gen::suchThat(
        rc::gen::arbitrary<std::string>(),
        [](const std::string& s) {
            return s.find('\0') == std::string::npos;
        }
    );

    // Build a minimal EnemyConfig using the generated spriteName.
    // Use fixed values for int fields and a simple wide name.
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

    // Use StrictMock to catch any unexpected calls on the asset service.
    StrictMock<MockAssetService> mockAsset;

    // Property assertion: ParseSprite must be called exactly once with spriteName.
    // Return ESprite::Null (default zero-init) for any unrecognised name — this
    // matches Requirement 3.4 (fallback behaviour is preserved).
    EXPECT_CALL(mockAsset, ParseSprite(spriteName))
        .Times(1)
        .WillOnce(Return(static_cast<ESprite>(0)));

    auto cardService   = CreateCardService(configService);
    auto battleService = CreateBattleService(configService, *cardService, mockAsset);

    // LoadEnemy is what we want to exercise directly.
    battleService->LoadEnemy(config);

    // GMock verifies EXPECT_CALL on mock destruction (end of this iteration).
}

// ---------------------------------------------------------------------------
// Property 1b: LoadEnemy routes ParseSprite even when called via StartStage
//
// StartStage calls LoadEnemy internally for the first enemy in the sequence.
// Verify ParseSprite is called for each LoadEnemy invocation that occurs.
// ---------------------------------------------------------------------------

// **Validates: Requirements 3.2, 3.4**
RC_GTEST_PROP(BattleServiceParseSpriteProperty,
              StartStage_TriggersParseSprite_ForFirstEnemy, ())
{
    // Generate a sprite name without embedded nulls.
    auto spriteName = *rc::gen::suchThat(
        rc::gen::arbitrary<std::string>(),
        [](const std::string& s) {
            return s.find('\0') == std::string::npos;
        }
    );

    // Use the same sprite name for all three enemies in the pool so that any
    // call to ParseSprite hits our expectation regardless of which enemies are
    // selected by StartStage.
    std::vector<EnemyConfig> pool = {
        EnemyConfig{ 10, 1, 1, 1, 1, L"EnemyA", spriteName },
        EnemyConfig{ 12, 1, 2, 1, 1, L"EnemyB", spriteName },
        EnemyConfig{ 15, 1, 1, 3, 1, L"EnemyC", spriteName },
    };

    class MultiEnemyConfigService : public IConfigService
    {
        std::vector<EnemyConfig> enemies_;
        std::vector<CardConfig>  cards_ = { { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 } };
    public:
        explicit MultiEnemyConfigService(std::vector<EnemyConfig> e)
            : enemies_(std::move(e)) {}
        const std::vector<CardConfig>&  GetCardConfigs()  const override { return cards_; }
        const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemies_; }
    };

    MultiEnemyConfigService configService(pool);

    // NiceMock: we only assert on ParseSprite, other calls (if any) are irrelevant.
    ::testing::NiceMock<MockAssetService> mockAsset;

    // StartStage selects 3 enemies and calls LoadEnemy once for the first enemy.
    // That single LoadEnemy call must invoke ParseSprite exactly once.
    EXPECT_CALL(mockAsset, ParseSprite(spriteName))
        .Times(::testing::AtLeast(1))
        .WillRepeatedly(Return(static_cast<ESprite>(0)));

    auto cardService   = CreateCardService(configService);
    auto battleService = CreateBattleService(configService, *cardService, mockAsset);

    battleService->StartStage();

    // Explicitly verify expectations before end of iteration.
    ::testing::Mock::VerifyAndClearExpectations(&mockAsset);
}

} // namespace
} // namespace mc
