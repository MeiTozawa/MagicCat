// Feature: sequential-enemy-battles
// Property-based tests for BattleService (tasks 3.3 and 5.4)

// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with a local variable named `small` inside RapidCheck's Text.hpp generator.
// Undefine it here before including RapidCheck headers to avoid the clash.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

import Enemy;
import Player;
import HealthComponent;
import ConfigService;
import Character;
import EventBus;
import BattleService;
import CardService;
import SceneService;

namespace mc {
namespace {

// ---------------------------------------------------------------------------
// Minimal stub IConfigService for constructing BattleService in tests
// ---------------------------------------------------------------------------
class StubConfigService : public IConfigService
{
public:
    const std::vector<CardConfig>& GetCardConfigs() const override
    {
        static const std::vector<CardConfig> empty;
        return empty;
    }

    const std::vector<EnemyConfig>& GetEnemyConfigs() const override
    {
        static const std::vector<EnemyConfig> configs = {
            EnemyConfig{ 10, 1, 1, 1, 1, L"TestEnemy", "Bunny" }
        };
        return configs;
    }
};

// ---------------------------------------------------------------------------
// Stub IConfigService with a configurable pool for BattleService tests
// ---------------------------------------------------------------------------
class PoolConfigService : public IConfigService
{
public:
    std::vector<EnemyConfig> pool;
    std::vector<CardConfig>  cards;

    explicit PoolConfigService(std::vector<EnemyConfig> p, std::vector<CardConfig> c = {})
        : pool(std::move(p)), cards(std::move(c)) {}

    const std::vector<CardConfig>& GetCardConfigs() const override { return cards; }
    const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return pool; }
};

// ---------------------------------------------------------------------------
// Stub ISceneService — does nothing (no DxLib scenes needed in unit tests)
// ---------------------------------------------------------------------------
class StubSceneService : public ISceneService
{
public:
    void Update(float) override {}
    void PushScene(ESceneState) override {}
    void PopScene() override {}
    void RegisterScene(ESceneState, std::unique_ptr<IScene>&&) override {}
    ESceneState GetCurrentScene() override { return ESceneState::Info; }
    void SetCurrentScene(ESceneState) override {}
};

// ---------------------------------------------------------------------------
// Property 6: LoadEnemy resets weight offsets to 0 and sets HP to config.hp
// Validates: Requirements 2.4
// ---------------------------------------------------------------------------
// Feature: sequential-enemy-battles, Property 6: LoadEnemy_ResetsWeightsAndHp
TEST(LoadEnemyProperties, LoadEnemy_ResetsWeightsAndHp)
{
    // Run at least 100 iterations as required by the design spec
    _putenv_s("RC_PARAMS", "max_success=100");

    auto result = rc::check(
        "LoadEnemy resets all weight offsets to 0 and sets HP to config.hp",
        []()
        {
            // Generate arbitrary EnemyConfig values
            const int hp           = *rc::gen::inRange(1, 1000);
            const int baseWeight   = *rc::gen::inRange(0, 100);
            const int rockDamage   = *rc::gen::inRange(0, 50);
            const int scissorsDmg  = *rc::gen::inRange(0, 50);
            const int paperDamage  = *rc::gen::inRange(0, 50);

            EnemyConfig config;
            config.hp             = hp;
            config.baseWeight     = baseWeight;
            config.rockDamage     = rockDamage;
            config.scissorsDamage = scissorsDmg;
            config.paperDamage    = paperDamage;
            config.name           = L"TestEnemy";
            config.spriteName     = "Bunny";

            // Construct a BattleService and call LoadEnemy
            StubConfigService configService;
            auto cardService = CreateCardService(configService);
            StubSceneService sceneService;
            auto battleService = CreateBattleService(
                configService, *cardService, sceneService);

            battleService->LoadEnemy(config);

            const Enemy& enemy = battleService->GetEnemy();

            // All weight offsets must be 0 after LoadEnemy
            RC_ASSERT(enemy.GetWeightOffset(EAttackType::Rock)     == 0);
            RC_ASSERT(enemy.GetWeightOffset(EAttackType::Scissors) == 0);
            RC_ASSERT(enemy.GetWeightOffset(EAttackType::Paper)    == 0);

            // HP must equal config.hp
            RC_ASSERT(enemy.GetHealthComponent().GetHealth() == config.hp);
        });

    EXPECT_TRUE(result);
}

// ===========================================================================
// Properties 1–5, 7–8: BattleService
// ===========================================================================

// ---------------------------------------------------------------------------
// Helper: build a pool of N unique EnemyConfigs
// ---------------------------------------------------------------------------
static EnemyConfig MakeEnemyConfig(int id)
{
    EnemyConfig c;
    c.hp             = 10 + id;
    c.baseWeight     = id;
    c.rockDamage     = 1;
    c.scissorsDamage = 1;
    c.paperDamage    = 1;
    c.name           = std::wstring(L"Enemy") + std::to_wstring(id);
    c.spriteName     = "Bunny";
    return c;
}

// ---------------------------------------------------------------------------
// Property 1: StartStage always produces a sequence of size 3 (pool >= 1)
// Validates: Requirements 1.1, 1.2
// ---------------------------------------------------------------------------
// Feature: sequential-enemy-battles, Property 1: Sequence_AlwaysHasSize3
TEST(BattleSequenceProperties, Sequence_AlwaysHasSize3)
{
    _putenv_s("RC_PARAMS", "max_success=25");

    auto result = rc::check(
        "StartStage always produces GetSequence().size() == 3 for any pool size >= 1",
        []()
        {
            // Generate a pool size in [1, 10]
            const int poolSize = *rc::gen::inRange(1, 11);

            std::vector<EnemyConfig> pool;
            pool.reserve(poolSize);
            for (int i = 0; i < poolSize; ++i)
                pool.push_back(MakeEnemyConfig(i));

            PoolConfigService configService(pool);
            auto cardService      = CreateCardService(configService);
            StubSceneService sceneService;

            auto battleService = CreateBattleService(
                configService, *cardService, sceneService);

            battleService->StartStage();

            RC_ASSERT(battleService->GetSequence().size() == 3u);
        });

    EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Property 2: When pool size >= 3, sequence entries are all distinct
// Validates: Requirements 1.1
// ---------------------------------------------------------------------------
// Feature: sequential-enemy-battles, Property 2: Sequence_NoDuplicatesWhenPoolLargeEnough
TEST(BattleSequenceProperties, Sequence_NoDuplicatesWhenPoolLargeEnough)
{
    _putenv_s("RC_PARAMS", "max_success=25");

    auto result = rc::check(
        "Sequence entries are all distinct when pool size >= 3",
        []()
        {
            // Generate pool size in [3, 10]
            const int poolSize = *rc::gen::inRange(3, 11);

            std::vector<EnemyConfig> pool;
            pool.reserve(poolSize);
            for (int i = 0; i < poolSize; ++i)
                pool.push_back(MakeEnemyConfig(i));

            PoolConfigService configService(pool);
            auto cardService      = CreateCardService(configService);
            StubSceneService sceneService;

            auto battleService = CreateBattleService(
                configService, *cardService, sceneService);

            battleService->StartStage();

            const auto& seq = battleService->GetSequence();
            RC_ASSERT(seq.size() == 3u);

            // All three names must be distinct (names are unique per MakeEnemyConfig)
            RC_ASSERT(seq[0].name != seq[1].name);
            RC_ASSERT(seq[1].name != seq[2].name);
            RC_ASSERT(seq[0].name != seq[2].name);
        });

    EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Property 3: After StartStage, GetEnemy() matches sequence[0]
// Validates: Requirements 1.4
// ---------------------------------------------------------------------------
// Feature: sequential-enemy-battles, Property 3: LoadedEnemy_MatchesSequence0
TEST(BattleSequenceProperties, LoadedEnemy_MatchesSequence0)
{
    _putenv_s("RC_PARAMS", "max_success=25");

    auto result = rc::check(
        "After StartStage, battleService.GetEnemy() has same hp/name as GetSequence()[0]",
        []()
        {
            const int poolSize = *rc::gen::inRange(1, 6);

            std::vector<EnemyConfig> pool;
            pool.reserve(poolSize);
            for (int i = 0; i < poolSize; ++i)
                pool.push_back(MakeEnemyConfig(i * 7 + 3)); // distinct hp values

            PoolConfigService configService(pool);
            auto cardService      = CreateCardService(configService);
            StubSceneService sceneService;

            auto battleService = CreateBattleService(
                configService, *cardService, sceneService);

            battleService->StartStage();

            const auto& seq   = battleService->GetSequence();
            const Enemy& enemy = battleService->GetEnemy();

            // HP should match sequence[0].hp
            RC_ASSERT(enemy.GetHealthComponent().GetHealth() == seq[0].hp);
            // Name should match sequence[0].name
            RC_ASSERT(enemy.GetName() == seq[0].name);
        });

    EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Property 4: When index is 0 or 1, enemy death increments index by 1 and
//             EnemyDefeatedEvent.defeatedIndex == old index
// Validates: Requirements 2.2
// ---------------------------------------------------------------------------
// Feature: sequential-enemy-battles, Property 4: EnemyDefeat_IncrementsIndex
TEST(BattleSequenceProperties, EnemyDefeat_IncrementsIndex)
{
    _putenv_s("RC_PARAMS", "max_success=25");

    auto result = rc::check(
        "Enemy death when index in {0,1} increments index and emits correct EnemyDefeatedEvent",
        []()
        {
            // Use a fixed pool of 3 distinct enemies
            std::vector<EnemyConfig> pool = {
                MakeEnemyConfig(0), MakeEnemyConfig(1), MakeEnemyConfig(2)
            };

            PoolConfigService configService(pool);
            auto cardService      = CreateCardService(configService);
            StubSceneService sceneService;

            auto battleService = CreateBattleService(
                configService, *cardService, sceneService);

            battleService->StartStage();

            // Pick a starting index: 0 or 1
            const int startIndex = *rc::gen::inRange(0, 2); // [0, 2) → 0 or 1

            // Advance to startIndex by defeating enemies one by one
            for (int i = 0; i < startIndex; ++i)
            {
                Enemy& e = battleService->GetEnemy();
                EventBus::Publish(DeathEvent(&e));
            }

            RC_ASSERT(battleService->GetCurrentEnemyIndex() == startIndex);

            // Capture the EnemyDefeatedEvent
            int capturedDefeatedIndex = -1;
            auto handle = EventBus::Subscribe<EnemyDefeatedEvent>(
                [&](const EnemyDefeatedEvent& ev)
                {
                    capturedDefeatedIndex = ev.defeatedIndex;
                });

            // Defeat current enemy
            Enemy& enemy = battleService->GetEnemy();
            EventBus::Publish(DeathEvent(&enemy));

            EventBus::Unsubscribe(handle);

            RC_ASSERT(battleService->GetCurrentEnemyIndex() == startIndex + 1);
            RC_ASSERT(capturedDefeatedIndex == startIndex);
        });

    EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Property 5: After enemy defeat, GetEnemy() matches sequence[newIndex]
// Validates: Requirements 2.3
// ---------------------------------------------------------------------------
// Feature: sequential-enemy-battles, Property 5: EnemyDefeat_LoadsCorrectNextEnemy
TEST(BattleSequenceProperties, EnemyDefeat_LoadsCorrectNextEnemy)
{
    _putenv_s("RC_PARAMS", "max_success=25");

    auto result = rc::check(
        "After enemy defeat at index 0 or 1, GetEnemy() matches sequence[newIndex]",
        []()
        {
            // Use a fixed pool of 3 distinct enemies with different hp
            std::vector<EnemyConfig> pool = {
                MakeEnemyConfig(0), MakeEnemyConfig(1), MakeEnemyConfig(2)
            };

            PoolConfigService configService(pool);
            auto cardService      = CreateCardService(configService);
            StubSceneService sceneService;

            auto battleService = CreateBattleService(
                configService, *cardService, sceneService);

            battleService->StartStage();

            // Pick starting index 0 or 1
            const int startIndex = *rc::gen::inRange(0, 2);

            // Advance to startIndex
            for (int i = 0; i < startIndex; ++i)
            {
                Enemy& e = battleService->GetEnemy();
                EventBus::Publish(DeathEvent(&e));
            }

            RC_ASSERT(battleService->GetCurrentEnemyIndex() == startIndex);

            // Snapshot the sequence before defeat
            const auto seqCopy = battleService->GetSequence();

            // Defeat current enemy
            Enemy& enemy = battleService->GetEnemy();
            EventBus::Publish(DeathEvent(&enemy));

            const int newIndex = startIndex + 1;
            RC_ASSERT(battleService->GetCurrentEnemyIndex() == newIndex);

            const Enemy& nextEnemy = battleService->GetEnemy();
            RC_ASSERT(nextEnemy.GetHealthComponent().GetHealth() == seqCopy[newIndex].hp);
            RC_ASSERT(nextEnemy.GetName() == seqCopy[newIndex].name);
        });

    EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Property 7: After StartStage, Player HP and MP equal constructor defaults
// Validates: Requirements 6.1
// ---------------------------------------------------------------------------
// Feature: sequential-enemy-battles, Property 7: StartStage_ResetsPlayerState
TEST(BattleSequenceProperties, StartStage_ResetsPlayerState)
{
    _putenv_s("RC_PARAMS", "max_success=25");

    auto result = rc::check(
        "After StartStage, player HP and MP equal Player constructor defaults",
        []()
        {
            std::vector<EnemyConfig> pool = { MakeEnemyConfig(0) };

            PoolConfigService configService(pool);
            auto cardService      = CreateCardService(configService);
            StubSceneService sceneService;

            auto battleService = CreateBattleService(
                configService, *cardService, sceneService);

            battleService->StartStage();

            const Player& player = battleService->GetPlayer();

            // Player() constructor sets maxHp=10 (HealthComponent default) in release
            // and mp=0, maxMp=10. In _DEBUG build mp/maxMp=100.
            // We only check: HP == maxHP (full health) and MP == maxMP (full mp).
            RC_ASSERT(player.GetHealthComponent().GetHealth() ==
                      player.GetHealthComponent().GetMaxHealth());
            RC_ASSERT(player.GetMp() == player.GetMaxMp());
        });

    EXPECT_TRUE(result);
}

// ---------------------------------------------------------------------------
// Property 8: After StartStage, hand/discard are empty, draw pile == initial deck
// Validates: Requirements 6.2
// ---------------------------------------------------------------------------
// Feature: sequential-enemy-battles, Property 8: StartStage_ResetsDeck
TEST(BattleSequenceProperties, StartStage_ResetsDeck)
{
    _putenv_s("RC_PARAMS", "max_success=25");

    auto result = rc::check(
        "After StartStage, hand and discard are empty, draw pile equals the full initial deck",
        []()
        {
            // Build a card deck with a few cards
            std::vector<CardConfig> cards = {
                {0, 2}, {1, 3}, {2, 2}, {0, 3}, {3, 2}
            };
            std::vector<EnemyConfig> pool = { MakeEnemyConfig(0) };

            PoolConfigService configService(pool, cards);
            auto cardService      = CreateCardService(configService);
            StubSceneService sceneService;

            auto battleService = CreateBattleService(
                configService, *cardService, sceneService);

            // Pollute the deck state before StartStage by drawing some cards
            cardService->Start();
            cardService->DrawCard();
            cardService->DrawCard();

            // Now reset via StartStage
            battleService->StartStage();

            RC_ASSERT(cardService->GetHandCards().empty());
            RC_ASSERT(cardService->GetDiscardCards().empty());
            // Draw pile should contain all cards (= deck size)
            RC_ASSERT(cardService->GetDrawCards().size() == cards.size());
        });

    EXPECT_TRUE(result);
}

} // namespace
} // namespace mc
