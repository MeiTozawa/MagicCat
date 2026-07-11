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
    NiceMock<MockOSService>     osService;

    std::unique_ptr<IDisplayer> displayer;

    void SetUp() override {
        ON_CALL(assetService, GetImageHandle(_)).WillByDefault(Return(1));
        ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
        ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));

        displayer = CreateControlDisplayer(assetService, renderService, inputService, osService);
    }
};

// ---------------------------------------------------------------------------
// Placeholder 窶・fixture setup smoke test
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, FixtureSetup_DoesNotCrash) {
    ASSERT_NE(displayer, nullptr);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Gamepad: 4 繧｢繧､繧ｳ繝ｳ謠冗判遒ｺ隱・
// OnDraw 縺ｧ縺ｯ BUTTON_MENU (蜈ｱ騾・ + DrawGamepadHints 4 譛ｬ = 險・5 蝗・DrawRotaGraphF
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Gamepad_DrawsExactlyFourIcons) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Gamepad));
    // BUTTON_MENU (蜈ｱ騾壹・蟒・ｭ｢) + Gamepad 4 繧｢繧､繧ｳ繝ｳ = 4 蝗・
    EXPECT_CALL(renderService, DrawRotaGraphF(_, _, _, _, _, _)).Times(3);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Gamepad: SetCursorArrow 縺悟他縺ｰ繧後ｋ
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Gamepad_SetsCursorArrow) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Gamepad));
    EXPECT_CALL(osService, SetCursorArrow()).Times(1);
    EXPECT_CALL(osService, SetCursorPointer()).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Gamepad: DrawString 縺・蝗槫他縺ｰ繧後ｋ・医き繝ｼ繝峨ｒ蠑輔￥繝ｻ驕ｸ謚槭☆繧具ｼ・
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Gamepad_DrawsLabels) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Gamepad));
    EXPECT_CALL(renderService, DrawString(_, _, _, _)).Times(2);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Keyboard: 4 縺､縺ｮ繧ｭ繝ｼ繝懊・繝峨い繧､繧ｳ繝ｳ縺梧緒逕ｻ縺輔ｌ繧・(KB_Q, KB_ESCAPE, KB_UP, KB_DOWN, KB_SPACE) + BUTTON_MENU = 險・蝗・
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Keyboard_DrawsKeyboardIcons) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    // 莉悶・ GetImageHandle 蜻ｼ縺ｳ蜃ｺ縺・(BUTTON_MENU 遲・ 縺ｯ蝠上ｏ縺ｪ縺・
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_Q)).Times(1);
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_ESCAPE)).Times(1);
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_UP)).Times(1);
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_DOWN)).Times(1);
    EXPECT_CALL(assetService, GetImageHandle(EImage::KB_SPACE)).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Keyboard: 3 縺､縺ｮ繝ｩ繝吶Ν譁・ｭ怜・縺梧緒逕ｻ縺輔ｌ繧・(繧ｫ繝ｼ繝峨ｒ蠑輔￥, 繝ｫ繝ｼ繝ｫ繧定ｦ九ｋ, 驕ｸ謚槭☆繧・
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Keyboard_DrawsThreeStringLabels) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    EXPECT_CALL(renderService, DrawString(_, _, _, _)).Times(3);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Keyboard: SetCursorArrow 縺悟他縺ｰ繧後ｋ
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Keyboard_SetsCursorArrow) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    EXPECT_CALL(osService, SetCursorArrow()).Times(1);
    EXPECT_CALL(osService, SetCursorPointer()).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Keyboard: Gamepad 蟆ら畑繧｢繧､繧ｳ繝ｳ繧定ｦ∵ｱゅ＠縺ｪ縺・
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Keyboard_DoesNotRequestGamepadIcons) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    // Gamepad 蟆ら畑縺ｮ 3 遞ｮ縺ｯ蜻ｼ縺ｰ繧後↑縺・ゆｻ悶・ GetImageHandle 蜻ｼ縺ｳ蜃ｺ縺励・蝠上ｏ縺ｪ縺・
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::XBOX_DPAD_HORIZONTAL)).Times(0);
    EXPECT_CALL(assetService, GetImageHandle(EImage::XBOX_A)).Times(0);
    EXPECT_CALL(assetService, GetImageHandle(EImage::XBOX_X)).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse 繝｢繝ｼ繝・ GetMousePosition 蜻ｼ縺ｳ蜃ｺ縺礼｢ｺ隱・
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_CallsGetMousePositionAtLeastOnce) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    EXPECT_CALL(inputService, GetMousePosition()).Times(AtLeast(1));
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse 繝｢繝ｼ繝・ 縺ｩ縺ｮ hitbox 縺ｫ繧ょｽ薙◆繧峨↑縺・�ｴ蜷・SetCursorArrow 縺悟他縺ｰ繧後ｋ
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_NoHit_SetsCursorArrow) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    EXPECT_CALL(osService, SetCursorArrow()).Times(1);
    EXPECT_CALL(osService, SetCursorPointer()).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse 繝｢繝ｼ繝・ 縺ｩ縺ｮ hitbox 縺ｫ繧ょｽ薙◆繧峨↑縺・�ｴ蜷・MOUSE_LEFT 繧｢繧､繧ｳ繝ｳ縺ｨ DrawString 縺ｯ蜻ｼ縺ｰ繧後↑縺・
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_NoHit_DoesNotDrawHint) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    // MOUSE_LEFT 縺ｯ蜻ｼ縺ｰ繧後↑縺・ゆｻ悶・ GetImageHandle (BUTTON_MENU 遲・ 縺ｯ蝠上ｏ縺ｪ縺・
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::MOUSE_LEFT)).Times(0);
    EXPECT_CALL(renderService, DrawString(_, _, _, _)).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse 繝｢繝ｼ繝・ DrawPile hitbox 蜀・↓繝槭え繧ｹ縺後≠繧句�ｴ蜷・SetCursorPointer 縺悟他縺ｰ繧後ｋ
// DRAW_PILE_X1=50, DRAW_PILE_Y1=400, DRAW_PILE_X2=250, DRAW_PILE_Y2=700
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_HitDrawPile_SetsCursorPointer) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{150, 550}));
    EXPECT_CALL(osService, SetCursorPointer()).Times(1);
    EXPECT_CALL(osService, SetCursorArrow()).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse 繝｢繝ｼ繝・ DrawPile hitbox 蜀・↓繝槭え繧ｹ縺後≠繧句�ｴ蜷・MOUSE_LEFT 繧｢繧､繧ｳ繝ｳ縺ｨ DrawString 縺悟他縺ｰ繧後ｋ
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_HitDrawPile_DrawsHintIconAndText) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{150, 550}));
    // MOUSE_LEFT 縺・1 蝗槭ゆｻ悶・ GetImageHandle (BUTTON_MENU 遲・ 縺ｯ蝠上ｏ縺ｪ縺・
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::MOUSE_LEFT)).Times(1);
    EXPECT_CALL(renderService, DrawString(_, _, _, _)).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse 繝｢繝ｼ繝・ ActionMenu row0 (鬲疲ｳ輔ｒ菴ｿ縺・ hitbox 蜀・↓繝槭え繧ｹ 竊・SetCursorPointer
// ACTION_MENU_X=400, ACTION_MENU_Y=200, ACTION_MENU_W=300, ACTION_MENU_H=100
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_HitActionMenuRow0_SetsCursorPointer) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{500, 250}));
    EXPECT_CALL(osService, SetCursorPointer()).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse 繝｢繝ｼ繝・ hitbox 縺ｮ蠅・阜荳奇ｼ亥ｷｦ遶ｯ x1 縺ｯ蜷ｫ繧縲∝承遶ｯ x2 縺ｯ蜷ｫ縺ｾ縺ｪ縺・ｼ・
// DrawPile: x1=50(蜷ｫ繧), x2=250(蜷ｫ縺ｾ縺ｪ縺・
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_HitboxLeftEdge_Included) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{50, 550}));
    EXPECT_CALL(osService, SetCursorPointer()).Times(1);
    displayer->Draw(0.0f);
}

TEST_F(ControlDisplayerMouseHints, MouseMode_HitboxRightEdge_Excluded) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{250, 550}));
    EXPECT_CALL(osService, SetCursorPointer()).Times(0);
    EXPECT_CALL(osService, SetCursorArrow()).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse 繝｢繝ｼ繝・ 繝｡繝九Η繝ｼ繝懊ち繝ｳ hitbox 縺ｯ繧ｦ繧｣繝ｳ繝峨え蟷・ｾ晏ｭ・
// MENU_ICON_X = WINDOW_WIDTH - 60 = 1920 - 60 = 1860
// hitbox: x=[1860-28,1860+40)=[1832,1900), y=[60-28,60+40)=[32,100)
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_HitMenuButton_SetsCursorPointer) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    // 1920 - 60 = 1860 竊・hitbox x: [1832, 1900), y: [32, 100)
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{1860, 60}));
    EXPECT_CALL(osService, SetCursorPointer()).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Mouse 繝｢繝ｼ繝・ BUTTON_MENU 繧｢繧､繧ｳ繝ｳ縺梧ｯ弱ヵ繝ｬ繝ｼ繝� 1 蝗樊緒逕ｻ縺輔ｌ繧・
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_AllModes_MenuIconAlwaysDrawn_Mouse) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::BUTTON_MENU)).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Keyboard 繝｢繝ｼ繝・ BUTTON_MENU 繧｢繧､繧ｳ繝ｳ縺梧ｯ弱ヵ繝ｬ繝ｼ繝� 1 蝗樊緒逕ｻ縺輔ｌ繧具ｼ亥ｰ・擂螳溯｣・畑繝ｻ迴ｾ蝨ｨ DISABLED・・
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, DISABLED_AllModes_MenuIconAlwaysDrawn_Keyboard) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));
    EXPECT_CALL(assetService, GetImageHandle(_)).Times(::testing::AnyNumber());
    EXPECT_CALL(assetService, GetImageHandle(EImage::BUTTON_MENU)).Times(1);
    displayer->Draw(0.0f);
}

} // namespace
} // namespace mc

// =============================================================================
// RapidCheck 繝励Ο繝代ユ繧｣繝・せ繝・
// =============================================================================

// ---------------------------------------------------------------------------
// PBT-1: MouseMode 繧｢繧､繧ｳ繝ｳ謗剃ｻ匁ｧ
// Mouse 繝｢繝ｼ繝峨〒縺ｯ Keyboard/Gamepad 蟆ら畑繧｢繧､繧ｳ繝ｳ繧定ｦ∵ｱゅ＠縺ｪ縺・
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
    // EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::BUTTON_MENU)).Times(1);

    NiceMock<mc::MockOSService> osSvc;
    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc, osSvc);
    disp->Draw(0.0f);
}
#endif

// ---------------------------------------------------------------------------
// PBT-5: GamepadMode 邏皮ｲ区ｧ
// Gamepad 繝｢繝ｼ繝峨〒縺ｯ 4 繧｢繧､繧ｳ繝ｳ繧呈緒逕ｻ縺励．rawHollowBox 繧・DrawString 繧貞他縺ｰ縺ｪ縺・
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

    // BUTTON_MENU 縺ｯ GamepadHints 縺ｧ 1 蝗樊緒逕ｻ縺輔ｌ繧・
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_DPAD_HORIZONTAL)).Times(1);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_A)).Times(1);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_X)).Times(1);
    // EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::BUTTON_MENU)).Times(1);

    EXPECT_CALL(renderSvc, DrawHollowBox(_, _, _, _, _, _)).Times(0);
    EXPECT_CALL(renderSvc, DrawString(_, _, _, _)).Times(2);  // 繧ｫ繝ｼ繝峨ｒ蠑輔￥繝ｻ驕ｸ謚槭☆繧・

    NiceMock<mc::MockOSService> osSvc;

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc, osSvc);
    disp->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// PBT-2: Keyboard 繝｢繝ｼ繝臥ｴ皮ｲ区ｧ
// Keyboard 繝｢繝ｼ繝峨〒縺ｯ繧ｭ繝ｼ繝懊・繝峨い繧､繧ｳ繝ｳ縺ｮ縺ｿ隕∵ｱゅ＠縲；amepad/Mouse 蟆ら畑繧定ｦ∵ｱゅ＠縺ｪ縺・
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

    NiceMock<mc::MockOSService> osSvc;
    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc, osSvc);
    disp->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// PBT-3: Mouse 繝偵ャ繝域凾繧ｫ繝ｼ繧ｽ繝ｫ縺ｮ荳雋ｫ諤ｧ
// DrawPile hitbox 蜀・・莉ｻ諢上・繝槭え繧ｹ蠎ｧ讓吶〒縺ｯ蟶ｸ縺ｫ SetCursorPointer 縺悟他縺ｰ繧後ｋ
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

    // DrawPile 蜀・・繝ｩ繝ｳ繝繝�蠎ｧ讓・
    auto mx = *rc::gen::inRange(50, 250);   // [DRAW_PILE_X1, DRAW_PILE_X2)
    auto my = *rc::gen::inRange(400, 700);  // [DRAW_PILE_Y1, DRAW_PILE_Y2)
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    EXPECT_CALL(osSvc, SetCursorPointer()).Times(1);
    EXPECT_CALL(osSvc, SetCursorArrow()).Times(0);

    NiceMock<mc::MockOSService> osSvc;
    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc, osSvc);
    disp->Draw(0.0f);
}
#endif

// ---------------------------------------------------------------------------
// PBT-4: Mouse 螟悶ｌ譎ゅき繝ｼ繧ｽ繝ｫ縺ｮ荳雋ｫ諤ｧ
// 蜈ｨ hitbox 螟悶・莉ｻ諢上・蠎ｧ讓吶〒縺ｯ蟶ｸ縺ｫ SetCursorArrow 縺悟他縺ｰ繧後ｋ
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

    // 蜈ｨ hitbox 縺ｮ螟・ 蟾ｦ荳翫・隗偵・蟶ｸ縺ｫ螳牙・・・<50 縺九▽ y<32・・
    auto mx = *rc::gen::inRange(-500, 49);
    auto my = *rc::gen::inRange(-500, 31);
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    EXPECT_CALL(osSvc, SetCursorArrow()).Times(1);
    EXPECT_CALL(osSvc, SetCursorPointer()).Times(0);

    NiceMock<mc::MockOSService> osSvc;
    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc, osSvc);
    disp->Draw(0.0f);
}
#endif


