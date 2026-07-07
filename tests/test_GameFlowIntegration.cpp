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
        void Update(float) override {}
    };

    class GameFlowIntegrationTest : public ::testing::Test {
    protected:
        std::unique_ptr<MockInputService> mockInput;
        std::unique_ptr<ISceneService> sceneService;

        void SetUp() override {
            mockInput = std::make_unique<NiceMock<MockInputService>>();
            sceneService = CreateSceneService();
            
            sceneService->RegisterScene(ESceneState::Combat, std::make_unique<DummyScene>());
            sceneService->RegisterScene(ESceneState::Menu, std::make_unique<DummyScene>());
            
            sceneService->PushScene(ESceneState::Combat);
        }

        void TearDown() override {}
    };

    TEST_F(GameFlowIntegrationTest, SceneTransition_CombatToMenuAndBack) {
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);

        sceneService->PushScene(ESceneState::Menu);
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Menu);

        sceneService->PopScene();
        EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);
    }

} // namespace
} // namespace mc
