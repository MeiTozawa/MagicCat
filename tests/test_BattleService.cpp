// Unit tests for IBattleService — sequence management, enemy/player death handling,
// and state transitions.

// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before gtest includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>

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

class StubConfigService : public IConfigService
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
};

// Minimal Character subclass that carries a specific tag — used to fire DeathEvents.
class TaggedCharacter : public Character
{
public:
    explicit TaggedCharacter(ETag tag)
    {
        tags.push_back(tag);
    }
    void TakeDamage(int) const override {}
};

class BattleServiceTest : public ::testing::Test
{
protected:
    StubConfigService              configService;
    NiceMock<MockAssetService>     mockAssetService;
    std::unique_ptr<ICardService>  cardService;
    std::unique_ptr<IBattleService> battleService;

    void SetUp() override
    {
        cardService   = CreateCardService(configService);
        battleService = CreateBattleService(configService, *cardService, mockAssetService);
    }

    void TearDown() override
    {
        // Destroy BattleService first to unsubscribe its DeathEvent listener.
        battleService.reset();
        cardService.reset();
    }

    static void PublishDeath(Character& victim)
    {
        EventBus::Publish(DeathEvent(&victim));
    }
};

TEST_F(BattleServiceTest, StartStage_GeneratesSequenceOfThree)
{
    battleService->StartStage();
    EXPECT_EQ(battleService->GetSequence().size(), 3u);
}

TEST_F(BattleServiceTest, StartStage_SetsCurrentIndexToZero)
{
    battleService->StartStage();
    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 0);
}

TEST_F(BattleServiceTest, StartStage_PublishesStageStartedEvent)
{
    bool received = false;
    auto handle = EventBus::Subscribe<StageStartedEvent>(
        [&](const StageStartedEvent&) { received = true; });

    battleService->StartStage();

    EXPECT_TRUE(received);
    EventBus::Unsubscribe(handle);
}

TEST_F(BattleServiceTest, StartStage_LoadsFirstEnemyFromSequence)
{
    battleService->StartStage();
    const auto& seq   = battleService->GetSequence();
    const auto& enemy = battleService->GetEnemy();

    ASSERT_FALSE(seq.empty());
    EXPECT_EQ(enemy.GetName(),                             seq[0].name);
    EXPECT_EQ(enemy.GetBaseDamage(EAttackType::Rock),     seq[0].rockDamage);
    EXPECT_EQ(enemy.GetBaseDamage(EAttackType::Scissors), seq[0].scissorsDamage);
    EXPECT_EQ(enemy.GetBaseDamage(EAttackType::Paper),    seq[0].paperDamage);
}

TEST_F(BattleServiceTest, EnemyDeath_AtIndex0_AdvancesIndex)
{
    battleService->StartStage();

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);

    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 1);
}

TEST_F(BattleServiceTest, EnemyDeath_AtIndex0_PublishesEnemyDefeatedEvent)
{
    battleService->StartStage();

    int capturedIndex = -1;
    auto handle = EventBus::Subscribe<EnemyDefeatedEvent>(
        [&](const EnemyDefeatedEvent& e) { capturedIndex = e.defeatedIndex; });

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);

    EXPECT_EQ(capturedIndex, 0);
    EventBus::Unsubscribe(handle);
}

TEST_F(BattleServiceTest, EnemyDeath_AtIndex1_AdvancesIndex)
{
    battleService->StartStage();

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);
    PublishDeath(enemy);

    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 2);
}

TEST_F(BattleServiceTest, EnemyDeath_AtIndex2_PublishesStageClearEvent)
{
    battleService->StartStage();

    bool received = false;
    auto handle = EventBus::Subscribe<StageClearEvent>(
        [&](const StageClearEvent&) { received = true; });

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);
    PublishDeath(enemy);
    PublishDeath(enemy);

    EXPECT_TRUE(received);
    EventBus::Unsubscribe(handle);
}

TEST_F(BattleServiceTest, EnemyDeath_AtIndex2_ResetsState)
{
    battleService->StartStage();

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);
    PublishDeath(enemy);
    PublishDeath(enemy);

    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(battleService->GetSequence().empty());
}

TEST_F(BattleServiceTest, EnemyDeath_AtIndex0_LoadsNextEnemy)
{
    battleService->StartStage();

    // Capture the full sequence before any kill.
    const std::vector<EnemyConfig> seq = battleService->GetSequence();
    ASSERT_GE(seq.size(), 2u);

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);

    const auto& currentEnemy = battleService->GetEnemy();
    EXPECT_EQ(currentEnemy.GetName(),                             seq[1].name);
    EXPECT_EQ(currentEnemy.GetBaseDamage(EAttackType::Rock),     seq[1].rockDamage);
    EXPECT_EQ(currentEnemy.GetBaseDamage(EAttackType::Scissors), seq[1].scissorsDamage);
    EXPECT_EQ(currentEnemy.GetBaseDamage(EAttackType::Paper),    seq[1].paperDamage);
}

TEST_F(BattleServiceTest, PlayerDeath_PublishesStageFailEvent)
{
    battleService->StartStage();

    bool received = false;
    auto handle = EventBus::Subscribe<StageFailEvent>(
        [&](const StageFailEvent&) { received = true; });

    TaggedCharacter player(ETag::Player);
    PublishDeath(player);

    EXPECT_TRUE(received);
    EventBus::Unsubscribe(handle);
}

TEST_F(BattleServiceTest, PlayerDeath_ResetsState)
{
    battleService->StartStage();

    TaggedCharacter player(ETag::Player);
    PublishDeath(player);

    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(battleService->GetSequence().empty());
}

TEST_F(BattleServiceTest, ConsecutiveStartStage_GeneratesNewSequence)
{
    battleService->StartStage();
    ASSERT_EQ(battleService->GetSequence().size(), 3u);

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);
    PublishDeath(enemy);
    PublishDeath(enemy);

    battleService->StartStage();
    EXPECT_EQ(battleService->GetSequence().size(), 3u);
    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 0);
}

// Even with a pool of only 1 enemy, StartStage still produces a sequence of 3
// (duplicates are allowed).
TEST_F(BattleServiceTest, StartStage_WithSmallPool_StillGeneratesThreeEnemies)
{
    StubConfigService tinyConfig(
        { EnemyConfig{ 10, 1, 1, 1, 1, L"Lone", "Bunny" } });
    auto cs  = CreateCardService(tinyConfig);
    NiceMock<MockAssetService> mockAsset;
    auto bs  = CreateBattleService(tinyConfig, *cs, mockAsset);

    bs->StartStage();
    EXPECT_EQ(bs->GetSequence().size(), 3u);
}

} // namespace
} // namespace mc
