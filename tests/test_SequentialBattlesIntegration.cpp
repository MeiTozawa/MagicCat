// End-to-end integration tests for the sequential enemy battles feature.
// Uses REAL implementations of BattleService, CardService,
// and SceneService — no stubs for the core services under test.
//
// Validates: Requirements 3.1, 3.2, 4.1, 4.2, 1.1

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
// Stub config service — supplies a fixed pool of 3 EnemyConfigs + CardConfigs
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

// ---------------------------------------------------------------------------
// Minimal DummyScene so scene transitions don't crash when Start() is called
// ---------------------------------------------------------------------------

class DummyScene : public IScene
{
public:
    int startCount = 0;
    void Start()  override { ++startCount; }
    void Update(float) override {}
};

// ---------------------------------------------------------------------------
// Helper: minimal Character subclass that carries a specific tag
// Used to synthesise DeathEvent with the correct tag
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
// Integration test fixture
//
// Wiring:
//   MockConfigService (provides 3 EnemyConfigs + CardConfigs)
//   → CreateCardService
//   → CreateSceneService          (real — subscribes to StageClearEvent/StageFailEvent)
//   → CreateBattleService (real — owns characters, subscribes to DeathEvent,
//                                  publishes stage events)
//
// DummyScene instances are registered for Info and Combat so that PushScene /
// SetCurrentScene don't assert or crash.
// ---------------------------------------------------------------------------

class SequentialBattlesIntegrationTest : public ::testing::Test
{
protected:
    StubConfigService                          configService;

    std::unique_ptr<ICardService>              cardService;
    std::unique_ptr<ISceneService>             sceneService;
    std::unique_ptr<IBattleService>    BattleService;

    DummyScene* infoDummy   = nullptr;
    DummyScene* combatDummy = nullptr;

    void SetUp() override
    {
        cardService      = CreateCardService(configService);
        sceneService     = CreateSceneService();

        // Register real-ish dummy scenes so transitions succeed
        auto infoScene   = std::make_unique<DummyScene>();
        auto combatScene = std::make_unique<DummyScene>();
        infoDummy   = infoScene.get();
        combatDummy = combatScene.get();
        sceneService->RegisterScene(ESceneState::Info,   std::move(infoScene));
        sceneService->RegisterScene(ESceneState::Combat, std::move(combatScene));

        BattleService = CreateBattleService(
            configService, *cardService, *sceneService);
    }

    void TearDown() override
    {
        // Destroy BattleService first to unsubscribe DeathEvent listener
        // before the other services are torn down.
        BattleService.reset();
        sceneService.reset();
        cardService.reset();
    }

    // Convenience: publish a DeathEvent as if the given character died
    static void PublishDeath(Character& victim)
    {
        EventBus::Publish(DeathEvent(&victim));
    }
};

// ---------------------------------------------------------------------------
// Integration Test 1 — Three-kill win flow (Requirement 3.1, 4.1, 3.2)
//
// StartStage()
//   → PushScene(Combat)
// Kill enemy 0  → EnemyDefeatedEvent, LoadEnemy(seq[1]), currentIndex == 1
// Kill enemy 1  → EnemyDefeatedEvent, LoadEnemy(seq[2]), currentIndex == 2
// Kill enemy 2  → StageClearEvent published
//               → SceneService reacts: SetCurrentScene(Info), calls Info::Start()
// ---------------------------------------------------------------------------

TEST_F(SequentialBattlesIntegrationTest, ThreeKillWin_StageClearAndSceneTransition)
{
    // --- Requirement 1.1: StartStage produces a Sequence of size 3 ---
    BattleService->StartStage();
    ASSERT_EQ(BattleService->GetSequence().size(), 3u)
        << "Requirement 1.1: sequence must have exactly 3 entries after StartStage()";

    // After StartStage the scene should have been pushed to Combat
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    bool stageClearReceived = false;
    auto handle = EventBus::Subscribe<StageClearEvent>(
        [&](const StageClearEvent&) { stageClearReceived = true; });

    TaggedCharacter enemy(ETag::Enemy);

    // Kill enemy at index 0 — should advance to 1, NOT fire StageClearEvent
    PublishDeath(enemy);
    EXPECT_FALSE(stageClearReceived);
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 1);

    // Kill enemy at index 1 — should advance to 2, NOT fire StageClearEvent
    PublishDeath(enemy);
    EXPECT_FALSE(stageClearReceived);
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 2);

    // Kill enemy at index 2 — must fire StageClearEvent (Requirement 3.1)
    // and SceneService must transition back to Info (Requirement 3.2)
    int infoStartBefore = infoDummy->startCount;
    PublishDeath(enemy);

    EXPECT_TRUE(stageClearReceived) << "Requirement 3.1: StageClearEvent must fire after 3rd enemy death";
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info)
        << "Requirement 3.2: SceneService must switch to Info on StageClearEvent";
    EXPECT_GT(infoDummy->startCount, infoStartBefore)
        << "Requirement 3.2: InfoScene::Start() must be called after StageClearEvent";

    // Internal state must be reset (Requirement 4.1 / cleanup)
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(BattleService->GetSequence().empty());

    EventBus::Unsubscribe(handle);
}

// ---------------------------------------------------------------------------
// Integration Test 2 — Player death flow (Requirement 4.1, 4.2)
//
// StartStage()
// Player dies mid-battle → StageFailEvent published
//                        → SceneService reacts: SetCurrentScene(Info), Info::Start()
// ---------------------------------------------------------------------------

TEST_F(SequentialBattlesIntegrationTest, PlayerDeath_StageFailAndSceneTransition)
{
    BattleService->StartStage();

    // Confirm we're in Combat after start
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    bool stageFailReceived = false;
    auto handle = EventBus::Subscribe<StageFailEvent>(
        [&](const StageFailEvent&) { stageFailReceived = true; });

    int infoStartBefore = infoDummy->startCount;

    TaggedCharacter player(ETag::Player);
    PublishDeath(player);

    EXPECT_TRUE(stageFailReceived)
        << "Requirement 4.1: StageFailEvent must fire when player dies";
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info)
        << "Requirement 4.2: SceneService must switch to Info on StageFailEvent";
    EXPECT_GT(infoDummy->startCount, infoStartBefore)
        << "Requirement 4.2: InfoScene::Start() must be called after StageFailEvent";

    // Internal state must be reset
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(BattleService->GetSequence().empty());

    EventBus::Unsubscribe(handle);
}

// ---------------------------------------------------------------------------
// Integration Test 3 — Consecutive stages (Requirement 1.1)
//
// Stage 1: StartStage() → kill all 3 enemies → StageClearEvent → state reset
// Stage 2: StartStage() again → new Sequence of size 3 must be generated
//          verify currentIndex is back to 0 and sequence has 3 entries
// ---------------------------------------------------------------------------

TEST_F(SequentialBattlesIntegrationTest, ConsecutiveStages_NewSequenceGeneratedAfterClear)
{
    // === Stage 1 ===
    BattleService->StartStage();
    ASSERT_EQ(BattleService->GetSequence().size(), 3u);

    // Capture first sequence for later comparison
    const std::vector<EnemyConfig> firstSequence = BattleService->GetSequence();

    TaggedCharacter enemy(ETag::Enemy);
    // Kill all three enemies to clear the stage
    PublishDeath(enemy); // index 0 → 1
    PublishDeath(enemy); // index 1 → 2
    PublishDeath(enemy); // index 2 → StageClearEvent

    // State should be fully reset after stage clear
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(BattleService->GetSequence().empty());

    // === Stage 2 ===
    BattleService->StartStage();

    // Requirement 1.1: new sequence must also have 3 entries
    EXPECT_EQ(BattleService->GetSequence().size(), 3u)
        << "Requirement 1.1: second StartStage() must generate a new Sequence of size 3";

    // Index must be reset to 0
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0)
        << "Requirement 1.1: currentIndex must be 0 at start of second stage";

    // Scene must be back in Combat
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat)
        << "StartStage() must push to Combat scene for second stage";

    // The new sequence is valid (size 3) — content may differ since the pool
    // only has 3 entries so shuffling may produce the same order, but state is fresh
    EXPECT_EQ(BattleService->GetSequence().size(), 3u);
}

// ---------------------------------------------------------------------------
// Integration Test 4 — Player death mid-battle after 1 enemy kill (Requirement 4.1)
//
// StartStage() → kill enemy 0 (index advances to 1) → player dies
// Must still fire StageFailEvent and transition to Info
// ---------------------------------------------------------------------------

TEST_F(SequentialBattlesIntegrationTest, PlayerDeathAfterPartialProgress_StageFailEvent)
{
    BattleService->StartStage();

    // Kill first enemy to advance index
    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 1);

    bool stageFailReceived = false;
    auto handle = EventBus::Subscribe<StageFailEvent>(
        [&](const StageFailEvent&) { stageFailReceived = true; });

    // Player now dies
    TaggedCharacter player(ETag::Player);
    PublishDeath(player);

    EXPECT_TRUE(stageFailReceived)
        << "Requirement 4.1: StageFailEvent must fire even when player dies mid-stage";
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info)
        << "Requirement 4.2: scene must revert to Info after player death mid-stage";
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(BattleService->GetSequence().empty());

    EventBus::Unsubscribe(handle);
}

} // namespace
} // namespace mc
