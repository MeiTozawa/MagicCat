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

// ---------------------------------------------------------------------------
// Stub implementations
// ---------------------------------------------------------------------------

class StubConfigService : public IConfigService
{
    std::vector<EnemyConfig> enemies_;
    std::vector<CardConfig>  cards_;
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

// ---------------------------------------------------------------------------
// Test fixture
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

// 1. StartStage generates a sequence of exactly 3 enemies.
TEST_F(BattleServiceTest, StartStage_GeneratesSequenceOfThree)
{
    battleService->StartStage();
    EXPECT_EQ(battleService->GetSequence().size(), 3u);
}

// 2. StartStage resets the current enemy index to 0.
TEST_F(BattleServiceTest, StartStage_SetsCurrentIndexToZero)
{
    battleService->StartStage();
    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 0);
}

// 3. StartStage publishes StageStartedEvent.
TEST_F(BattleServiceTest, StartStage_PublishesStageStartedEvent)
{
    bool received = false;
    auto handle = EventBus::Subscribe<StageStartedEvent>(
        [&](const StageStartedEvent&) { received = true; });

    battleService->StartStage();

    EXPECT_TRUE(received);
    EventBus::Unsubscribe(handle);
}

// 4. StartStage loads the first enemy from the sequence into GetEnemy().
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

// 5. An enemy death at index 0 advances the index to 1.
TEST_F(BattleServiceTest, EnemyDeath_AtIndex0_AdvancesIndex)
{
    battleService->StartStage();

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);

    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 1);
}

// 6. An enemy death at index 0 publishes EnemyDefeatedEvent with defeatedIndex == 0.
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

// 7. Two enemy deaths advance the index to 2.
TEST_F(BattleServiceTest, EnemyDeath_AtIndex1_AdvancesIndex)
{
    battleService->StartStage();

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy); // 0 → 1
    PublishDeath(enemy); // 1 → 2

    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 2);
}

// 8. Three enemy deaths publish StageClearEvent.
TEST_F(BattleServiceTest, EnemyDeath_AtIndex2_PublishesStageClearEvent)
{
    battleService->StartStage();

    bool received = false;
    auto handle = EventBus::Subscribe<StageClearEvent>(
        [&](const StageClearEvent&) { received = true; });

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy); // 0 → 1
    PublishDeath(enemy); // 1 → 2
    PublishDeath(enemy); // 2 → StageClearEvent

    EXPECT_TRUE(received);
    EventBus::Unsubscribe(handle);
}

// 9. After StageClear, index is 0 and sequence is empty.
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

// 10. After the first enemy kill, GetEnemy() matches sequence[1] (captured before kill).
TEST_F(BattleServiceTest, EnemyDeath_AtIndex0_LoadsNextEnemy)
{
    battleService->StartStage();

    // Capture the full sequence before any kill.
    const std::vector<EnemyConfig> seq = battleService->GetSequence();
    ASSERT_GE(seq.size(), 2u);

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy); // kills index 0, loads index 1

    const auto& currentEnemy = battleService->GetEnemy();
    EXPECT_EQ(currentEnemy.GetName(),                             seq[1].name);
    EXPECT_EQ(currentEnemy.GetBaseDamage(EAttackType::Rock),     seq[1].rockDamage);
    EXPECT_EQ(currentEnemy.GetBaseDamage(EAttackType::Scissors), seq[1].scissorsDamage);
    EXPECT_EQ(currentEnemy.GetBaseDamage(EAttackType::Paper),    seq[1].paperDamage);
}

// 11. Player death publishes StageFailEvent.
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

// 12. Player death resets index to 0 and clears the sequence.
TEST_F(BattleServiceTest, PlayerDeath_ResetsState)
{
    battleService->StartStage();

    TaggedCharacter player(ETag::Player);
    PublishDeath(player);

    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(battleService->GetSequence().empty());
}

// 13. Two consecutive StartStage calls each produce a sequence of size 3 at index 0.
TEST_F(BattleServiceTest, ConsecutiveStartStage_GeneratesNewSequence)
{
    battleService->StartStage();
    ASSERT_EQ(battleService->GetSequence().size(), 3u);

    // Complete the first stage so state is clean.
    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);
    PublishDeath(enemy);
    PublishDeath(enemy);

    // Second stage.
    battleService->StartStage();
    EXPECT_EQ(battleService->GetSequence().size(), 3u);
    EXPECT_EQ(battleService->GetCurrentEnemyIndex(), 0);
}

// 14. Even with a pool of only 1 enemy, StartStage still produces a sequence of 3
//     (duplicates are allowed).
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
