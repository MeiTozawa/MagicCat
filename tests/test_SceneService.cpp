#include <gtest/gtest.h>

import SceneService;
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
        void SetUp() override {
        }

        void TearDown() override {
        }
    };

    TEST_F(SceneServiceTest, PushScene_ChangesCurrentScene) {
        auto sceneService = CreateSceneService(nullptr);
        
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
        auto sceneService = CreateSceneService(nullptr);
        
        sceneService->RegisterScene(ESceneState::Info, std::make_unique<DummyScene>());
        sceneService->RegisterScene(ESceneState::Combat, std::make_unique<DummyScene>());

        sceneService->PushScene(ESceneState::Combat);
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

        sceneService->PopScene();
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Info);
    }

    TEST_F(SceneServiceTest, PopScene_WhenStackHasOnlyDefaultScene_DoesNotCrash) {
        auto sceneService = CreateSceneService(nullptr);

        sceneService->RegisterScene(ESceneState::Info, std::make_unique<DummyScene>());

        // Pop when nothing is on the stack above the default — must not crash
        EXPECT_NO_FATAL_FAILURE(sceneService->PopScene());
    }

    TEST_F(SceneServiceTest, Update_DelegatesToCurrentScene) {
        auto sceneService = CreateSceneService(nullptr);

        auto* dummyRaw = new DummyScene();
        sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(dummyRaw));

        sceneService->Update(0.016f);
        sceneService->Update(0.016f);

        EXPECT_EQ(dummyRaw->updateCount, 2);
    }

} // namespace
} // namespace mc
