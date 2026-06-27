#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockServices.h"

import SceneService;
import InputService;

using ::testing::Return;
using ::testing::NiceMock;

namespace mc {
namespace {

    class DummyScene : public IScene {
    public:
        void Start() override {}
        void Update(float deltaTime) override {}
    };

    class GameFlowIntegrationTest : public ::testing::Test {
    protected:
        std::unique_ptr<MockInputService> mockInput;
        std::unique_ptr<ISceneService> sceneService;

        void SetUp() override {
            mockInput = std::make_unique<NiceMock<MockInputService>>();
            sceneService = CreateSceneService();
            
            // Register dummy scenes to satisfy scene transitions
            sceneService->RegisterScene(ESceneState::Combat, std::make_unique<DummyScene>());
            sceneService->RegisterScene(ESceneState::Rules, std::make_unique<DummyScene>());
            
            sceneService->PushScene(ESceneState::Combat); // Start in combat scene
        }

        void TearDown() override {
        }
    };

    TEST_F(GameFlowIntegrationTest, SceneTransition_CombatToRulesAndBack) {
        // Initially in combat scene
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

        // Push Rules Scene
        sceneService->PushScene(ESceneState::Rules);
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Rules);

        // Pop Rules Scene
        sceneService->PopScene();
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);
    }

} // namespace
} // namespace mc
