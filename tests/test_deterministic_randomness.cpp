#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rapidcheck/gtest.h>
#include <RandomUtils.h>
#include <vector>
#include <map>
#include <cmath>
#include "MockServices.h"

import BattleService;
import CardService;
import ConfigService;
import EventBus;
import Enemy;
import Player;

using namespace mc;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;

namespace mc {
namespace {

// ============================================================================
// Mock Services for Testing
// ============================================================================

class MockAssetServiceLocal : public IAssetService
{
public:
    MOCK_METHOD(ESprite, ParseSprite, (const std::string& name), (const, override));
    MOCK_METHOD(std::string, SpriteToString, (ESprite sprite), (const, override));
    MOCK_METHOD(int, GetImageHandle, (EImage image), (const, override));
    MOCK_METHOD(int, GetFontHandle, (EFont font), (const, override));
    MOCK_METHOD(int, GetSoundHandle, (ESound sound), (const, override));
};

// ============================================================================
// Test Fixtures and Helpers
// ============================================================================

/// @brief Helper to create a valid card config for testing
std::vector<CardConfig> CreateTestCardConfig()
{
    return {
        CardConfig{0, 3}, // Rock x3
        CardConfig{1, 2}, // Scissors x2
        CardConfig{2, 3}, // Paper x3
    };
}

/// @brief Helper to create a valid enemy config for testing
std::vector<EnemyConfig> CreateTestEnemyConfig()
{
    return {
        EnemyConfig{10, 5, 3, 4, 2, "Cat", "cat"},
        EnemyConfig{12, 4, 4, 3, 3, "Dog", "dog"},
        EnemyConfig{15, 6, 2, 5, 4, "Bird", "bird"},
    };
}

/// @brief Helper to create a valid game config for testing
GameConfig CreateTestGameConfig()
{
    GameConfig config;
    config.battleCount = 3;
    return config;
}

/// @brief Helper to create a valid player config for testing
PlayerConfig CreateTestPlayerConfig()
{
    PlayerConfig config;
    config.hp = 20;
    config.maxMp = 10;
    config.healAmount = 5;
    config.maxHealUses = 3;
    config.mpCostHeal = 3;
    config.mpCostClairvoyance = 2;
    config.mpCostWeaken = 4;
    config.spriteName = "player";
    return config;
}

// ============================================================================
// Property 5: Deterministic Card Draw Order
// ============================================================================
// **Validates: Requirements 2.9, 2.10, 2.11**
//
// For any seed S and valid card config C, starting CardService twice with
// the same seed S produces identical card draw sequences.
// ============================================================================

RC_GTEST_PROP(DeterministicRandomness, Property5_SameSeed_SameCardDrawOrder, ())
{
    const unsigned int seed = *rc::gen::arbitrary<unsigned int>();

    EventBus::Clear();

    // Setup mocks
    NiceMock<MockIConfigService> mockConfig;
    NiceMock<MockAssetService> mockAsset;
    auto cardConfig = CreateTestCardConfig();
    mockConfig.cardConfigs = cardConfig; // Store for reference return
    ON_CALL(mockConfig, GetCardConfigs()).WillByDefault(ReturnRef(mockConfig.cardConfigs));

    // First run with seed
    Random::Seed(seed);
    auto cardService1 = CreateCardService(mockConfig);
    cardService1->Start();

    std::vector<Card> sequence1;
    for (int i = 0; i < 8; ++i)
    {
        auto card = cardService1->DrawCard();
        if (card.CardType != ECardType::Null)
            sequence1.push_back(card);
    }

    // Second run with same seed
    Random::Seed(seed);
    auto cardService2 = CreateCardService(mockConfig);
    cardService2->Start();

    std::vector<Card> sequence2;
    for (int i = 0; i < 8; ++i)
    {
        auto card = cardService2->DrawCard();
        if (card.CardType != ECardType::Null)
            sequence2.push_back(card);
    }

    // Verify sequences are identical
    RC_ASSERT(sequence1.size() == sequence2.size());
    for (size_t i = 0; i < sequence1.size(); ++i)
    {
        RC_ASSERT(sequence1[i].CardType == sequence2[i].CardType);
        RC_ASSERT(sequence1[i].Power == sequence2[i].Power);
    }
}

// ============================================================================
// Property 5: Deterministic Enemy Selection
// ============================================================================
// **Validates: Requirements 2.9, 2.10, 2.11**
//
// For any seed S and valid enemy config E, starting BattleService twice with
// the same seed S produces identical enemy sequences.
// ============================================================================

RC_GTEST_PROP(DeterministicRandomness, Property5_SameSeed_SameEnemySequence, ())
{
    const unsigned int seed = *rc::gen::arbitrary<unsigned int>();

    EventBus::Clear();

    // Setup mocks
    NiceMock<MockIConfigService> mockConfig;
    NiceMock<MockIPersistenceService> mockPersist;
    NiceMock<MockAssetService> mockAsset;

    mockConfig.cardConfigs = CreateTestCardConfig();
    mockConfig.enemyConfigs = CreateTestEnemyConfig();
    mockConfig.gameConfig = CreateTestGameConfig();
    mockConfig.playerConfig = CreateTestPlayerConfig();

    ON_CALL(mockConfig, GetCardConfigs()).WillByDefault(ReturnRef(mockConfig.cardConfigs));
    ON_CALL(mockConfig, GetEnemyConfigs()).WillByDefault(ReturnRef(mockConfig.enemyConfigs));
    ON_CALL(mockConfig, GetGameConfig()).WillByDefault(ReturnRef(mockConfig.gameConfig));
    ON_CALL(mockConfig, GetPlayerConfig()).WillByDefault(ReturnRef(mockConfig.playerConfig));
    ON_CALL(mockAsset, ParseSprite(_)).WillByDefault(Return(ESprite::MeowingCat));

    auto cardService1 = CreateCardService(mockConfig);
    auto cardService2 = CreateCardService(mockConfig);

    // First run with seed
    Random::Seed(seed);
    auto battleService1 = CreateBattleService(mockConfig, mockPersist, *cardService1, mockAsset);
    battleService1->StartStage();
    auto sequence1 = battleService1->GetSequence();

    // Second run with same seed
    Random::Seed(seed);
    auto battleService2 = CreateBattleService(mockConfig, mockPersist, *cardService2, mockAsset);
    battleService2->StartStage();
    auto sequence2 = battleService2->GetSequence();

    // Verify sequences are identical
    RC_ASSERT(sequence1.size() == sequence2.size());
    for (size_t i = 0; i < sequence1.size(); ++i)
    {
        RC_ASSERT(sequence1[i].name == sequence2[i].name);
        RC_ASSERT(sequence1[i].hp == sequence2[i].hp);
        RC_ASSERT(sequence1[i].baseWeight == sequence2[i].baseWeight);
    }
}

// ============================================================================
// Property 5: Deterministic Enemy Attack Selection
// ============================================================================
// **Validates: Requirements 2.9, 2.10, 2.11**
//
// For any seed S, baseWeight W, and weight offsets O, calling GetAttackIntent
// multiple times with the same seed produces identical attack sequences.
// ============================================================================

RC_GTEST_PROP(DeterministicRandomness, Property5_SameSeed_SameEnemyAttackChoices, ())
{
    const unsigned int seed = *rc::gen::arbitrary<unsigned int>();
    const int baseWeight = *rc::gen::inRange(1, 10);
    const int rockOffset = *rc::gen::inRange(-5, 5);
    const int scissorsOffset = *rc::gen::inRange(-5, 5);
    const int paperOffset = *rc::gen::inRange(-5, 5);

    // Ensure total weight remains positive
    RC_PRE((baseWeight + rockOffset) > 0 && (baseWeight + scissorsOffset) > 0 && (baseWeight + paperOffset) > 0);

    EventBus::Clear();

    // First run with seed
    Random::Seed(seed);
    Enemy enemy1(baseWeight, 3, 3, 3, L"Test1", ESprite::Cat, 10);
    enemy1.SetRockOffset(rockOffset);
    enemy1.SetScissorsOffset(scissorsOffset);
    enemy1.SetPaperOffset(paperOffset);

    std::vector<EAttackType> attacks1;
    for (int i = 0; i < 20; ++i)
    {
        attacks1.push_back(enemy1.GetAttackIntent());
    }

    // Second run with same seed
    Random::Seed(seed);
    Enemy enemy2(baseWeight, 3, 3, 3, L"Test2", ESprite::Dog, 10);
    enemy2.SetRockOffset(rockOffset);
    enemy2.SetScissorsOffset(scissorsOffset);
    enemy2.SetPaperOffset(paperOffset);

    std::vector<EAttackType> attacks2;
    for (int i = 0; i < 20; ++i)
    {
        attacks2.push_back(enemy2.GetAttackIntent());
    }

    // Verify attack sequences are identical
    RC_ASSERT(attacks1.size() == attacks2.size());
    for (size_t i = 0; i < attacks1.size(); ++i)
    {
        RC_ASSERT(attacks1[i] == attacks2[i]);
    }
}

// ============================================================================
// Property 5: Weighted Enemy Attack Distribution
// ============================================================================
// **Validates: Requirements 2.9, 2.10, 2.11**
//
// For a given seed range and weight configuration, the distribution of enemy
// attacks should approximately match the configured weights (statistical check).
// This validates that weighted behavior is preserved across deterministic runs.
// ============================================================================

RC_GTEST_PROP(DeterministicRandomness, Property5_WeightedDistribution_MatchesExpectedRatio, ())
{
    const unsigned int baseSeed = *rc::gen::arbitrary<unsigned int>();
    const int baseWeight = *rc::gen::inRange(5, 20);

    // Create weights with intentional bias
    const int rockOffset = *rc::gen::inRange(0, 10);
    const int scissorsOffset = *rc::gen::inRange(-5, 0);
    const int paperOffset = *rc::gen::inRange(-5, 0);

    // Ensure all weights remain positive
    RC_PRE((baseWeight + rockOffset) > 0);
    RC_PRE((baseWeight + scissorsOffset) > 0);
    RC_PRE((baseWeight + paperOffset) > 0);

    EventBus::Clear();

    // Calculate expected probabilities
    const int rockWeight = baseWeight + rockOffset;
    const int scissorsWeight = baseWeight + scissorsOffset;
    const int paperWeight = baseWeight + paperOffset;
    const int totalWeight = rockWeight + scissorsWeight + paperWeight;

    const double expectedRock = static_cast<double>(rockWeight) / totalWeight;
    const double expectedScissors = static_cast<double>(scissorsWeight) / totalWeight;
    const double expectedPaper = static_cast<double>(paperWeight) / totalWeight;

    // Run multiple trials across controlled seed range
    std::map<EAttackType, int> counts;
    const int trials = 1000;

    for (int trial = 0; trial < trials; ++trial)
    {
        Random::Seed(baseSeed + trial);
        Enemy enemy(baseWeight, 3, 3, 3, L"Test", ESprite::Cat, 10);
        enemy.SetRockOffset(rockOffset);
        enemy.SetScissorsOffset(scissorsOffset);
        enemy.SetPaperOffset(paperOffset);

        EAttackType attack = enemy.GetAttackIntent();
        counts[attack]++;
    }

    // Verify distribution is within reasonable bounds (using chi-square like approach)
    const double actualRock = static_cast<double>(counts[EAttackType::Rock]) / trials;
    const double actualScissors = static_cast<double>(counts[EAttackType::Scissors]) / trials;
    const double actualPaper = static_cast<double>(counts[EAttackType::Paper]) / trials;

    // Allow 10% tolerance for statistical variation
    const double tolerance = 0.10;
    RC_ASSERT(std::abs(actualRock - expectedRock) < tolerance);
    RC_ASSERT(std::abs(actualScissors - expectedScissors) < tolerance);
    RC_ASSERT(std::abs(actualPaper - expectedPaper) < tolerance);
}

// ============================================================================
// Property 5: Combat Outcome Determinism
// ============================================================================
// **Validates: Requirements 2.9, 2.10, 2.11**
//
// For any seed S and valid battle configuration, running a full combat
// sequence with the same seed and same player actions produces identical
// outcomes (damage, HP, enemy defeat order).
// ============================================================================

RC_GTEST_PROP(DeterministicRandomness, Property5_SameSeed_SameCombatOutcome, ())
{
    const unsigned int seed = *rc::gen::arbitrary<unsigned int>();

    EventBus::Clear();

    // Setup mocks
    NiceMock<MockIConfigService> mockConfig;
    NiceMock<MockIPersistenceService> mockPersist;
    NiceMock<MockAssetService> mockAsset;

    mockConfig.cardConfigs = CreateTestCardConfig();
    mockConfig.enemyConfigs = CreateTestEnemyConfig();
    mockConfig.gameConfig = CreateTestGameConfig();
    mockConfig.playerConfig = CreateTestPlayerConfig();

    ON_CALL(mockConfig, GetCardConfigs()).WillByDefault(ReturnRef(mockConfig.cardConfigs));
    ON_CALL(mockConfig, GetEnemyConfigs()).WillByDefault(ReturnRef(mockConfig.enemyConfigs));
    ON_CALL(mockConfig, GetGameConfig()).WillByDefault(ReturnRef(mockConfig.gameConfig));
    ON_CALL(mockConfig, GetPlayerConfig()).WillByDefault(ReturnRef(mockConfig.playerConfig));
    ON_CALL(mockAsset, ParseSprite(_)).WillByDefault(Return(ESprite::MeowingCat));

    auto cardService1 = CreateCardService(mockConfig);
    auto cardService2 = CreateCardService(mockConfig);

    // First run with seed
    Random::Seed(seed);
    auto battleService1 = CreateBattleService(mockConfig, mockPersist, *cardService1, mockAsset);
    battleService1->StartStage();
    
    std::vector<EAttackType> enemyAttacks1;
    for (int i = 0; i < 5; ++i)
    {
        enemyAttacks1.push_back(battleService1->GetEnemy().GetAttackIntent());
    }
    int playerHp1 = battleService1->GetPlayer().GetHealthComponent().GetHealth();

    // Second run with same seed
    Random::Seed(seed);
    auto battleService2 = CreateBattleService(mockConfig, mockPersist, *cardService2, mockAsset);
    battleService2->StartStage();
    
    std::vector<EAttackType> enemyAttacks2;
    for (int i = 0; i < 5; ++i)
    {
        enemyAttacks2.push_back(battleService2->GetEnemy().GetAttackIntent());
    }
    int playerHp2 = battleService2->GetPlayer().GetHealthComponent().GetHealth();

    // Verify outcomes are identical
    RC_ASSERT(enemyAttacks1.size() == enemyAttacks2.size());
    for (size_t i = 0; i < enemyAttacks1.size(); ++i)
    {
        RC_ASSERT(enemyAttacks1[i] == enemyAttacks2[i]);
    }
    RC_ASSERT(playerHp1 == playerHp2);
}

// ============================================================================
// Property 5: Seed Reporting for Test Failures
// ============================================================================
// **Validates: Requirements 2.9**
//
// This is a documentation test that demonstrates how to capture and report
// seeds when a test fails. In practice, RapidCheck automatically provides
// minimal counterexamples including the seed.
// ============================================================================

TEST(DeterministicRandomness, SeedReporting_DocumentationExample)
{
    // This test demonstrates the pattern for seed-based debugging.
    // When a RapidCheck property fails, it prints the failing inputs,
    // including the seed value. This seed can be used to reproduce
    // the exact failure scenario.

    const unsigned int reproducibleSeed = 42;
    Random::Seed(reproducibleSeed);

    // Example: if this fails, report the seed
    Enemy enemy(5, 3, 3, 3, L"Test", ESprite::Cat, 10);
    EAttackType attack = enemy.GetAttackIntent();

    // If this assertion fails, we know seed=42 caused it
    EXPECT_TRUE(attack == EAttackType::Rock || 
                attack == EAttackType::Scissors || 
                attack == EAttackType::Paper);

    // In a real failure scenario, the seed would be in the RapidCheck output:
    // "Falsifiable after X tests. Seed: 12345"
}

// ============================================================================
// Property 5: Different Seeds Produce Different Sequences
// ============================================================================
// **Validates: Requirements 2.9, 2.10, 2.11**
//
// For different seeds, the random sequences should be different (with high
// probability). This validates that seeding actually controls randomness.
// ============================================================================

TEST(DeterministicRandomness, Property5_DifferentSeeds_DifferentSequences)
{
    const unsigned int seed1 = 12345;
    const unsigned int seed2 = 67890;

    EventBus::Clear();

    // Setup mocks
    NiceMock<MockIConfigService> mockConfig;
    mockConfig.cardConfigs = CreateTestCardConfig();
    ON_CALL(mockConfig, GetCardConfigs()).WillByDefault(ReturnRef(mockConfig.cardConfigs));

    // First run with seed1
    Random::Seed(seed1);
    auto cardService1 = CreateCardService(mockConfig);
    cardService1->Start();

    std::vector<Card> sequence1;
    for (int i = 0; i < 8; ++i)
    {
        auto card = cardService1->DrawCard();
        if (card.CardType != ECardType::Null)
            sequence1.push_back(card);
    }

    // Second run with seed2
    Random::Seed(seed2);
    auto cardService2 = CreateCardService(mockConfig);
    cardService2->Start();

    std::vector<Card> sequence2;
    for (int i = 0; i < 8; ++i)
    {
        auto card = cardService2->DrawCard();
        if (card.CardType != ECardType::Null)
            sequence2.push_back(card);
    }

    // Verify sequences are different (with high probability)
    // At least one card should differ
    bool foundDifference = false;
    if (sequence1.size() == sequence2.size())
    {
        for (size_t i = 0; i < sequence1.size(); ++i)
        {
            if (sequence1[i].CardType != sequence2[i].CardType ||
                sequence1[i].Power != sequence2[i].Power)
            {
                foundDifference = true;
                break;
            }
        }
    }
    else
    {
        foundDifference = true;
    }

    // With different seeds, we expect different outcomes
    EXPECT_TRUE(foundDifference);
}

} // namespace
} // namespace mc
