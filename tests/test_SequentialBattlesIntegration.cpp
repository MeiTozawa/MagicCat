// End-to-end integration tests for the sequential enemy battles feature.
// Uses real implementations of BattleService, CardService, and SceneService.

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
    bool LoadSoundSettings(int&, int&, int&) override { return false; }
    bool SaveSoundSettings(int, int, int) override { return true; }
};

class DummyScene : public IScene
{
public:
    int startCount = 0;
    void Start()  override { ++startCount; }
    void Update(float) override {}
};

// Minimal Character subclass that carries a specific tag — used to synthesise DeathEvents.
class TaggedCharacter : public Character
{
public:
    explicit TaggedCharacter(ETag tag)
    {
        tags.push_back(tag);
    }
    void TakeDamage(int) const override {}
};

class SequentialBattlesIntegrationTest : public ::testing::Test
{
protected:
    StubConfigService                          configService;
    NiceMock<MockAssetService>                 mockAssetService;

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
            configService, *cardService, mockAssetService);
    }

    void TearDown() override
    {
        // Destroy BattleService first to unsubscribe DeathEvent listener
        // before the other services are torn down.
        BattleService.reset();
        sceneService.reset();
        cardService.reset();
    }

    static void PublishDeath(Character& victim)
    {
        EventBus::Publish(DeathEvent(&victim));
    }
};

TEST_F(SequentialBattlesIntegrationTest, ThreeKillWin_StageClearAndSceneTransition)
{
    BattleService->StartStage();
    ASSERT_EQ(BattleService->GetSequence().size(), 3u)
        << "Requirement 1.1: sequence must have exactly 3 entries after StartStage()";

    // StartStage routes to Cutscene first (new routing)
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene)
        << "Requirement 2.1: StartStage must route to Cutscene first";

    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat)
        << "After CutsceneFinishedEvent, scene must be Combat";

    bool stageClearReceived = false;
    auto handle = EventBus::Subscribe<StageClearEvent>(
        [&](const StageClearEvent&) { stageClearReceived = true; });

    TaggedCharacter enemy(ETag::Enemy);

    PublishDeath(enemy);
    EXPECT_FALSE(stageClearReceived);
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 1);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene)
        << "Requirement 2.2: after enemy defeat (index 0→1), must route to Cutscene";

    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    PublishDeath(enemy);
    EXPECT_FALSE(stageClearReceived);
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 2);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene)
        << "Requirement 2.2: after enemy defeat (index 1→2), must route to Cutscene";

    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    // Kill enemy at index 2 — must fire StageClearEvent, scene returns to Info,
    // and no Cutscene appears for the final enemy.
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

    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(BattleService->GetSequence().empty());

    EventBus::Unsubscribe(handle);
}

TEST_F(SequentialBattlesIntegrationTest, PlayerDeath_StageFailAndSceneTransition)
{
    BattleService->StartStage();

    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene);

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

    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(BattleService->GetSequence().empty());

    EventBus::Unsubscribe(handle);
}

TEST_F(SequentialBattlesIntegrationTest, ConsecutiveStages_NewSequenceGeneratedAfterClear)
{
    // === Stage 1 ===
    BattleService->StartStage();
    ASSERT_EQ(BattleService->GetSequence().size(), 3u);

    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    TaggedCharacter enemy(ETag::Enemy);

    PublishDeath(enemy);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene);
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    PublishDeath(enemy);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene);
    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    PublishDeath(enemy);

    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0);
    EXPECT_TRUE(BattleService->GetSequence().empty());
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info);

    // === Stage 2 ===
    BattleService->StartStage();

    EXPECT_EQ(BattleService->GetSequence().size(), 3u)
        << "Requirement 1.1: second StartStage() must generate a new Sequence of size 3";
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 0)
        << "Requirement 1.1: currentIndex must be 0 at start of second stage";
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene)
        << "StartStage() must route to Cutscene scene for second stage";
}

TEST_F(SequentialBattlesIntegrationTest, PlayerDeathAfterPartialProgress_StageFailEvent)
{
    BattleService->StartStage();

    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    TaggedCharacter enemy(ETag::Enemy);
    PublishDeath(enemy);
    EXPECT_EQ(BattleService->GetCurrentEnemyIndex(), 1);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Cutscene);

    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

    bool stageFailReceived = false;
    auto handle = EventBus::Subscribe<StageFailEvent>(
        [&](const StageFailEvent&) { stageFailReceived = true; });

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
