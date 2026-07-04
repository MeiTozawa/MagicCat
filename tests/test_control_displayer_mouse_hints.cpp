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
        ON_CALL(renderService, GetWindowWidth()).WillByDefault(Return(1280));
        ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
        ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));

        displayer = CreateControlDisplayer(assetService, renderService, inputService);
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
// Task 5.2 窶・Gamepad: 4 繧｢繧､繧ｳ繝ｳ謠冗判遒ｺ隱・
// Validates: Requirements 5.1
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, Gamepad_DrawsExactlyFourIcons) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Gamepad));
    EXPECT_CALL(renderService, DrawRotaGraphF(_, _, _, _, _, _)).Times(4);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Task 5.3 窶・GetMousePosition 蜻ｼ縺ｳ蜃ｺ縺礼｢ｺ隱・
// Validates: Requirements 6.1
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerMouseHints, MouseMode_CallsGetMousePositionAtLeastOnce) {
    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Mouse));
    EXPECT_CALL(inputService, GetMousePosition()).Times(AtLeast(1));
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Task 5.4 窶・MenuHitBox 蠅・阜繝斐け繧ｻ繝ｫ・亥・蛛ｴ繝ｻ螟門・・・
// Validates: Requirements 2.1, 2.3
// windowWidth=1280 竊・menuIconX=1220, MenuHitBox=[1180,1260)ﾃ夕32,88)
// ---------------------------------------------------------------------------

// mx=1180 (= menuIconX-40, 譛蟾ｦ遶ｯ繝ｻ蜀・・): 繝偵Φ繝郁｡ｨ遉ｺ
TEST_F(ControlDisplayerMouseHints, MenuHitBox_LeftBoundary_Inside_ShowsHint) {
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{1180, 60}));
    EXPECT_CALL(renderService, DrawString(20, 20, ::testing::StrEq(L"繧ｯ繝ｪ繝・け縺励※繝｡繝九Η繝ｼ繧帝幕縺・), 0xFFFF00u));
    displayer->Draw(0.0f);
}

// mx=1179 (= menuIconX-41, 螟門・): 繝偵Φ繝磯撼陦ｨ遉ｺ
TEST_F(ControlDisplayerMouseHints, MenuHitBox_LeftBoundary_Outside_NoHint) {
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{1179, 60}));
    EXPECT_CALL(renderService, DrawString(20, 20, _, _)).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Task 5.5 窶・DrawCardHitBox 蠅・阜繝斐け繧ｻ繝ｫ・亥・蛛ｴ繝ｻ螟門・・・
// Validates: Requirements 3.1, 3.2
// DrawCardHitBox = [50,250)ﾃ夕400,700)
// ---------------------------------------------------------------------------

// (50, 400) 窶・蟾ｦ荳願ｧ偵・蜀・・: 繧ｫ繝ｼ繝牙ｼ輔″繝偵Φ繝郁｡ｨ遉ｺ
TEST_F(ControlDisplayerMouseHints, DrawCardHitBox_TopLeftCorner_ShowsCardHint) {
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{50, 400}));
    EXPECT_CALL(renderService, DrawString(20, 20, ::testing::StrEq(L"繧ｯ繝ｪ繝・け縺励※繧ｫ繝ｼ繝峨ｒ蠑輔￥"), 0xFFFF00u));
    displayer->Draw(0.0f);
}

// (49, 400) 窶・蟾ｦ螟門・: 繝偵Φ繝磯撼陦ｨ遉ｺ
TEST_F(ControlDisplayerMouseHints, DrawCardHitBox_LeftOutside_NoHint) {
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{49, 400}));
    EXPECT_CALL(renderService, DrawString(20, 20, _, _)).Times(0);
    displayer->Draw(0.0f);
}

// (250, 400) 窶・蜿ｳ螟門・・亥承髢句玄髢難ｼ・ 繝偵Φ繝磯撼陦ｨ遉ｺ
TEST_F(ControlDisplayerMouseHints, DrawCardHitBox_RightOutside_NoHint) {
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{250, 400}));
    EXPECT_CALL(renderService, DrawString(20, 20, _, _)).Times(0);
    displayer->Draw(0.0f);
}

// (50, 399) 窶・荳雁､門・: 繝偵Φ繝磯撼陦ｨ遉ｺ
TEST_F(ControlDisplayerMouseHints, DrawCardHitBox_TopOutside_NoHint) {
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{50, 399}));
    EXPECT_CALL(renderService, DrawString(20, 20, _, _)).Times(0);
    displayer->Draw(0.0f);
}

// (50, 700) 窶・荳句､門・・亥承髢句玄髢難ｼ・ 繝偵Φ繝磯撼陦ｨ遉ｺ
TEST_F(ControlDisplayerMouseHints, DrawCardHitBox_BottomOutside_NoHint) {
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{50, 700}));
    EXPECT_CALL(renderService, DrawString(20, 20, _, _)).Times(0);
    displayer->Draw(0.0f);
}

} // namespace
} // namespace mc

// =============================================================================
// RapidCheck 繝励Ο繝代ユ繧｣繝・せ繝茨ｼ・BT-1縲・・・
// RC_GTEST_PROP 縺ｯ TEST() 繝槭け繝ｭ繧剃ｽｿ縺・◆繧√’ixture 繧ｯ繝ｩ繧ｹ蜷阪→陦晉ｪ√＠縺ｪ縺・ｈ縺・
// "ControlDisplayerMouseHintsProps" 縺ｨ縺・≧繧ｹ繧､繝ｼ繝亥錐繧堤畑縺・ｋ縲・
// =============================================================================

// ---------------------------------------------------------------------------
// Task 6.1 窶・PBT-1: MouseMode 繧｢繧､繧ｳ繝ｳ謗剃ｻ匁ｧ
// Property 1: MouseMode 繧｢繧､繧ｳ繝ｳ謗剃ｻ匁ｧ
// Validates: Requirements 1.2, 5.2
// ---------------------------------------------------------------------------
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property1_MouseModeIconExclusivity, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(renderSvc, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Mouse));

    auto mx = *rc::gen::inRange(0, 1920);
    auto my = *rc::gen::inRange(0, 1080);
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    // In KeyboardMode these image handles must NEVER be requested
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_Q)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_ESCAPE)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_SPACE)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_UP)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::KB_DOWN)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_DPAD_HORIZONTAL)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_A)).Times(0);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_X)).Times(0);
    // In KeyboardMode BUTTON_MENU is drawn once (top-right corner icon)
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::BUTTON_MENU)).Times(1);

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Task 6.2 窶・PBT-2: MenuHitBox 繝帙ヰ繝ｼ譎ゅ・隍・粋繝輔ぅ繝ｼ繝峨ヰ繝・け
// Property 2: MenuHitBox 繝帙ヰ繝ｼ譎ゅ・隍・粋繝輔ぅ繝ｼ繝峨ヰ繝・け
// Validates: Requirements 2.1, 2.2
// menuIconX=1220 (windowWidth=1280), MenuHitBox=[1180,1260)ﾃ夕32,88)
// ---------------------------------------------------------------------------
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property2_MenuHitBoxHoverFeedback, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(renderSvc, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Mouse));

    // Generate (mx, my) strictly inside MenuHitBox
    auto mx = *rc::gen::inRange(1180, 1260);
    auto my = *rc::gen::inRange(32, 88);
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    // Must draw hint text
    EXPECT_CALL(renderSvc, DrawString(20, 20, ::testing::StrEq(L"繧ｯ繝ｪ繝・け縺励※繝｡繝九Η繝ｼ繧帝幕縺・), 0xFFFF00u))
        .Times(1);
    // Must draw hollow box with COLOR_HOVER
    EXPECT_CALL(renderSvc, DrawHollowBox(::testing::_, ::testing::_, ::testing::_, ::testing::_, 2, 0xFFFF00u))
        .Times(1);

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Task 6.3 窶・PBT-3: DrawCardHitBox 繝帙ヰ繝ｼ譎ゅ・繝偵Φ繝郁｡ｨ遉ｺ
// Property 3: DrawCardHitBox 繝帙ヰ繝ｼ譎ゅ・繝偵Φ繝郁｡ｨ遉ｺ
// Validates: Requirements 3.1
// DrawCardHitBox = [50,250)ﾃ夕400,700)
// ---------------------------------------------------------------------------
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property3_DrawCardHitBoxHoverHint, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(renderSvc, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Mouse));

    // Generate (mx, my) strictly inside DrawCardHitBox
    auto mx = *rc::gen::inRange(50, 250);
    auto my = *rc::gen::inRange(400, 700);
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    // Must draw card hint text
    EXPECT_CALL(renderSvc, DrawString(20, 20, ::testing::StrEq(L"繧ｯ繝ｪ繝・け縺励※繧ｫ繝ｼ繝峨ｒ蠑輔￥"), 0xFFFF00u))
        .Times(1);

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// Task 6.4 窶・PBT-4: HintArea 蜈育捩荳閾ｴ・域怙螟ｧ 1 繝偵Φ繝茨ｼ・
// Property 4: HintArea 蜈育捩荳閾ｴ・域怙螟ｧ1繝偵Φ繝茨ｼ・
// Validates: Requirements 4.1, 4.4, 1.3
// ---------------------------------------------------------------------------
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property4_HintAreaAtMostOneDrawString, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(renderSvc, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Mouse));

    auto mx = *rc::gen::inRange(0, 1920);
    auto my = *rc::gen::inRange(0, 1080);
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    // Count DrawString calls at (HINT_X=20, HINT_Y=20) with COLOR_HOVER
    int hintDrawCount = 0;
    ON_CALL(renderSvc, DrawString(20, 20, ::testing::_, 0xFFFF00u))
        .WillByDefault([&hintDrawCount](int, int, const wchar_t*, uint32_t) {
            ++hintDrawCount;
        });

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);

    RC_ASSERT(hintDrawCount <= 1);
}

// ---------------------------------------------------------------------------
// Task 2.2 窶・PBT-5: GamepadMode 邏皮ｲ区ｧ
// Property 5: GamepadMode 邏皮ｲ区ｧ
// Validates: Requirements 5.1, 6.3
// ---------------------------------------------------------------------------
RC_GTEST_PROP(ControlDisplayerMouseHintsProps, Property5_GamepadPurity, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(renderSvc, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(inputSvc,  GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Gamepad));

    auto mx = *rc::gen::arbitrary<int>();
    auto my = *rc::gen::arbitrary<int>();
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    // Gamepad mode must draw all 4 gamepad icons
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_DPAD_HORIZONTAL)).Times(1);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_A)).Times(1);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::XBOX_X)).Times(1);
    EXPECT_CALL(assetSvc, GetImageHandle(mc::EImage::BUTTON_MENU)).Times(1);

    // Gamepad mode must NOT call DrawHollowBox or HintArea DrawString
    EXPECT_CALL(renderSvc, DrawHollowBox(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);
    EXPECT_CALL(renderSvc, DrawString(20, 20, ::testing::_, ::testing::_)).Times(0);

    auto disp = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    disp->Draw(0.0f);
}

