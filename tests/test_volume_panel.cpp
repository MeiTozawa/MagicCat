// Feature: menu-scene-refactor, Property 3: VolumePanel 音量直接設定

#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include "MockServices.h"

import Displayer;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;

namespace mc {
namespace {

class VolumePanelFixture {
public:
    NiceMock<MockInputService> mockInput;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockAudioService> mockAudio;
    NiceMock<MockOSService> mockOs;

    VolumePanelFixture() {
        ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
        ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
        ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    }

    std::unique_ptr<MenuPanel> make() {
        return CreateVolumePanel(mockInput, mockRender, mockAudio, mockOs);
    }
};

void PressConfirm(MenuPanel& panel, NiceMock<MockInputService>& input)
{
    ON_CALL(input, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    panel.Update(0.0f);
    ON_CALL(input, IsPressed(InputAction::Confirm)).WillByDefault(Return(false));
}

// -----------------------------------------------------------------------
// Property 3: VolumePanel 音量直接設定
// 行 row を確認して値微調状態に入り、オプション j を選択して Confirm すると
// Set*Volume(j) が呼ばれることを検証する。
// -----------------------------------------------------------------------
RC_GTEST_PROP(VolumePanelProperties, Property3_ConfirmOption_SetsVolumeDirectly, ())
{
    const int row = *rc::gen::inRange(0, 3);
    const int targetLevel = *rc::gen::inRange(0, 5);

    VolumePanelFixture f;
    ON_CALL(f.mockAudio, GetMasterVolume()).WillByDefault(Return(2));
    ON_CALL(f.mockAudio, GetBgmVolume()).WillByDefault(Return(2));
    ON_CALL(f.mockAudio, GetSfxVolume()).WillByDefault(Return(2));

    // Move to row
    ON_CALL(f.mockInput, IsPressed(InputAction::Down)).WillByDefault(Return(true));
    auto panel = f.make();
    for (int i = 0; i < row; ++i)
        panel->Update(0.0f);
    ON_CALL(f.mockInput, IsPressed(InputAction::Down)).WillByDefault(Return(false));

    PressConfirm(*panel, f.mockInput); // enter tweak state

    // Navigate to targetLevel in secondaryGroup (start index is 2)
    if (targetLevel < 2)
    {
        ON_CALL(f.mockInput, IsPressed(InputAction::Left)).WillByDefault(Return(true));
        for (int i = 0; i < 2 - targetLevel; ++i)
            panel->Update(0.0f);
        ON_CALL(f.mockInput, IsPressed(InputAction::Left)).WillByDefault(Return(false));
    }
    else if (targetLevel > 2)
    {
        ON_CALL(f.mockInput, IsPressed(InputAction::Right)).WillByDefault(Return(true));
        for (int i = 0; i < targetLevel - 2; ++i)
            panel->Update(0.0f);
        ON_CALL(f.mockInput, IsPressed(InputAction::Right)).WillByDefault(Return(false));
    }

    if (row == 0)
        EXPECT_CALL(f.mockAudio, SetMasterVolume(targetLevel)).Times(1);
    else if (row == 1)
        EXPECT_CALL(f.mockAudio, SetBgmVolume(targetLevel)).Times(1);
    else
        EXPECT_CALL(f.mockAudio, SetSfxVolume(targetLevel)).Times(1);

    PressConfirm(*panel, f.mockInput);
}

// -----------------------------------------------------------------------
// Property 4: VolumePanel 値微調退出不改変行焦点
// 値微調状態で ToggleMenu を押すと値微調を抜ける。
// -----------------------------------------------------------------------
RC_GTEST_PROP(VolumePanelProperties, Property4_ExitTweak_PanelStaysActive, ())
{
    const int row = *rc::gen::inRange(0, 3);

    VolumePanelFixture f;
    ON_CALL(f.mockInput, IsPressed(InputAction::Down)).WillByDefault(Return(true));
    auto panel = f.make();
    for (int i = 0; i < row; ++i)
        panel->Update(0.0f);
    ON_CALL(f.mockInput, IsPressed(InputAction::Down)).WillByDefault(Return(false));

    PressConfirm(*panel, f.mockInput); // 値微調状態へ

    // ToggleMenu で値微調を抜ける
    ON_CALL(f.mockInput, IsPressed(InputAction::ToggleMenu)).WillByDefault(Return(true));
    panel->Update(0.0f);
    ON_CALL(f.mockInput, IsPressed(InputAction::ToggleMenu)).WillByDefault(Return(false));
}

RC_GTEST_PROP(VolumePanelProperties, Property4_ExitTweak_PopContextCalled, ())
{
    const int row = *rc::gen::inRange(0, 3);

    VolumePanelFixture f;
    ON_CALL(f.mockInput, IsPressed(InputAction::Down)).WillByDefault(Return(true));
    auto panel = f.make();
    for (int i = 0; i < row; ++i)
        panel->Update(0.0f);
    ON_CALL(f.mockInput, IsPressed(InputAction::Down)).WillByDefault(Return(false));

    // PushContext は値微調突入時に 1 回呼ばれるはず
    EXPECT_CALL(f.mockInput, PushContext(InputContext::VolumeControl)).Times(1);
    PressConfirm(*panel, f.mockInput);

    // ToggleMenu で抜ける → PopContext が 1 回呼ばれるはず
    EXPECT_CALL(f.mockInput, PopContext()).Times(1);
    ON_CALL(f.mockInput, IsPressed(InputAction::ToggleMenu)).WillByDefault(Return(true));
    panel->Update(0.0f);
    ON_CALL(f.mockInput, IsPressed(InputAction::ToggleMenu)).WillByDefault(Return(false));
}

// -----------------------------------------------------------------------
// 例示ベーステスト
// -----------------------------------------------------------------------

TEST(VolumePanelTest, DestructFromTweakState_PopContextCalled)
{
    VolumePanelFixture f;
    {
        auto panel = f.make();
        // 行 0 を確認して値微調へ
        EXPECT_CALL(f.mockInput, PushContext(InputContext::VolumeControl)).Times(1);
        PressConfirm(*panel, f.mockInput);

        // Deactivate (destructor) → PopContext が呼ばれるはず
        EXPECT_CALL(f.mockInput, PopContext()).Times(1);
    }
}

TEST(VolumePanelTest, DestructFromRowSelect_PopContextNotCalled)
{
    VolumePanelFixture f;
    {
        auto panel = f.make();
        // 値微調に入らずそのまま終了 → PopContext 不要
        EXPECT_CALL(f.mockInput, PopContext()).Times(0);
    }
}

} // namespace
} // namespace mc
