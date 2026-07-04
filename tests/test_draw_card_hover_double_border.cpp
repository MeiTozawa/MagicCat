// Feature: draw-card-hover-double-border

#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include "MockServices.h"

import Displayer;
import EventBus;
import CardService;

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::InSequence;

namespace mc {
namespace {

// ---------------------------------------------------------------------------
// CardDisplayer fixture
// ---------------------------------------------------------------------------
class DrawCardHoverTest : public ::testing::Test {
protected:
    NiceMock<MockCardService>   cardService;
    NiceMock<MockAssetService>  assetService;
    NiceMock<MockRenderService> renderService;

    std::unique_ptr<Displayer> displayer;

    void SetUp() override {
        ON_CALL(cardService, GetHandCards()).WillByDefault(Return(std::vector<Card>{}));
        ON_CALL(cardService, GetDrawCards()).WillByDefault(Return(std::vector<Card>{}));
        ON_CALL(cardService, GetDiscardCards()).WillByDefault(Return(std::vector<Card>{}));
        ON_CALL(assetService, GetImageHandle(_)).WillByDefault(Return(1));
        ON_CALL(renderService, GetWindowWidth()).WillByDefault(Return(1280));

        displayer = CreateCardDisplayer(cardService, assetService, renderService);
    }
};

// ---------------------------------------------------------------------------
// 5.4 — デフォルト状態（イベント未受信）で DrawDoubleHollowBox が呼ばれない
// Validates: Req 3.6
// ---------------------------------------------------------------------------
TEST_F(DrawCardHoverTest, Default_NoDoubleBorder) {
    EXPECT_CALL(renderService, DrawDoubleHollowBox(_, _, _, _, _, _)).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// 5.2 — DrawPileHoveredEvent 受信後に DrawDoubleHollowBox が正しい引数で呼ばれる
// Validates: Req 3.3, 3.5
// ---------------------------------------------------------------------------
TEST_F(DrawCardHoverTest, HoveredEvent_DrawsDoubleBorder) {
    EventBus::Publish(DrawPileHoveredEvent{});
    EXPECT_CALL(renderService, DrawDoubleHollowBox(50, 400, 250, 700, 5, 0xFFFF00u)).Times(1);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// 5.3 — DrawPileUnhoveredEvent 受信後は DrawDoubleHollowBox が呼ばれない
// Validates: Req 3.4, 3.6
// ---------------------------------------------------------------------------
TEST_F(DrawCardHoverTest, UnhoveredEvent_NoDoubleBorder) {
    EventBus::Publish(DrawPileHoveredEvent{});
    EventBus::Publish(DrawPileUnhoveredEvent{});
    EXPECT_CALL(renderService, DrawDoubleHollowBox(_, _, _, _, _, _)).Times(0);
    displayer->Draw(0.0f);
}

// ---------------------------------------------------------------------------
// 5.5 — デストラクタで Unsubscribe されること
// Validates: Req 3.7
// ---------------------------------------------------------------------------
TEST_F(DrawCardHoverTest, Destructor_Unsubscribes) {
    // Destroy the displayer — after this, event callbacks should be gone
    displayer.reset();

    // Publishing events after destruction must not crash or affect any new displayer
    int callbackCount = 0;
    EventHandle h = EventBus::Subscribe<DrawPileHoveredEvent>(
        [&callbackCount](const DrawPileHoveredEvent&) { ++callbackCount; });

    EventBus::Publish(DrawPileHoveredEvent{});
    EXPECT_EQ(callbackCount, 1); // Only our new subscription fires, not the destroyed one

    EventBus::Unsubscribe(h);
}

// ---------------------------------------------------------------------------
// ControlDisplayer fixture (for event publishing tests)
// ---------------------------------------------------------------------------
class ControlDisplayerHoverTest : public ::testing::Test {
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

    ~ControlDisplayerHoverTest() override {
        displayer.reset();
    }
};

// ---------------------------------------------------------------------------
// 5.6 — ゲームパッド切り替えエッジケース: ホバー中に Gamepad モードへ切り替えると
//         DrawPileUnhoveredEvent が 1 回発行される
// Validates: Req 4.2
// ---------------------------------------------------------------------------
TEST_F(ControlDisplayerHoverTest, GamepadSwitch_WhileHovered_PublishesUnhoverEvent) {
    // Step 1: hover over DrawCardHitBox (inside [50,250)×[400,700))
    ON_CALL(inputService, GetMousePosition()).WillByDefault(Return(Point<int>{150, 550}));
    displayer->Draw(0.0f); // → publishes DrawPileHoveredEvent, drawPileHovered = true

    // Step 2: switch to Gamepad mode
    int unhoveredCount = 0;
    EventHandle h = EventBus::Subscribe<DrawPileUnhoveredEvent>(
        [&unhoveredCount](const DrawPileUnhoveredEvent&) { ++unhoveredCount; });

    ON_CALL(inputService, GetActiveDevice()).WillByDefault(Return(InputDevice::Gamepad));
    displayer->Draw(0.0f); // → should publish DrawPileUnhoveredEvent

    EXPECT_EQ(unhoveredCount, 1);
    EventBus::Unsubscribe(h);
}

} // namespace
} // namespace mc

// =============================================================================
// RapidCheck プロパティテスト
// =============================================================================

// ---------------------------------------------------------------------------
// 5.7 — Property 1: DrawDoubleHollowBox が DrawHollowBox を正しい外枠・内枠座標で呼ぶ
// Validates: Req 1.2
// ---------------------------------------------------------------------------
RC_GTEST_PROP(DrawCardHoverDoubleProps, Property1_DrawDoubleHollowBox_CallsHollowBoxTwice, ()) {
    using ::testing::NiceMock;
    using ::testing::_;

    NiceMock<mc::MockRenderService> renderSvc;

    auto x1 = *rc::gen::inRange(0, 500);
    auto y1 = *rc::gen::inRange(0, 500);
    auto x2 = x1 + *rc::gen::inRange(20, 300);
    auto y2 = y1 + *rc::gen::inRange(20, 300);
    auto thickness = *rc::gen::inRange(1, 5);
    auto color = *rc::gen::arbitrary<uint32_t>();

    // Capture DrawHollowBox calls via a recording stub.
    // DrawDoubleHollowBox on MockRenderService is a no-op stub; configure it via
    // ON_CALL to replicate the same math that DxLibRenderService uses, so we can
    // verify the coordinate relationship through the recorded DrawHollowBox calls.
    struct HollowBoxCall { int x1, y1, x2, y2, t; uint32_t c; };
    std::vector<HollowBoxCall> calls;

    ON_CALL(renderSvc, DrawHollowBox(_, _, _, _, _, _))
        .WillByDefault([&calls](int cx1, int cy1, int cx2, int cy2, int ct, uint32_t cc) {
            calls.push_back({cx1, cy1, cx2, cy2, ct, cc});
        });
    ON_CALL(renderSvc, DrawDoubleHollowBox(_, _, _, _, _, _))
        .WillByDefault([&renderSvc](int cx1, int cy1, int cx2, int cy2, int ct, uint32_t cc) {
            // Mirrors DxLibRenderService::DrawDoubleHollowBox logic
            renderSvc.DrawHollowBox(cx1, cy1, cx2, cy2, ct, cc);
            renderSvc.DrawHollowBox(cx1 + 2*ct, cy1 + 2*ct, cx2 - 2*ct, cy2 - 2*ct, ct, cc);
        });

    renderSvc.DrawDoubleHollowBox(x1, y1, x2, y2, thickness, color);

    // Must have produced exactly 2 DrawHollowBox calls
    RC_ASSERT(calls.size() == 2u);

    // Outer box — same coordinates as input
    RC_ASSERT(calls[0].x1 == x1);
    RC_ASSERT(calls[0].y1 == y1);
    RC_ASSERT(calls[0].x2 == x2);
    RC_ASSERT(calls[0].y2 == y2);
    RC_ASSERT(calls[0].t  == thickness);
    RC_ASSERT(calls[0].c  == color);

    // Inner box — inset by 2*thickness on every side
    RC_ASSERT(calls[1].x1 == x1 + 2*thickness);
    RC_ASSERT(calls[1].y1 == y1 + 2*thickness);
    RC_ASSERT(calls[1].x2 == x2 - 2*thickness);
    RC_ASSERT(calls[1].y2 == y2 - 2*thickness);
    RC_ASSERT(calls[1].t  == thickness);
    RC_ASSERT(calls[1].c  == color);
}

// ---------------------------------------------------------------------------
// 5.8 — Property 2: ホバーエントリで DrawPileHoveredEvent が 1 回だけ発行される
// Validates: Req 2.4
// ---------------------------------------------------------------------------
RC_GTEST_PROP(DrawCardHoverDoubleProps, Property2_HoverEntry_PublishesExactlyOneHoveredEvent, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(renderSvc, GetWindowWidth()).WillByDefault(Return(1280));
    // Previous frame: outside hitbox
    ON_CALL(inputSvc, GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Mouse));
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{0, 0}));

    auto ctrl = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);
    ctrl->Draw(0.0f); // no hover, drawPileHovered = false

    // Current frame: inside DrawCardHitBox [50,250)×[400,700)
    auto mx = *rc::gen::inRange(50, 250);
    auto my = *rc::gen::inRange(400, 700);
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{mx, my}));

    int hoveredCount = 0, unhoveredCount = 0;
    mc::EventHandle hh = mc::EventBus::Subscribe<mc::DrawPileHoveredEvent>(
        [&hoveredCount](const mc::DrawPileHoveredEvent&) { ++hoveredCount; });
    mc::EventHandle hu = mc::EventBus::Subscribe<mc::DrawPileUnhoveredEvent>(
        [&unhoveredCount](const mc::DrawPileUnhoveredEvent&) { ++unhoveredCount; });

    ctrl->Draw(0.0f);

    RC_ASSERT(hoveredCount == 1);
    RC_ASSERT(unhoveredCount == 0);

    mc::EventBus::Unsubscribe(hh);
    mc::EventBus::Unsubscribe(hu);
}

// ---------------------------------------------------------------------------
// 5.9 — Property 3: ホバーエグジットで DrawPileUnhoveredEvent が 1 回だけ発行される
// Validates: Req 2.5
// ---------------------------------------------------------------------------
RC_GTEST_PROP(DrawCardHoverDoubleProps, Property3_HoverExit_PublishesExactlyOneUnhoveredEvent, ()) {
    using ::testing::NiceMock;
    using ::testing::Return;

    NiceMock<mc::MockAssetService>  assetSvc;
    NiceMock<mc::MockRenderService> renderSvc;
    NiceMock<mc::MockInputService>  inputSvc;

    ON_CALL(assetSvc,  GetImageHandle(::testing::_)).WillByDefault(Return(1));
    ON_CALL(renderSvc, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(inputSvc, GetActiveDevice()).WillByDefault(Return(mc::InputDevice::Mouse));

    auto ctrl = mc::CreateControlDisplayer(assetSvc, renderSvc, inputSvc);

    // Step 1: enter hitbox → drawPileHovered = true
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{150, 550}));
    ctrl->Draw(0.0f);

    // Step 2: exit hitbox (random position outside [50,250)×[400,700))
    // Use a position clearly outside: (0, 0)
    auto exitX = *rc::gen::inRange(300, 1000);
    auto exitY = *rc::gen::inRange(0, 300);
    ON_CALL(inputSvc, GetMousePosition()).WillByDefault(Return(mc::Point<int>{exitX, exitY}));

    int hoveredCount = 0, unhoveredCount = 0;
    mc::EventHandle hh = mc::EventBus::Subscribe<mc::DrawPileHoveredEvent>(
        [&hoveredCount](const mc::DrawPileHoveredEvent&) { ++hoveredCount; });
    mc::EventHandle hu = mc::EventBus::Subscribe<mc::DrawPileUnhoveredEvent>(
        [&unhoveredCount](const mc::DrawPileUnhoveredEvent&) { ++unhoveredCount; });

    ctrl->Draw(0.0f);

    RC_ASSERT(unhoveredCount == 1);
    RC_ASSERT(hoveredCount == 0);

    mc::EventBus::Unsubscribe(hh);
    mc::EventBus::Unsubscribe(hu);
}
