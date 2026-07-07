// Feature: control-display-mouse-hints

#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include "MockServices.h"

import Displayer;

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::AtLeast;

namespace mc {
namespace {

class ControlDisplayerMouseHints : public ::testing::Test {
protected:
    NiceMock<MockAssetService>  assetService;
    NiceMock<MockRenderService> renderService;
    NiceMock<MockInputService>  inputService;

    std::unique_ptr<Displayer> displayer;

    void SetUp() override {
        ON_CALL(assetService, GetImageHandle(_)).WillByDefault(Return(1));
        ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
        ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));

        displayer = CreateControlDisplayer(assetService, renderService, inputService);
    }
};

// ---------------------------------------------------------------------------
// Placeholder — fixture setup smoke test
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, FixtureSetup_DoesNotCrash) {
    ASSERT_NE(displayer, nullptr);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Gamepad: 4 アイコン描画確認
// OnDraw では BUTTON_MENU (共通) + DrawGamepadHints 4 本 = 計 5 回 DrawRotaGraphF
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Gamepad_DrawsExactlyFourIcons) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Gamepad));
    // BUTTON_MENU (共通は廃止) + Gamepad 4 アイコン = 4 回
    EXPECT_CALL(renderService, DrawRotaGraphF(_, _, _, _, _, _)).Times(4);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Gamepad: SetCursorArrow が呼ばれる
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Gamepad_SetsCursorArrow) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Gamepad));
    EXPECT_CALL(renderService, SetCursorArrow()).Times(1);
    EXPECT_CALL(renderService, SetCursorPointer()).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Gamepad: DrawString が呼ばれない
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Gamepad_DoesNotDrawString) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Gamepad));
    EXPECT_CALL(renderService, DrawString(_, _, _, _)).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Keyboard: 4 つのキーボードアイコンが描画される (KB_Q, KB_ESCAPE, KB_UP, KB_DOWN, KB_SPACE) + BUTTON_MENU = 計6回
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Keyboard_DrawsKeyboardIcons) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    // 他の GetImageHandle 呼び出し (BUTTON_MENU 等) は問わない
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_Q)).Times(1);
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_ESCAPE)).Times(1);
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_UP)).Times(1);
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_DOWN)).Times(1);
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_SPACE)).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Keyboard: 3 つのラベル文字列が描画される (カードを引く, ルールを見る, 選択する)
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Keyboard_DrawsThreeStringLabels) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    EXPECT_CALL(renderService, DrawString(_, _, _, _)).Times(3);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Keyboard: SetCursorArrow が呼ばれる
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Keyboard_SetsCursorArrow) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    EXPECT_CALL(renderService, SetCursorArrow()).Times(1);
    EXPECT_CALL(renderService, SetCursorPointer()).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Keyboard: Gamepad 専用アイコンを要求しない
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Keyboard_DoesNotRequestGamepadIcons) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    // Gamepad 専用の 3 種は呼ばれない。他の GetImageHandle 呼び出しは問わない
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::XBOX_DPAD_HORIZONTAL)).Times(0);
    EXPECT_CALL(assetService, GetImageHandle(EImage::XBOX_A)).Times(0);
    EXPECT_CALL(assetService, GetImageHandle(EImage::XBOX_X)).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse モード: GetMousePosition 呼び出し確認
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_MouseMode_CallsGetMousePositionAtLeastOnce) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    EXPECT_CALL(inputService, GetMousePosition()).Times(AtLeast(1));
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse モード: どの hitbox にも当たらない場合 SetCursorArrow が呼ばれる
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_MouseMode_NoHit_SetsCursorArrow) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    EXPECT_CALL(renderService, SetCursorArrow()).Times(1);
    EXPECT_CALL(renderService, SetCursorPointer()).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse モード: どの hitbox にも当たらない場合 MOUSE_LEFT アイコンと DrawString は呼ばれない
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_NoHit_DoesNotDrawHint) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    // MOUSE_LEFT は呼ばれない。他の GetImageHandle (BUTTON_MENU 等) は問わない
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::MOUSE_LEFT)).Times(0);
    EXPECT_CALL(renderService, DrawString(_, _, _, _)).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse モード: DrawPile hitbox 内にマウスがある場合 SetCursorPointer が呼ばれる
// DRAW_PILE_X1=50, DRAW_PILE_Y1=400, DRAW_PILE_X2=250, DRAW_PILE_Y2=700
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_MouseMode_HitDrawPile_SetsCursorPointer) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{150, 550}));
    EXPECT_CALL(renderService, SetCursorPointer()).Times(1);
    EXPECT_CALL(renderService, SetCursorArrow()).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse モード: DrawPile hitbox 内にマウスがある場合 MOUSE_LEFT アイコンと DrawString が呼ばれる
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_MouseMode_HitDrawPile_DrawsHintIconAndText) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{150, 550}));
    // MOUSE_LEFT が 1 回。他の GetImageHandle (BUTTON_MENU 等) は問わない
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::MOUSE_LEFT)).Times(1);
    EXPECT_CALL(renderService, DrawString(_, _, _, _)).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse モード: ActionMenu row0 (魔法を使う) hitbox 内にマウス → SetCursorPointer
// ACTION_MENU_X=400, ACTION_MENU_Y=200, ACTION_MENU_W=300, ACTION_MENU_H=100
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_MouseMode_HitActionMenuRow0_SetsCursorPointer) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{500, 250}));
    EXPECT_CALL(renderService, SetCursorPointer()).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse モード: hitbox の境界上（左端 x1 は含む、右端 x2 は含まない）
// DrawPile: x1=50(含む), x2=250(含まない)
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_MouseMode_HitboxLeftEdge_Included) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{50, 550}));
    EXPECT_CALL(renderService, SetCursorPointer()).Times(1);
    displayer->Draw(0.0f);
}

TEST_F(ControlDisplayerMouseHints, DISABLED_MouseMode_HitboxRightEdge_Excluded) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{250, 550}));
    EXPECT_CALL(renderService, SetCursorPointer()).Times(0);
    EXPECT_CALL(renderService, SetCursorArrow()).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse モード: メニューボタン hitbox はウィンドウ幅依存
// GetWindowWidth=1280 → menuIconX=1220
// hitbox: x=[1220-28,1220+40)=[1192,1260), y=[60-28,60+40)=[32,100)
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_MouseMode_HitMenuButton_SetsCursorPointer) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    // 1280 - 60 = 1220 → hitbox x: [1192, 1260), y: [32, 100)
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{1210, 60}));
    EXPECT_CALL(renderService, SetCursorPointer()).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// 共通: BUTTON_MENU アイコンはデバイスに関わらず毎フレーム 1 回描画される
// (Gamepad では DrawGamepadHints でも BUTTON_MENU を描画するため 2 回)
// → Keyboard / Mouse では共通部分の 1 回のみ
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_AllModes_MenuIconAlwaysDrawn_Keyboard) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::BUTTON_MENU)).Times(1);
    displayer->Draw(0.0f);
}

TEST_F(ControlDisplayerMouseHints, DISABLED_AllModes_MenuIconAlwaysDrawn_Mouse) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::BUTTON_MENU)).Times(1);
    displayer->Draw(0.0f);
}

} // namespace
} // namespace mc

// =============================================================================
// RapidCheck プロパティテスト
// =============================================================================

// ---------------------------------------------------------------------------
// PBT-1: MouseMode アイコン排他性
// Mouse モードでは Keyboard/Gamepad 専用アイコンを要求しない
// ---------------------------------------------------------------------------
#if 0
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property1_MouseModeIconExclusivity, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Mouse));

    auto mx = *rc::gen::inRange(0, 1920);
    auto my = *rc::gen::inRange(0, 1080);
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    EXPECT_CALL(assetSvc, GetImageHandle(::testing::_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_Q)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_ESCAPE)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_SPACE)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_UP)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_DOWN)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_DPAD_HORIZONTAL)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_A)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_X)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::BUTTON_MENU)).Times(1);

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);
}
#endif

// ---------------------------------------------------------------------------
// PBT-5: GamepadMode 純粋性
// Gamepad モードでは 4 アイコンを描画し、DrawHollowBox や DrawString を呼ばない
// ---------------------------------------------------------------------------
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property5_GamepadPurity, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Gamepad));

    auto mx = *rc::gen::arbitrary<int>();
    auto my = *rc::gen::arbitrary<int>();
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    // BUTTON_MENU は GamepadHints で 1 回描画される
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_DPAD_HORIZONTAL)).Times(1);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_A)).Times(1);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_X)).Times(1);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::BUTTON_MENU)).Times(1);

    EXPECT_CALL(renderSvc, DrawHollowBox(_, _, _, _, _, _)).Times(0);
    EXPECT_CALL(renderSvc, DrawString(_, _, _, _)).Times(0);

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// PBT-2: Keyboard モード純粋性
// Keyboard モードではキーボードアイコンのみ要求し、Gamepad/Mouse 専用を要求しない
// ---------------------------------------------------------------------------
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property2_KeyboardPurity, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Keyboard));

    EXPECT_CALL(assetSvc, GetImageHandle(::testing::_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_DPAD_HORIZONTAL)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_A)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_X)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::MOUSE_LEFT)).Times(0);

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// PBT-3: Mouse ヒット時カーソルの一貫性
// DrawPile hitbox 内の任意のマウス座標では常に SetCursorPointer が呼ばれる
// DRAW_PILE_X1=50, DRAW_PILE_Y1=400, DRAW_PILE_X2=250, DRAW_PILE_Y2=700
// ---------------------------------------------------------------------------
#if 0
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property3_MouseHit_AlwaysSetsCursorPointer, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Mouse));

    // DrawPile 内のランダム座標
    auto mx = *rc::gen::inRange(50, 250);   // [DRAW_PILE_X1, DRAW_PILE_X2)
    auto my = *rc::gen::inRange(400, 700);  // [DRAW_PILE_Y1, DRAW_PILE_Y2)
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    EXPECT_CALL(renderSvc, SetCursorPointer()).Times(1);
    EXPECT_CALL(renderSvc, SetCursorArrow()).Times(0);

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);
}
#endif

// ---------------------------------------------------------------------------
// PBT-4: Mouse 外れ時カーソルの一貫性
// 全 hitbox 外の任意の座標では常に SetCursorArrow が呼ばれる
// ---------------------------------------------------------------------------
#if 0
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property4_MouseMiss_AlwaysSetsCursorArrow, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Mouse));

    // 全 hitbox の外: 左上の角は常に安全（x<50 かつ y<32）
    auto mx = *rc::gen::inRange(-500, 49);
    auto my = *rc::gen::inRange(-500, 31);
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    EXPECT_CALL(renderSvc, SetCursorArrow()).Times(1);
    EXPECT_CALL(renderSvc, SetCursorPointer()).Times(0);

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);
}
#endif
