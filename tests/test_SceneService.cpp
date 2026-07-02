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
        void SetUp() override {}
        void TearDown() override {}
    };

    TEST_F(SceneServiceTest, PushScene_ChangesCurrentScene) {
        auto sceneService = CreateSceneService();
        
        auto infoSceneRaw = new DummyScene();
        auto combatSceneRaw = new DummyScene();

        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatSceneRaw));

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

    // SceneService must transition to Info when StageClearEvent or StageFailEvent is published.

    TEST_F(SceneServiceTest, StageClearEvent_TransitionsToInfoScene) {
        auto sceneService = CreateSceneService();

        auto infoSceneRaw = new DummyScene();
        auto combatSceneRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatSceneRaw));

        sceneService->PushScene(ESceneState::Combat);
        ASSERT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

        EventBus::Publish(StageClearEvent{});

        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info);
    }

    TEST_F(SceneServiceTest, StageClearEvent_RestartsInfoScene) {
        auto sceneService = CreateSceneService();

        auto infoSceneRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::make_unique<DummyScene>());

        sceneService->PushScene(ESceneState::Combat);
        int startCountBefore = infoSceneRaw->startCount;

        EventBus::Publish(StageClearEvent{});

        EXPECT_EQ(infoSceneRaw->startCount, startCountBefore + 1);
    }

    TEST_F(SceneServiceTest, StageFailEvent_TransitionsToInfoScene) {
        auto sceneService = CreateSceneService();

        auto infoSceneRaw = new DummyScene();
        auto combatSceneRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatSceneRaw));

        sceneService->PushScene(ESceneState::Combat);
        ASSERT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

        EventBus::Publish(StageFailEvent{});

        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info);
    }

    TEST_F(SceneServiceTest, StageFailEvent_RestartsInfoScene) {
        auto sceneService = CreateSceneService();

        auto infoSceneRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoSceneRaw));
        sceneService->RegisterScene(ESceneState::Combat, std::make_unique<DummyScene>());

        sceneService->PushScene(ESceneState::Combat);
        int startCountBefore = infoSceneRaw->startCount;

        EventBus::Publish(StageFailEvent{});

        EXPECT_EQ(infoSceneRaw->startCount, startCountBefore + 1);
    }

} // namespace
} // namespace mc
