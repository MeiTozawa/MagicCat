// Tests for ExitPanel
// Requirements: 6.1, 6.2, 6.3

#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <RenderUtils.h>
#include "MockServices.h"

import Displayer;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;
using ::testing::StrictMock;

namespace mc {
namespace {

// -----------------------------------------------------------------------
// Test: IsPanelFocus() always returns false
// (ExitPanel does not override IsPanelFocus, so the base MenuPanel
//  default of false is always returned.)
// Validates: Requirement 6.1
// -----------------------------------------------------------------------
TEST(ExitPanelTest, IsPanelFocusAlwaysFalse)
{
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;

    auto panel = CreateExitPanel(mockInput, mockRender);

    EXPECT_FALSE(panel->IsPanelFocus());

    // Still false after an update with no input
    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    panel->Update(0.0f);

    EXPECT_FALSE(panel->IsPanelFocus());
}

// -----------------------------------------------------------------------
// Test: OnUpdate calls renderService.ExitApplication() when
//       IsPressed(Confirm) is true
// Validates: Requirement 6.2
// -----------------------------------------------------------------------
TEST(ExitPanelTest, OnUpdateCallsExitApplicationWhenConfirmPressed)
{
    NiceMock<MockInputService>  mockInput;
    StrictMock<MockRenderService> mockRender;

    ON_CALL(mockInput, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));

    // ExitApplication must be called exactly once
    EXPECT_CALL(mockRender, ExitApplication()).Times(1);

    auto panel = CreateExitPanel(mockInput, mockRender);
    panel->Update(0.0f);
}

// -----------------------------------------------------------------------
// Test: OnUpdate does NOT call renderService.ExitApplication() when
//       IsPressed(Confirm) is false
// Validates: Requirement 6.2
// -----------------------------------------------------------------------
TEST(ExitPanelTest, OnUpdateDoesNotCallExitApplicationWhenConfirmNotPressed)
{
    NiceMock<MockInputService>  mockInput;
    StrictMock<MockRenderService> mockRender;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));

    // ExitApplication must never be called
    EXPECT_CALL(mockRender, ExitApplication()).Times(0);

    auto panel = CreateExitPanel(mockInput, mockRender);
    panel->Update(0.0f);
}

// -----------------------------------------------------------------------
// Test: OnDraw renders both Japanese text lines via IRenderService
// Validates: Requirement 6.3
// -----------------------------------------------------------------------
TEST(ExitPanelTest, OnDrawRendersBothJapaneseTextLines)
{
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;

    bool sawFirstLine  = false;
    bool sawSecondLine = false;

    ON_CALL(mockRender, DrawCenterString(_, _, testing::StrEq(L"ゲームを終了しますか？"), _))
        .WillByDefault([&](int, int, const wchar_t*, uint32_t)
        {
            sawFirstLine = true;
        });

    ON_CALL(mockRender, DrawCenterString(_, _, testing::StrEq(L"保存されていない内容は失われます！"), _))
        .WillByDefault([&](int, int, const wchar_t*, uint32_t)
        {
            sawSecondLine = true;
        });

    auto panel = CreateExitPanel(mockInput, mockRender);

    // Disable input so Update does not trigger ExitApplication during Draw
    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));

    panel->Draw(0.0f);

    EXPECT_TRUE(sawFirstLine)  << "Expected DrawCenterString call for L\"ゲームを終了しますか？\"";
    EXPECT_TRUE(sawSecondLine) << "Expected DrawCenterString call for L\"保存されていない内容は失われます！\"";
}

// -----------------------------------------------------------------------
// Test: OnDraw renders second line with COLOR_TEXT_RED
// Validates: Requirement 6.3
// -----------------------------------------------------------------------
TEST(ExitPanelTest, OnDrawRendersSecondLineWithRedColor)
{
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;

    uint32_t capturedColor = 0;

    ON_CALL(mockRender, DrawCenterString(_, _, testing::StrEq(L"保存されていない内容は失われます！"), _))
        .WillByDefault([&](int, int, const wchar_t*, uint32_t color)
        {
            capturedColor = color;
        });

    auto panel = CreateExitPanel(mockInput, mockRender);

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));

    panel->Draw(0.0f);

    EXPECT_EQ(capturedColor, COLOR_TEXT_RED)
        << "Second line should be rendered with COLOR_TEXT_RED";
}

} // namespace
} // namespace mc
