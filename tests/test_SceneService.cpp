#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <RenderUtils.h>
#include "MockServices.h"

import SceneService;
import Displayer;
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

// MenuIconDisplayer テスト
// Validates: Requirements 5.4, 5.5
// ============================================================

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;

namespace mc {

/// @brief ISceneService のローカルモック（SceneService モジュールが既にインポート済み）
class MockSceneService : public ISceneService {
public:
    MOCK_METHOD(void, Update, (float), (override));
    MOCK_METHOD(void, PushScene, (ESceneState), (override));
    MOCK_METHOD(void, PopScene, (), (override));
    MOCK_METHOD(void, RegisterScene, (ESceneState, std::unique_ptr<IScene>&&), (override));
    MOCK_METHOD(ESceneState, GetCurrentScene, (), (override));
    MOCK_METHOD(void, SetCurrentScene, (ESceneState), (override));
};

class MenuIconDisplayerTest : public ::testing::Test {
protected:
    NiceMock<MockSceneService>  mockScene;
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockAssetService>  mockAsset;
    NiceMock<MockOSService>     mockOS;

    std::unique_ptr<IDisplayer> controller;

    void SetUp() override {
        // デフォルト: クリックなし、マウスポジション原点（ホバーなし）
        ON_CALL(mockInput, OnMouseClick(_))
            .WillByDefault(Return(Point<int>{-1, -1}));
        ON_CALL(mockInput, GetMousePosition())
            .WillByDefault(Return(Point<int>{0, 0}));
        ON_CALL(mockInput, GetActiveDevice())
            .WillByDefault(Return(InputDevice::Mouse));
        ON_CALL(mockAsset, GetImageHandle(_))
            .WillByDefault(Return(-1));

        controller = CreateMenuIconDisplayer(
            mockScene, mockInput, mockRender, mockAsset, mockOS);
    }
};

/// @brief メニュー開状態でアイコンをクリックすると PopContext + PopScene が呼ばれる
/// Validates: Requirement 5.4
TEST_F(MenuIconDisplayerTest, MenuOpen_Click_CallsPopContextAndPopScene) {
    ON_CALL(mockScene, GetCurrentScene())
        .WillByDefault(Return(ESceneState::Menu));
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{MENU_ICON_X, MENU_ICON_Y}));

    EXPECT_CALL(mockInput, PopContext()).Times(1);
    EXPECT_CALL(mockScene, PopScene()).Times(1);
    EXPECT_CALL(mockScene, PushScene(_)).Times(0);

    controller->Update(0.016f);
}

/// @brief メニュー閉状態でアイコンをクリックすると PushScene(Menu) が呼ばれる
/// Validates: Requirement 5.4
TEST_F(MenuIconDisplayerTest, MenuClosed_Click_CallsPushSceneMenu) {
    ON_CALL(mockScene, GetCurrentScene())
        .WillByDefault(Return(ESceneState::Info));
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{MENU_ICON_X, MENU_ICON_Y}));
    ON_CALL(mockInput, IsPressed(InputAction::ToggleMenu))
        .WillByDefault(Return(false));

    EXPECT_CALL(mockScene, PushScene(ESceneState::Menu)).Times(1);
    EXPECT_CALL(mockScene, PopScene()).Times(0);
    EXPECT_CALL(mockInput, PopContext()).Times(0);

    controller->Update(0.016f);
}

/// @brief カットシーン中は描画・入力検出がスキップされる
/// Validates: Requirement 5.5
TEST_F(MenuIconDisplayerTest, Cutscene_SkipsDrawAndInput) {
    ON_CALL(mockScene, GetCurrentScene())
        .WillByDefault(Return(ESceneState::Cutscene));
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{MENU_ICON_X, MENU_ICON_Y}));

    EXPECT_CALL(mockRender, DrawRotaGraphF(_, _, _, _, _, _)).Times(0);
    EXPECT_CALL(mockScene, PushScene(_)).Times(0);
    EXPECT_CALL(mockScene, PopScene()).Times(0);
    EXPECT_CALL(mockInput, PopContext()).Times(0);

    controller->Update(0.016f);
}

/// @brief クリックなしの場合、シーン操作は一切呼ばれない
/// Validates: Requirement 5.4
TEST_F(MenuIconDisplayerTest, NoClick_NoSceneOperations) {
    ON_CALL(mockScene, GetCurrentScene())
        .WillByDefault(Return(ESceneState::Info));
    ON_CALL(mockInput, IsPressed(InputAction::ToggleMenu))
        .WillByDefault(Return(false));
    // OnMouseClick はデフォルトの {-1,-1} のまま

    EXPECT_CALL(mockScene, PushScene(_)).Times(0);
    EXPECT_CALL(mockScene, PopScene()).Times(0);
    EXPECT_CALL(mockInput, PopContext()).Times(0);

    controller->Update(0.016f);
}

} // namespace mc
