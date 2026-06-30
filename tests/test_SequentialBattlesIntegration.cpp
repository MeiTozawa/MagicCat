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

    DummyScene* infoDummy    = nullptr;
    DummyScene* combatDummy  = nullptr;
    DummyScene* cutsceneDummy = nullptr;

    void SetUp() override
    {
        cardService      = CreateCardService(configService);
        sceneService     = CreateSceneService();

        // Register real-ish dummy scenes so transitions succeed
        auto infoScene     = std::make_unique<DummyScene>();
        auto combatScene   = std::make_unique<DummyScene>();
        auto cutsceneScene = std::make_unique<DummyScene>();
        infoDummy     = infoScene.get();
        combatDummy   = combatScene.get();
        cutsceneDummy = cutsceneScene.get();
        sceneService->RegisterScene(ESceneState::Info,     std::move(infoScene));
        sceneService->RegisterScene(ESceneState::Combat,   std::move(combatScene));
        sceneService->RegisterScene(ESceneState::Cutscene, std::move(cutsceneScene));

        BattleService = CreateBattleService(
            configService, *cardService);
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
//   → Cutscene_Scene (new routing: StageStartedEvent → Cutscene, not Combat)
// CutsceneFinishedEvent → Combat
// Kill enemy 0  → EnemyDefeatedEvent → Cutscene, LoadEnemy(seq[1]), currentIndex == 1
// CutsceneFinishedEvent → Combat
// Kill enemy 1  → EnemyDefeatedEvent → Cutscene, LoadEnemy(seq[2]), currentIndex == 2
// CutsceneFinishedEvent → Combat
// Kill enemy 2  → StageClearEvent published
//               → SceneService reacts: SetCurrentScene(Info), calls Info::Start()
// ---------------------------------------------------------------------------

TEST_F(SequentialBattlesIntegrationTest, ThreeKillWin_StageClearAndSceneTransition)
{
    // --- Requirement 1.1: StartStage produces a Sequence of size 3 ---
    BattleService->StartStage();
    ASSERT_EQ(BattleService->GetSequence().size(), 3u)
        << "Requirement 1.1: sequence must have exactly 3 entries after StartStage()";

    // After StartStage the scene should have been pushed to Cutscene (new routing)
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene)
        << "Requirement 2.1: StartStage must route to Cutscene first";

    // Simulate cutscene finishing → transitions to Combat
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat)
        << "After CutsceneFinishedEvent, scene must be Combat";

    bool stageClearReceived = false;
    auto handle = EventBus::Subscribe<StageClearEvent>(
        [&](const StageClearEvent&) { stageClearReceived = true; });

    TaggedCharacter enemy(ETag::Enemy);

    // Kill enemy at index 0 — should advance to 1, show Cutscene, NOT fire StageClearEvent
    PublishDeath(enemy);
    EXPECT_FALSE(stageClearReceived);
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 1);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene)
        << "Requirement 2.2: after enemy defeat (index 0→1), must route to Cutscene";

    // Simulate cutscene → Combat for second enemy
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    // Kill enemy at index 1 — should advance to 2, show Cutscene, NOT fire StageClearEvent
    PublishDeath(enemy);
    EXPECT_FALSE(stageClearReceived);
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 2);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene)
        << "Requirement 2.2: after enemy defeat (index 1→2), must route to Cutscene";

    // Simulate cutscene → Combat for third enemy
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    // Kill enemy at index 2 — must fire StageClearEvent (Requirement 3.1)
    // and SceneService must transition back to Info (Requirement 3.2)
    // NO Cutscene for the final enemy (Requirement 2.8)
    int infoStartBefore = infoDummy->startCount;
    int cutsceneStartBefore = cutsceneDummy->startCount;
    PublishDeath(enemy);

    EXPECT_TRUE(stageClearReceived) << "Requirement 3.1: StageClearEvent must fire after 3rd enemy death";
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info)
        << "Requirement 3.2: SceneService must switch to Info on StageClearEvent";
    EXPECT_GT(infoDummy->startCount, infoStartBefore)
        << "Requirement 3.2: InfoScene::Start() must be called after StageClearEvent";
    EXPECT_EQ(cutsceneDummy->startCount, cutsceneStartBefore)
        << "Requirement 2.8: Cutscene must NOT appear after the final enemy defeat";

    // Internal state must be reset (Requirement 4.1 / cleanup)
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(BattleService->GetSequence().empty());

    EventBus::Unsubscribe(handle);
}

// ---------------------------------------------------------------------------
// Integration Test 2 — Player death flow (Requirement 4.1, 4.2)
//
// StartStage()
// → Cutscene → CutsceneFinishedEvent → Combat
// Player dies mid-battle → StageFailEvent published
//                        → SceneService reacts: SetCurrentScene(Info), Info::Start()
// ---------------------------------------------------------------------------

TEST_F(SequentialBattlesIntegrationTest, PlayerDeath_StageFailAndSceneTransition)
{
    BattleService->StartStage();

    // Confirm we're in Cutscene after start (new routing)
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene);

    // Advance to Combat via CutsceneFinishedEvent
    EventBus::Publish(CutsceneFinishedEvent{});
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
// Stage 1: StartStage() → Cutscene → Combat → kill all 3 enemies → StageClearEvent → state reset
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

    // Advance to Combat via cutscene
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    TaggedCharacter enemy(ETag::Enemy);

    // Kill enemy 0 → Cutscene → Combat
    PublishDeath(enemy);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene);
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    // Kill enemy 1 → Cutscene → Combat
    PublishDeath(enemy);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene);
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    // Kill enemy 2 → StageClearEvent → Info (no Cutscene)
    PublishDeath(enemy);

    // State should be fully reset after stage clear
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(BattleService->GetSequence().empty());
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info);

    // === Stage 2 ===
    BattleService->StartStage();

    // Requirement 1.1: new sequence must also have 3 entries
    EXPECT_EQ(BattleService->GetSequence().size(), 3u)
        << "Requirement 1.1: second StartStage() must generate a new Sequence of size 3";

    // Index must be reset to 0
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0)
        << "Requirement 1.1: currentIndex must be 0 at start of second stage";

    // Scene must be Cutscene (new routing for StartStage)
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene)
        << "StartStage() must route to Cutscene scene for second stage";

    // The new sequence is valid (size 3)
    EXPECT_EQ(BattleService->GetSequence().size(), 3u);
}

// ---------------------------------------------------------------------------
// Integration Test 4 — Player death mid-battle after 1 enemy kill (Requirement 4.1)
//
// StartStage() → Cutscene → CutsceneFinishedEvent → Combat
// → kill enemy 0 (index advances to 1, Cutscene shown) → CutsceneFinishedEvent → Combat
// → player dies → must still fire StageFailEvent and transition to Info
// ---------------------------------------------------------------------------

TEST_F(SequentialBattlesIntegrationTest, PlayerDeathAfterPartialProgress_StageFailEvent)
{
    BattleService->StartStage();

    // Advance to Combat via cutscene
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    // Kill first enemy to advance index → Cutscene for next enemy
    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 1);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene);

    // Advance to Combat for second enemy via cutscene
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

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
