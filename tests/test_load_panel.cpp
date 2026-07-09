// Feature: menu-scene-refactor, Property 6: LoadPanel 加载結果決定場景跳転
// Task 5.3 + 5.4
// Validates: Requirements 5.1, 5.2, 5.3, 5.4

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

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;

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
// Fixture — holds all mocks needed for CreateLoadPanel / CreateSavePanel
// -----------------------------------------------------------------------
struct LoadPanelFixture
{
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockBattleService> mockBattle;
    NiceMock<MockConfigService> mockConfig;
    NiceMock<MockSceneService>  mockScene;
    NiceMock<MockOSService>     mockOs;

    LoadPanelFixture()
    {
        ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
        ON_CALL(mockInput, IsHolding(_)).WillByDefault(Return(false));
        ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
        ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));

        ON_CALL(mockConfig, GetSaveMetadata(_)).WillByDefault(Return(SaveMetadata{}));
    }

    std::unique_ptr<MenuPanel> makeLoad()
    {
        return CreateLoadPanel(mockInput, mockRender, mockBattle,
                               mockConfig, mockScene, mockOs);
    }

    std::unique_ptr<MenuPanel> makeSave()
    {
        return CreateSavePanel(mockInput, mockRender, mockBattle,
                               mockConfig, mockScene, mockOs);
    }
};

// -----------------------------------------------------------------------
// Helper: navigate to slot i (Down i times) then press Confirm once
// -----------------------------------------------------------------------
static void NavigateToSlotAndConfirm(MenuPanel& panel,
                                     NiceMock<MockInputService>& input,
                                     int targetSlot)
{
    ON_CALL(input, IsPressed(InputAction::Down)).WillByDefault(Return(true));
    for (int i = 0; i < targetSlot; ++i)
        panel.Update(0.0f);
    ON_CALL(input, IsPressed(InputAction::Down)).WillByDefault(Return(false));

    ON_CALL(input, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    panel.Update(0.0f);
    ON_CALL(input, IsPressed(InputAction::Confirm)).WillByDefault(Return(false));
}

// =======================================================================
// Property 6: LoadPanel 加载結果決定場景跳転
//
// For any slot i ∈ [0, SAVE_SLOT_COUNT):
//   • LoadState(i) returns true  → SetCurrentScene(ESceneState::Combat) called
//   • LoadState(i) returns false → SetCurrentScene never called
//
// Validates: Requirements 5.2, 5.3
// =======================================================================

// Feature: menu-scene-refactor, Property 6: LoadPanel 加載結果決定場景跳転
RC_GTEST_PROP(LoadPanelProperties, Property6_LoadTrue_SceneJumpsToCombat, ())
{
    const int slot = *rc::gen::inRange(0, SAVE_SLOT_COUNT);

    LoadPanelFixture f;
    ON_CALL(f.mockBattle, LoadState(slot)).WillByDefault(Return(true));

    EXPECT_CALL(f.mockScene, SetCurrentScene(ESceneState::Combat)).Times(1);

    auto panel = f.makeLoad();
    NavigateToSlotAndConfirm(*panel, f.mockInput, slot);
}

// Feature: menu-scene-refactor, Property 6: LoadPanel 加載結果決定場景跳転
RC_GTEST_PROP(LoadPanelProperties, Property6_LoadFalse_NoSceneJump, ())
{
    const int slot = *rc::gen::inRange(0, SAVE_SLOT_COUNT);

    LoadPanelFixture f;
    ON_CALL(f.mockBattle, LoadState(slot)).WillByDefault(Return(false));

    EXPECT_CALL(f.mockScene, SetCurrentScene(_)).Times(0);

    auto panel = f.makeLoad();
    NavigateToSlotAndConfirm(*panel, f.mockInput, slot);
}

// =======================================================================
// Example-based unit tests
// =======================================================================

TEST(LoadPanelTest, LoadState_Slot0_True_CallsSetCurrentScene)
{
    LoadPanelFixture f;
    ON_CALL(f.mockBattle, LoadState(0)).WillByDefault(Return(true));
    EXPECT_CALL(f.mockScene, SetCurrentScene(ESceneState::Combat)).Times(1);

    auto panel = f.makeLoad();
    ON_CALL(f.mockInput, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    panel->Update(0.0f);
}

TEST(LoadPanelTest, LoadState_Slot0_False_DoesNotCallSetCurrentScene)
{
    LoadPanelFixture f;
    ON_CALL(f.mockBattle, LoadState(0)).WillByDefault(Return(false));
    EXPECT_CALL(f.mockScene, SetCurrentScene(_)).Times(0);

    auto panel = f.makeLoad();
    ON_CALL(f.mockInput, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    panel->Update(0.0f);
}

TEST(LoadPanelTest, LoadState_CorrectSlotIndex_Passed)
{
    for (int targetSlot = 0; targetSlot < SAVE_SLOT_COUNT; ++targetSlot)
    {
        LoadPanelFixture f;
        ON_CALL(f.mockBattle, LoadState(_)).WillByDefault(Return(false));
        EXPECT_CALL(f.mockBattle, LoadState(targetSlot)).Times(1);

        auto panel = f.makeLoad();
        NavigateToSlotAndConfirm(*panel, f.mockInput, targetSlot);
    }
}

// -----------------------------------------------------------------------
// Task 5.4: CreateLoadPanel calls GetSaveMetadata for every slot on
// construction (Load mode triggers RefreshSlotMeta in the constructor).
// Validates: Requirements 5.1, 5.4
// -----------------------------------------------------------------------
TEST(LoadPanelTest, Construction_CallsGetSaveMetadata_ForAllSlots)
{
    LoadPanelFixture f;

    for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
    {
        EXPECT_CALL(f.mockConfig, GetSaveMetadata(i))
            .Times(1)
            .WillOnce(Return(SaveMetadata{}));
    }

    auto panel = f.makeLoad();
}

TEST(LoadPanelTest, SavePanel_Construction_DoesNotCallGetSaveMetadata)
{
    LoadPanelFixture f;

    EXPECT_CALL(f.mockConfig, GetSaveMetadata(_)).Times(0);

    auto panel = f.makeSave();
}

} // namespace
} // namespace mc
