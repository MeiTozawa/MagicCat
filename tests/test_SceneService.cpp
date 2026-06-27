#include <gtest/gtest.h>

import SceneService;
import BattleService;
import EventBus;

namespace mc {
namespace {

    class DummyScene : public IScene {
    public:
        int startCount = 0;
        int updateCount = 0;

        void Start() override { startCount++; }
        void Update(float deltaTime) override { updateCount++; }
    };

    class SceneServiceTest : public ::testing::Test {
    protected:
        void SetUp() override {
        }

        void TearDown() override {
        }
    };

    TEST_F(SceneServiceTest, PushScene_ChangesCurrentScene) {
        auto sceneService = CreateSceneService();
        
        auto infoSceneRaw = new DummyScene();
        auto combatSceneRaw = new DummyScene();

        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatSceneRaw));

        // Default scene should be Info
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info);

        sceneService->PushScene(ESceneState::Combat);

        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);
        EXPECT_EQ(combatSceneRaw->startCount, 1);
    }

    TEST_F(SceneServiceTest, PopScene_ReturnsToPreviousScene) {
        auto sceneService = CreateSceneService();
        
        sceneService->RegisterScene(ESceneState::Info, std::make_unique<DummyScene>());
        sceneService->RegisterScene(ESceneState::Combat, std::make_unique<DummyScene>());

        sceneService->PushScene(ESceneState::Combat);
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

        sceneService->PopScene();
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info);
    }

    TEST_F(SceneServiceTest, PopScene_WhenStackHasOnlyDefaultScene_DoesNotCrash) {
        auto sceneService = CreateSceneService();

        sceneService->RegisterScene(ESceneState::Info, std::make_unique<DummyScene>());

        // Pop when nothing is on the stack above the default — must not crash
        EXPECT_NO_FATAL_FAILURE(sceneService->PopScene());
    }

    TEST_F(SceneServiceTest, Update_DelegatesToCurrentScene) {
        auto sceneService = CreateSceneService();

        auto* dummyRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(dummyRaw));

        sceneService->Update(0.016f);
        sceneService->Update(0.016f);

        EXPECT_EQ(dummyRaw->updateCount, 2);
    }

    // --- Tests for Requirements 3.2 and 4.2 ---
    // SceneService must transition to Info when StageClearEvent or StageFailEvent is published,
    // replacing the old DeathEvent-based direct transition.

    TEST_F(SceneServiceTest, StageClearEvent_TransitionsToInfoScene) {
        // Requirement 3.2: SceneService transitions to Info on StageClearEvent
        auto sceneService = CreateSceneService();

        auto infoSceneRaw = new DummyScene();
        auto combatSceneRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatSceneRaw));

        // Start in Combat
        sceneService->PushScene(ESceneState::Combat);
        ASSERT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

        // Publishing StageClearEvent should trigger transition back to Info
        EventBus::Publish(StageClearEvent{});

        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info);
    }

    TEST_F(SceneServiceTest, StageClearEvent_RestartsInfoScene) {
        // Requirement 3.2: Info scene's Start() is called when StageClearEvent transitions back
        auto sceneService = CreateSceneService();

        auto infoSceneRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::make_unique<DummyScene>());

        // Force combat scene to be current
        sceneService->PushScene(ESceneState::Combat);
        int startCountBefore = infoSceneRaw->startCount;

        EventBus::Publish(StageClearEvent{});

        // Info scene's Start() must have been called once more
        EXPECT_EQ(infoSceneRaw->startCount, startCountBefore + 1);
    }

    TEST_F(SceneServiceTest, StageFailEvent_TransitionsToInfoScene) {
        // Requirement 4.2: SceneService transitions to Info on StageFailEvent
        auto sceneService = CreateSceneService();

        auto infoSceneRaw = new DummyScene();
        auto combatSceneRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatSceneRaw));

        // Start in Combat
        sceneService->PushScene(ESceneState::Combat);
        ASSERT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

        // Publishing StageFailEvent should trigger transition back to Info
        EventBus::Publish(StageFailEvent{});

        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info);
    }

    TEST_F(SceneServiceTest, StageFailEvent_RestartsInfoScene) {
        // Requirement 4.2: Info scene's Start() is called when StageFailEvent transitions back
        auto sceneService = CreateSceneService();

        auto infoSceneRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::make_unique<DummyScene>());

        // Force combat scene to be current
        sceneService->PushScene(ESceneState::Combat);
        int startCountBefore = infoSceneRaw->startCount;

        EventBus::Publish(StageFailEvent{});

        // Info scene's Start() must have been called once more
        EXPECT_EQ(infoSceneRaw->startCount, startCountBefore + 1);
    }

} // namespace
} // namespace mc
