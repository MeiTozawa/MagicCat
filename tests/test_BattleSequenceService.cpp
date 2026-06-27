// Unit tests for BattleService (concrete examples)
// Validates: Requirements 3.1, 3.3, 4.1, 4.3

// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before gtest includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>

import BattleService;
import CardService;
import SceneService;
import ConfigService;
import EventBus;
import Character;
import Enemy;
import Player;
import HealthComponent;

namespace mc {
namespace {

// ---------------------------------------------------------------------------
// Stub implementations
// ---------------------------------------------------------------------------

class StubConfigService : public IConfigService
{
    std::vector<EnemyConfig> configs_;
public:
    explicit StubConfigService(std::vector<EnemyConfig> configs = {
        EnemyConfig{ 10, 1, 1, 1, 1, L"EnemyA", "Bunny" },
        EnemyConfig{ 10, 1, 1, 1, 1, L"EnemyB", "Bunny" },
        EnemyConfig{ 10, 1, 1, 1, 1, L"EnemyC", "Bunny" },
    })
        : configs_(std::move(configs)) {}

    const std::vector<CardConfig>& GetCardConfigs() const override
    {
        static const std::vector<CardConfig> empty;
        return empty;
    }

    const std::vector<EnemyConfig>& GetEnemyConfigs() const override
    {
        return configs_;
    }
};

class StubCardService : public ICardService
{
public:
    void Start() override {}
    Card DrawCard() override { return {}; }
    void DiscardHand() override {}
    std::vector<Card> GetHandCards()    override { return {}; }
    std::vector<Card> GetDrawCards()    override { return {}; }
    std::vector<Card> GetDiscardCards() override { return {}; }
};

class StubSceneService : public ISceneService
{
public:
    ESceneState currentScene = ESceneState::Info;
    int pushCount = 0;

    void Update(float) override {}
    void PushScene(ESceneState type) override { currentScene = type; ++pushCount; }
    void PopScene()  override {}
    void RegisterScene(ESceneState, std::unique_ptr<IScene>&&) override {}
    ESceneState GetCurrentScene() override { return currentScene; }
    void SetCurrentScene(ESceneState type) override { currentScene = type; }
};

// ---------------------------------------------------------------------------
// Helper: minimal Character subclass that carries a specific tag
// ---------------------------------------------------------------------------

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
    StubConfigService   configService;
    StubCardService      cardService;
    StubSceneService     sceneService;

    std::unique_ptr<IBattleService> sut;

    void SetUp() override
    {
        sut = CreateBattleService(
            configService, cardService, sceneService);
    }

    void TearDown() override
    {
        // Destroy sut (and its DeathEvent subscription) before locals.
        sut.reset();
    }

    /// Calls StartStage() then simulates exactly `n` enemy deaths in sequence.
    void KillEnemies(int n)
    {
        TaggedCharacter enemy(ETag::Enemy);
        for (int i = 0; i < n; ++i)
        {
            EventBus::Publish(DeathEvent(&enemy));
        }
    }
};

// ---------------------------------------------------------------------------
// Test 1: Enemy at index 2 dies → StageClearEvent is published
// Requirements: 3.1, 4.1
// ---------------------------------------------------------------------------
TEST_F(BattleServiceTest, EnemyDiesAtIndex2_PublishesStageClearEvent)
{
    sut->StartStage(); // index = 0

    bool stageClearReceived = false;
    auto handle = EventBus::Subscribe<StageClearEvent>(
        [&](const StageClearEvent&) { stageClearReceived = true; });

    // Kill enemy 0 → index becomes 1
    // Kill enemy 1 → index becomes 2
    // Kill enemy 2 → StageClearEvent
    KillEnemies(3);

    EXPECT_TRUE(stageClearReceived);

    EventBus::Unsubscribe(handle);
}

// ---------------------------------------------------------------------------
// Test 2: Player dies → StageFailEvent is published
// Requirements: 3.3, 4.3
// ---------------------------------------------------------------------------
TEST_F(BattleServiceTest, PlayerDies_PublishesStageFailEvent)
{
    sut->StartStage(); // index = 0

    bool stageFailReceived = false;
    auto handle = EventBus::Subscribe<StageFailEvent>(
        [&](const StageFailEvent&) { stageFailReceived = true; });

    TaggedCharacter player(ETag::Player);
    EventBus::Publish(DeathEvent(&player));

    EXPECT_TRUE(stageFailReceived);

    EventBus::Unsubscribe(handle);
}

// ---------------------------------------------------------------------------
// Test 3: After StageClearEvent, GetCurrentEnemyIndex() == 0 and GetSequence().empty()
// Requirements: 3.1, 4.1
// ---------------------------------------------------------------------------
TEST_F(BattleServiceTest, AfterStageClear_IndexResetToZeroAndSequenceEmpty)
{
    sut->StartStage();

    // Subscribe to verify the event fires (not strictly needed, but good practice)
    auto handle = EventBus::Subscribe<StageClearEvent>([](const StageClearEvent&) {});

    KillEnemies(3); // triggers StageClearEvent at the 3rd kill

    EXPECT_EQ(sut->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(sut->GetSequence().empty());

    EventBus::Unsubscribe(handle);
}

// ---------------------------------------------------------------------------
// Test 4: After StageFailEvent, GetCurrentEnemyIndex() == 0 and GetSequence().empty()
// Requirements: 3.3, 4.3
// ---------------------------------------------------------------------------
TEST_F(BattleServiceTest, AfterStageFail_IndexResetToZeroAndSequenceEmpty)
{
    sut->StartStage();

    auto handle = EventBus::Subscribe<StageFailEvent>([](const StageFailEvent&) {});

    TaggedCharacter player(ETag::Player);
    EventBus::Publish(DeathEvent(&player));

    EXPECT_EQ(sut->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(sut->GetSequence().empty());

    EventBus::Unsubscribe(handle);
}

// ---------------------------------------------------------------------------
// Test 5: Two enemy deaths (index 0 → 1 → 2) do NOT trigger StageClearEvent;
//         only the third does.
// This verifies the index progression boundary.
// Requirements: 3.1
// ---------------------------------------------------------------------------
TEST_F(BattleServiceTest, TwoEnemyDeaths_DoNotTriggerStageClear)
{
    sut->StartStage();

    int stageClearCount = 0;
    auto handle = EventBus::Subscribe<StageClearEvent>(
        [&](const StageClearEvent&) { ++stageClearCount; });

    KillEnemies(2); // advances index to 2, no StageClear yet

    EXPECT_EQ(stageClearCount, 0);
    EXPECT_EQ(sut->GetCurrentEnemyIndex(), 2);

    EventBus::Unsubscribe(handle);
}

// ---------------------------------------------------------------------------
// Test 6: Player death mid-battle (after 1 enemy kill) still fires StageFailEvent
// Requirements: 3.3, 4.3
// ---------------------------------------------------------------------------
TEST_F(BattleServiceTest, PlayerDeathAfterOneEnemyKill_PublishesStageFailEvent)
{
    sut->StartStage();

    // Kill first enemy to advance index
    KillEnemies(1);
    EXPECT_EQ(sut->GetCurrentEnemyIndex(), 1);

    bool stageFailReceived = false;
    auto handle = EventBus::Subscribe<StageFailEvent>(
        [&](const StageFailEvent&) { stageFailReceived = true; });

    TaggedCharacter player(ETag::Player);
    EventBus::Publish(DeathEvent(&player));

    EXPECT_TRUE(stageFailReceived);
    EXPECT_EQ(sut->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(sut->GetSequence().empty());

    EventBus::Unsubscribe(handle);
}

} // namespace
} // namespace mc
