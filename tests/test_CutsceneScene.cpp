#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockServices.h"

import SceneService;
import BattleService;
import AssetService;
import RenderService;
import EventBus;

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::NiceMock;

namespace mc {
namespace {

class MockSceneServiceCutscene : public ISceneService {
public:
    MOCK_METHOD(void, Update, (float), (override));
    MOCK_METHOD(void, PushScene, (ESceneState), (override));
    MOCK_METHOD(void, PopScene, (), (override));
    MOCK_METHOD(void, RegisterScene, (ESceneState, std::unique_ptr<IScene>&&), (override));
    MOCK_METHOD(ESceneState, GetCurrentScene, (), (override));
    MOCK_METHOD(void, SetCurrentScene, (ESceneState), (override));
};

TEST(CutsceneSceneTest, Start_LoadsEnemiesAndInitializesFocusDisplayer) {
    NiceMock<MockSceneServiceCutscene> mockSceneSvc;
    NiceMock<MockAssetService>         mockAssetSvc;
    NiceMock<MockRenderService>        mockRenderSvc;
    NiceMock<MockBattleService>        mockBattleSvc;
    
    ON_CALL(mockRenderSvc, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(mockRenderSvc, GetWindowHeight()).WillByDefault(Return(720));
    
    std::vector<EnemyConfig> sequence = {
        EnemyConfig{10, 1, 1, 1, 1, L"EnemyA", "Bunny"},
        EnemyConfig{15, 1, 1, 1, 1, L"EnemyB", "Wolf"}
    };
    ON_CALL(mockBattleSvc, GetSequence()).WillByDefault(ReturnRef(sequence));
    ON_CALL(mockBattleSvc, GetCurrentEnemyIndex()).WillByDefault(Return(1));
    
    EXPECT_CALL(mockAssetSvc, ParseSprite("Bunny")).WillOnce(Return(static_cast<ESprite>(1)));
    EXPECT_CALL(mockAssetSvc, ParseSprite("Wolf")).WillOnce(Return(static_cast<ESprite>(2)));
    
    auto scene = CreateCutsceneScene(mockSceneSvc, mockAssetSvc, mockRenderSvc, mockBattleSvc);
    scene->Start();
}

TEST(CutsceneSceneTest, Update_TriggersCutsceneFinishedEvent_AfterDuration) {
    NiceMock<MockSceneServiceCutscene> mockSceneSvc;
    NiceMock<MockAssetService>         mockAssetSvc;
    NiceMock<MockRenderService>        mockRenderSvc;
    NiceMock<MockBattleService>        mockBattleSvc;
    
    ON_CALL(mockRenderSvc, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(mockRenderSvc, GetWindowHeight()).WillByDefault(Return(720));
    ON_CALL(mockAssetSvc, GetSpriteInfo(_)).WillByDefault(Return(SpriteInfo{{32, 32}, 4}));
    
    std::vector<EnemyConfig> sequence = { {10, 1, 1, 1, 1, L"EnemyA", "Bunny"} };
    ON_CALL(mockBattleSvc, GetSequence()).WillByDefault(ReturnRef(sequence));
    ON_CALL(mockBattleSvc, GetCurrentEnemyIndex()).WillByDefault(Return(0));
    
    auto scene = CreateCutsceneScene(mockSceneSvc, mockAssetSvc, mockRenderSvc, mockBattleSvc);
    scene->Start();
    
    bool eventFired = false;
    auto handle = EventBus::Subscribe<CutsceneFinishedEvent>([&](const CutsceneFinishedEvent&) {
        eventFired = true;
    });
    
    // Cutscene duration is 1.0f seconds. Update by 0.5s -> no event
    scene->Update(0.5f);
    EXPECT_FALSE(eventFired);
    
    // Update by another 0.6s (total 1.1s) -> event must fire
    scene->Update(0.6f);
    EXPECT_TRUE(eventFired);
    
    EventBus::Unsubscribe(handle);
}

} // namespace
} // namespace mc
