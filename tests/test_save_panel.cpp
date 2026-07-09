// Feature: menu-scene-refactor, Property 5: SavePanel 確認触発保存並刷新元数据

#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include "MockServices.h"

import Displayer;
import SceneService;
import ConfigService;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;

namespace mc {
namespace {

// -----------------------------------------------------------------------
// Local mock for ISceneService (not included in MockServices.h)
// -----------------------------------------------------------------------
class MockSceneService : public ISceneService {
public:
    MOCK_METHOD(void, Update, (float), (override));
    MOCK_METHOD(void, PushScene, (ESceneState), (override));
    MOCK_METHOD(void, PopScene, (), (override));
    MOCK_METHOD(void, RegisterScene, (ESceneState, std::unique_ptr<IScene>&&), (override));
    MOCK_METHOD(ESceneState, GetCurrentScene, (), (override));
    MOCK_METHOD(void, SetCurrentScene, (ESceneState), (override));
};

// -----------------------------------------------------------------------
// Fixture
// -----------------------------------------------------------------------
class SavePanelFixture {
public:
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockBattleService> mockBattle;
    NiceMock<MockConfigService> mockConfig;
    NiceMock<MockSceneService>  mockScene;
    NiceMock<MockOSService>     mockOs;

    SavePanelFixture()
    {
        ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
        ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
        ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));

        // Default: SaveState returns false (no metadata refresh)
        ON_CALL(mockBattle, SaveState(_)).WillByDefault(Return(false));

        // GetSaveMetadata returns an empty (non-existing) slot by default
        ON_CALL(mockConfig, GetSaveMetadata(_)).WillByDefault(Return(SaveMetadata{}));
    }

    std::unique_ptr<MenuPanel> make()
    {
        return CreateSavePanel(mockInput, mockRender, mockBattle, mockConfig, mockScene, mockOs);
    }
};

// Helper: navigate to slot i (slotGroup starts at index 0)
void NavigateToSlot(MenuPanel& panel, NiceMock<MockInputService>& input, int slot)
{
    if (slot == 0) return; // already at 0
    ON_CALL(input, IsPressed(InputAction::Down)).WillByDefault(Return(true));
    for (int i = 0; i < slot; ++i)
        panel.Update(0.0f);
    ON_CALL(input, IsPressed(InputAction::Down)).WillByDefault(Return(false));
}

// Helper: fire exactly one Confirm press
void PressConfirm(MenuPanel& panel, NiceMock<MockInputService>& input)
{
    ON_CALL(input, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    panel.Update(0.0f);
    ON_CALL(input, IsPressed(InputAction::Confirm)).WillByDefault(Return(false));
}

// -----------------------------------------------------------------------
// Property 5: SavePanel 確認触発保存並刷新元数据
// Validates: Requirements 4.2, 4.3, 8.4
//
// For any slot i ∈ [0, SAVE_SLOT_COUNT):
//   confirm on slot i  →  battleService.SaveState(i) called exactly once
//   if SaveState returns true  →  configService.GetSaveMetadata called for
//   all SAVE_SLOT_COUNT slots (RefreshSlotMeta)
// -----------------------------------------------------------------------

// Sub-property 5a: SaveState(i) is called for the confirmed slot
RC_GTEST_PROP(SavePanelProperties, Property5a_ConfirmCallsSaveState, ())
{
    const int slot = *rc::gen::inRange(0, SAVE_SLOT_COUNT);

    SavePanelFixture f;

    EXPECT_CALL(f.mockBattle, SaveState(slot)).Times(1).WillOnce(Return(false));

    auto panel = f.make();
    NavigateToSlot(*panel, f.mockInput, slot);
    PressConfirm(*panel, f.mockInput);
}

// Sub-property 5b: when SaveState returns true, GetSaveMetadata is called
//                  for every slot (RefreshSlotMeta)
RC_GTEST_PROP(SavePanelProperties, Property5b_SaveSuccessRefreshesAllSlots, ())
{
    const int slot = *rc::gen::inRange(0, SAVE_SLOT_COUNT);

    SavePanelFixture f;

    ON_CALL(f.mockBattle, SaveState(slot)).WillByDefault(Return(true));

    // GetSaveMetadata must be called for all SAVE_SLOT_COUNT slots
    for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
        EXPECT_CALL(f.mockConfig, GetSaveMetadata(i)).Times(AtLeast(1));

    auto panel = f.make();
    NavigateToSlot(*panel, f.mockInput, slot);
    PressConfirm(*panel, f.mockInput);
}

// Sub-property 5c: when SaveState returns false, GetSaveMetadata is NOT called
RC_GTEST_PROP(SavePanelProperties, Property5c_SaveFailDoesNotRefreshMeta, ())
{
    const int slot = *rc::gen::inRange(0, SAVE_SLOT_COUNT);

    SavePanelFixture f;

    ON_CALL(f.mockBattle, SaveState(slot)).WillByDefault(Return(false));
    EXPECT_CALL(f.mockConfig, GetSaveMetadata(_)).Times(0);

    auto panel = f.make();
    NavigateToSlot(*panel, f.mockInput, slot);
    PressConfirm(*panel, f.mockInput);
}

// -----------------------------------------------------------------------
// Example-based unit tests
// -----------------------------------------------------------------------

TEST(SavePanelTest, ConfirmSlot0_CallsSaveState0)
{
    SavePanelFixture f;
    EXPECT_CALL(f.mockBattle, SaveState(0)).Times(1).WillOnce(Return(false));

    auto panel = f.make();
    PressConfirm(*panel, f.mockInput);
}

TEST(SavePanelTest, ConfirmSlot2_CallsSaveState2)
{
    SavePanelFixture f;
    EXPECT_CALL(f.mockBattle, SaveState(2)).Times(1).WillOnce(Return(false));

    auto panel = f.make();
    NavigateToSlot(*panel, f.mockInput, 2);
    PressConfirm(*panel, f.mockInput);
}

TEST(SavePanelTest, SaveSuccessOnSlot1_RefreshesAllSlotMeta)
{
    SavePanelFixture f;
    ON_CALL(f.mockBattle, SaveState(1)).WillByDefault(Return(true));

    for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
        EXPECT_CALL(f.mockConfig, GetSaveMetadata(i)).Times(1);

    auto panel = f.make();
    NavigateToSlot(*panel, f.mockInput, 1);
    PressConfirm(*panel, f.mockInput);
}

TEST(SavePanelTest, SaveFailOnSlot3_DoesNotRefreshMeta)
{
    SavePanelFixture f;
    ON_CALL(f.mockBattle, SaveState(3)).WillByDefault(Return(false));
    EXPECT_CALL(f.mockConfig, GetSaveMetadata(_)).Times(0);

    auto panel = f.make();
    NavigateToSlot(*panel, f.mockInput, 3);
    PressConfirm(*panel, f.mockInput);
}

} // namespace
} // namespace mc
