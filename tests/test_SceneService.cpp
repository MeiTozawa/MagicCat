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
    protected:
        void SetUp() override {
            // Prevent actual scenes from loading, which would require extensive mocking
            SceneRegistry::GetRegistrations().clear();
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

} // namespace
} // namespace mc
