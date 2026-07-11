// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before gtest includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

import BattleService;
import CardService;
import ConfigService;
import EventBus;
import Character;
import Enemy;
import Player;
import HealthComponent;

#include "MockServices.h"

using ::testing::NiceMock;

namespace mc {
namespace {

class StubConfigService : public IConfigService, public IPersistenceService
{
    std::vector<EnemyConfig> enemies_;
    std::vector<CardConfig>  cards_;
    PlayerConfig playerConfig_ = Player::GetDefaultConfig();
    GameConfig gameConfig_{ 3 };
public:
    explicit StubConfigService(
        std::vector<EnemyConfig> enemies = {
            EnemyConfig{ 10, 1, 1, 1, 1, L"EnemyA", "Bunny" },
            EnemyConfig{ 12, 1, 2, 1, 1, L"EnemyB", "Bunny" },
            EnemyConfig{ 15, 1, 1, 3, 1, L"EnemyC", "Bunny" },
        },
        std::vector<CardConfig> cards = {
            { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 },
        })
        : enemies_(std::move(enemies)), cards_(std::move(cards)) {}

    const std::vector<CardConfig>&  GetCardConfigs()  const override { return cards_;   }
    const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemies_; }
    const PlayerConfig& GetPlayerConfig() const override { return playerConfig_; }
    const GameConfig& GetGameConfig() const override { return gameConfig_; }
    bool LoadSoundSettings(int&, int&, int&) override { return false; }
    bool SaveSoundSettings(int, int, int) override { return true; }
    bool SaveGame(int, const GameState&) override { return true; }
    std::optional<GameState> LoadGame(int) override { return std::nullopt; }
    SaveMetadata GetSaveMetadata(int) override { return SaveMetadata{}; }
};

class TaggedCharacter : public Character
{
public:
    explicit TaggedCharacter(ETag tag)
    {
        tags.push_back(tag);
    }
    void TakeDamage(int) const override {}
};

RC_GTEST_PROP(BattleServiceProperties, SequenceAlwaysHasThreeEntries, ())
{
    StubConfigService cfg;
    NiceMock<MockAssetService> mockAsset;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, cfg, *cs, mockAsset);

    bs->StartStage();

    RC_ASSERT(bs->GetSequence().size() == 3u);
}

RC_GTEST_PROP(BattleServiceProperties, SequenceEntriesAreFromPool, ())
{
    StubConfigService cfg;
    NiceMock<MockAssetService> mockAsset;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, cfg, *cs, mockAsset);

    bs->StartStage();

    const auto& pool = cfg.GetEnemyConfigs();
    for (const auto& entry : bs->GetSequence())
    {
        bool found = false;
        for (const auto& poolEntry : pool)
        {
            if (poolEntry.name == entry.name)
            {
                found = true;
                break;
            }
        }
        RC_ASSERT(found);
    }
}

RC_GTEST_PROP(BattleServiceProperties, CurrentIndexAlwaysStartsAtZero, ())
{
    StubConfigService cfg;
    NiceMock<MockAssetService> mockAsset;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, cfg, *cs, mockAsset);

    bs->StartStage();

    RC_ASSERT(bs->GetCurrentEnemyIndex() == 0);
}

RC_GTEST_PROP(BattleServiceProperties, EnemyDeathAlwaysIncrementsIndex, ())
{
    StubConfigService cfg;
    NiceMock<MockAssetService> mockAsset;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, cfg, *cs, mockAsset);

    bs->StartStage();
    RC_ASSERT(bs->GetCurrentEnemyIndex() == 0);

    TaggedCharacter enemy(ETag::Enemy);
    EventBus::Publish(DeathEvent(&enemy));

    RC_ASSERT(bs->GetCurrentEnemyIndex() == 1);
}

RC_GTEST_PROP(BattleServiceProperties, ThreeConsecutiveEnemyDeaths_StageClear, ())
{
    StubConfigService cfg;
    NiceMock<MockAssetService> mockAsset;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, cfg, *cs, mockAsset);

    bs->StartStage();

    int stageClearCount = 0;
    auto handle = EventBus::Subscribe<StageClearEvent>(
        [&](const StageClearEvent&) { ++stageClearCount; });

    TaggedCharacter enemy(ETag::Enemy);
    EventBus::Publish(DeathEvent(&enemy));
    EventBus::Publish(DeathEvent(&enemy));
    EventBus::Publish(DeathEvent(&enemy));

    EventBus::Unsubscribe(handle);

    RC_ASSERT(stageClearCount == 1);
    RC_ASSERT(bs->GetCurrentEnemyIndex() == 0);
}

} // namespace
} // namespace mc
