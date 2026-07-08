// Feature: menu-scene-refactor, Property 1: RulesPanel 翻页回绕

#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include "MockServices.h"

import Displayer;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;

namespace mc {
namespace {

// -----------------------------------------------------------------------
// Helper: advance the panel N pages via Confirm presses
// -----------------------------------------------------------------------
void AdvancePages(MenuPanel& panel, NiceMock<MockInputService>& mockInput, int n)
{
    ON_CALL(mockInput, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    for (int i = 0; i < n; ++i)
        panel.Update(0.0f);
}

// -----------------------------------------------------------------------
// Property 1: RulesPanel 翻页回绕
// For any startPage in [0, PAGE_COUNT), after one more Confirm press
// the panel wraps to (startPage + 1) % PAGE_COUNT.
// Verified by navigating to page 0 and checking Draw output.
// -----------------------------------------------------------------------
// Feature: menu-scene-refactor, Property 1: RulesPanel 翻页回绕
RC_GTEST_PROP(RulesPanelProperties, Property1_PageWrapAround, ())
{
    constexpr int PAGE_COUNT = 3;
    const int startPage = *rc::gen::inRange(0, PAGE_COUNT);

    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockAssetService>  mockAsset;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));

    auto panel = CreateRulesPanel(100, 215, mockInput, mockRender, mockAsset);

    // Advance to startPage
    AdvancePages(*panel, mockInput, startPage);

    // One more press → expected page = (startPage + 1) % PAGE_COUNT
    ON_CALL(mockInput, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    panel->Update(0.0f);

    // Navigate to page 0: press (PAGE_COUNT - expectedPage) % PAGE_COUNT more times
    const int expectedPage    = (startPage + 1) % PAGE_COUNT;
    const int pressesToReach0 = (PAGE_COUNT - expectedPage) % PAGE_COUNT;
    AdvancePages(*panel, mockInput, pressesToReach0);

    // Turn off input so Draw doesn't flip the page
    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));

    // Verify page-0 first-line text is rendered
    bool sawPage0FirstLine = false;
    ON_CALL(mockRender, DrawString(_, _, testing::StrEq(
        L"アクション: 【カードを引く】【魔法を使用する】【攻撃する】"), _))
        .WillByDefault([&](int, int, const wchar_t*, uint32_t)
        {
            sawPage0FirstLine = true;
        });

    panel->Draw(0.0f);
    RC_ASSERT(sawPage0FirstLine);
}

// -----------------------------------------------------------------------
// Example-based unit tests
// -----------------------------------------------------------------------

TEST(RulesPanelTest, IsActiveAlwaysFalse)
{
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockAssetService>  mockAsset;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));

    auto panel = CreateRulesPanel(100, 215, mockInput, mockRender, mockAsset);
    EXPECT_FALSE(panel->IsActive());

    panel->Activate();
    EXPECT_FALSE(panel->IsActive());

    ON_CALL(mockInput, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    panel->Update(0.0f);
    EXPECT_FALSE(panel->IsActive());
}

TEST(RulesPanelTest, ActivateDeactivateAreNoOps)
{
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockAssetService>  mockAsset;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));

    auto panel = CreateRulesPanel(100, 215, mockInput, mockRender, mockAsset);

    panel->Activate();
    EXPECT_FALSE(panel->IsActive());
    panel->Deactivate();
    EXPECT_FALSE(panel->IsActive());
}

TEST(RulesPanelTest, PageWrapsAt3To0)
{
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockAssetService>  mockAsset;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));

    auto panel = CreateRulesPanel(100, 215, mockInput, mockRender, mockAsset);

    // Press Confirm 3 times — wraps back to page 0
    ON_CALL(mockInput, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    panel->Update(0.0f); // 0 → 1
    panel->Update(0.0f); // 1 → 2
    panel->Update(0.0f); // 2 → 0

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));

    bool sawPage0FirstLine = false;
    ON_CALL(mockRender, DrawString(_, _, testing::StrEq(
        L"アクション: 【カードを引く】【魔法を使用する】【攻撃する】"), _))
        .WillByDefault([&](int, int, const wchar_t*, uint32_t)
        {
            sawPage0FirstLine = true;
        });

    panel->Draw(0.0f);
    EXPECT_TRUE(sawPage0FirstLine);
}

} // namespace
} // namespace mc
