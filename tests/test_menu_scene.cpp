// Tests for MenuScene routing logic
// Requirements: 7.2, 7.3, 7.4, 7.5, 8.1, 8.2

#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockServices.h"

import Displayer;
import SceneService;

using ::testing::NiceMock;
using ::testing::StrictMock;
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
// Fixture — holds all 8 services needed by CreateMenuScene
// -----------------------------------------------------------------------
class MenuSceneFixture {
public:
    NiceMock<MockInputService>  mockInput;
    NiceMock<MockSceneService>  mockScene;
    NiceMock<MockAssetService>  mockAsset;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockAudioService>  mockAudio;
    NiceMock<MockConfigService> mockConfig;
    NiceMock<MockBattleService> mockBattle;
    NiceMock<MockOSService>     mockOs;

    MenuSceneFixture()
    {
        // Default: no keys pressed, mouse off-screen
        ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
        ON_CALL(mockInput, IsHolding(_)).WillByDefault(Return(false));
        ON_CALL(mockInput, IsReleased(_)).WillByDefault(Return(false));
        ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
        ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
        ON_CALL(mockInput, GetActiveDevice()).WillByDefault(Return(InputDevice::Keyboard));

        // Load/Save metadata defaults
        ON_CALL(mockConfig, GetSaveMetadata(_)).WillByDefault(Return(SaveMetadata{}));
        ON_CALL(mockBattle, SaveState(_)).WillByDefault(Return(false));
        ON_CALL(mockBattle, LoadState(_)).WillByDefault(Return(false));
    }

    std::unique_ptr<IScene> make()
    {
        return CreateMenuScene(mockInput, mockScene, mockAsset, mockRender,
                               mockAudio, mockConfig, mockBattle, mockOs);
    }

    // Creates the scene and calls Start() — sets up PushContext + initial panel
    std::unique_ptr<IScene> makeAndStart()
    {
        auto scene = make();
        scene->Start();
        return scene;
    }
};

// Helper: press Right once to move button focus to the next index
static void PressRight(NiceMock<MockInputService>& input, IScene& scene)
{
    ON_CALL(input, IsPressed(InputAction::Right)).WillByDefault(Return(true));
    scene.Update(0.0f);
    ON_CALL(input, IsPressed(InputAction::Right)).WillByDefault(Return(false));
}

// Helper: press Confirm once
static void PressConfirm(NiceMock<MockInputService>& input, IScene& scene)
{
    ON_CALL(input, IsPressed(InputAction::Confirm)).WillByDefault(Return(true));
    scene.Update(0.0f);
    ON_CALL(input, IsPressed(InputAction::Confirm)).WillByDefault(Return(false));
}

// -----------------------------------------------------------------------
// Test 1: Start() calls PushContext(InputContext::Menu)
// Validates: Requirements 8.1
// -----------------------------------------------------------------------
TEST(MenuSceneTest, Start_PushesMenuContext)
{
    MenuSceneFixture f;
    EXPECT_CALL(f.mockInput, PushContext(InputContext::Menu)).Times(1);

    auto scene = f.make();
    scene->Start();
}

// -----------------------------------------------------------------------
// Test 2: ToggleMenu in base state → PopContext() + PopScene() called
// Validates: Requirements 7.5, 8.2
// -----------------------------------------------------------------------
TEST(MenuSceneTest, ToggleMenu_CallsPopContextAndPopScene)
{
    MenuSceneFixture f;

    EXPECT_CALL(f.mockInput, PopContext()).Times(1);
    EXPECT_CALL(f.mockScene, PopScene()).Times(1);

    auto scene = f.makeAndStart();

    ON_CALL(f.mockInput, IsPressed(InputAction::ToggleMenu)).WillByDefault(Return(true));
    scene->Update(0.0f);
}

// -----------------------------------------------------------------------
// Test 3: ToggleMenu does NOT continue updating after returning
//         (currentPanel->Update not called after ToggleMenu — no crash)
// Validates: Requirements 7.5
// -----------------------------------------------------------------------
TEST(MenuSceneTest, ToggleMenu_EarlyReturn_NoCrash)
{
    MenuSceneFixture f;
    auto scene = f.makeAndStart();

    // ToggleMenu → resets currentPanel; the scene should handle this gracefully
    ON_CALL(f.mockInput, IsPressed(InputAction::ToggleMenu)).WillByDefault(Return(true));
    ASSERT_NO_FATAL_FAILURE(scene->Update(0.0f));
}

// -----------------------------------------------------------------------
// Test 4: IsPanelFocus() == false → buttons.Update() IS called
//         Verified by: navigating Right (Horizontal ButtonGroup) while no
//         panel sub-focus → the focused index changes from 0 to 1,
//         triggering LoadPanel creation → GetSaveMetadata called
// Validates: Requirements 7.2, 7.3
// -----------------------------------------------------------------------
TEST(MenuSceneTest, NoPanelFocus_ButtonsUpdateCalled_NavigationChangesPanel)
{
    MenuSceneFixture f;

    // When focused index reaches 3 (Load), LoadPanel is created →
    // its constructor calls GetSaveMetadata for all slots
    EXPECT_CALL(f.mockConfig, GetSaveMetadata(_)).Times(AtLeast(1));

    auto scene = f.makeAndStart();

    // Navigate Right 3 times: index 0 → 1 → 2 → 3 (Load panel)
    PressRight(f.mockInput, *scene);   // → index 1
    PressRight(f.mockInput, *scene);   // → index 2
    PressRight(f.mockInput, *scene);   // → index 3 (Load)
}

// -----------------------------------------------------------------------
// Test 5: IsPanelFocus() == true → buttons.Update() NOT called
//         Verified by: entering VolumePanel tweak state (IsPanelFocus=true),
//         then pressing Right → focused index should NOT advance further
//         (no new LoadPanel → GetSaveMetadata not called again)
// Validates: Requirements 7.2
// -----------------------------------------------------------------------
TEST(MenuSceneTest, PanelFocus_ButtonsUpdateNotCalled_NavigationLocked)
{
    MenuSceneFixture f;

    // GetMasterVolume is queried by VolumePanel when entering tweak state
    ON_CALL(f.mockAudio, GetMasterVolume()).WillByDefault(Return(2));
    ON_CALL(f.mockAudio, GetBgmVolume()).WillByDefault(Return(2));
    ON_CALL(f.mockAudio, GetSfxVolume()).WillByDefault(Return(2));

    auto scene = f.makeAndStart();

    // Navigate to index 1 (VolumePanel) — it has IsPanelFocus()=false until tweak
    PressRight(f.mockInput, *scene);   // → index 1 (VolumePanel created)

    // Let VolumePanel update once normally (no input)
    scene->Update(0.0f);

    // Now press Confirm to enter VolumePanel's tweak state (IsPanelFocus becomes true)
    // VolumePanel row-select: Confirm → enters tweak, calls PushContext(VolumeControl)
    EXPECT_CALL(f.mockInput, PushContext(InputContext::VolumeControl)).Times(1);
    PressConfirm(f.mockInput, *scene);

    // Now IsPanelFocus() == true for VolumePanel
    // Pressing Right should NOT cause buttons.Update() to be called
    // → focused index stays at 1, no Load panel created → GetSaveMetadata NOT called
    EXPECT_CALL(f.mockConfig, GetSaveMetadata(_)).Times(0);

    PressRight(f.mockInput, *scene);
}

// -----------------------------------------------------------------------
// Test 6: Navigating to index 0 → RulesPanel (IsPanelFocus always false)
//         Confirmed by: no audio, battle, or config service calls on creation
// Validates: Requirements 7.3
// -----------------------------------------------------------------------
TEST(MenuSceneTest, ButtonIndex0_CreatesRulesPanel_NoPanelFocus)
{
    MenuSceneFixture f;

    auto scene = f.makeAndStart();

    // Index 0 is the default (Rules) — just update normally, no sub-state
    scene->Update(0.0f);

    // After an update with no input, IsPanelFocus must remain false
    // (indirectly: pressing Right must still move focus → Load creation works)
    EXPECT_CALL(f.mockConfig, GetSaveMetadata(_)).Times(AtLeast(1));
    PressRight(f.mockInput, *scene);   // → index 1
    PressRight(f.mockInput, *scene);   // → index 2
    PressRight(f.mockInput, *scene);   // → index 3 (Load)
}

// -----------------------------------------------------------------------
// Test 7: Navigating to index 1 → VolumePanel
//         Confirmed by: PushContext(VolumeControl) called when Confirm pressed
// Validates: Requirements 7.3
// -----------------------------------------------------------------------
TEST(MenuSceneTest, ButtonIndex1_CreatesVolumePanel)
{
    MenuSceneFixture f;
    ON_CALL(f.mockAudio, GetMasterVolume()).WillByDefault(Return(2));
    ON_CALL(f.mockAudio, GetBgmVolume()).WillByDefault(Return(2));
    ON_CALL(f.mockAudio, GetSfxVolume()).WillByDefault(Return(2));

    auto scene = f.makeAndStart();

    // Navigate to index 1
    PressRight(f.mockInput, *scene);

    // VolumePanel update (no-op frame)
    scene->Update(0.0f);

    // Pressing Confirm on VolumePanel row-select → enters tweak → PushContext(VolumeControl)
    EXPECT_CALL(f.mockInput, PushContext(InputContext::VolumeControl)).Times(1);
    PressConfirm(f.mockInput, *scene);
}

// -----------------------------------------------------------------------
// Test 8: Navigating to index 2 → SavePanel
//         Confirmed by: SaveState called when Confirm pressed on a slot
// Validates: Requirements 7.3
// -----------------------------------------------------------------------
TEST(MenuSceneTest, ButtonIndex2_CreatesSavePanel)
{
    MenuSceneFixture f;

    EXPECT_CALL(f.mockBattle, SaveState(0)).Times(1).WillOnce(Return(false));

    auto scene = f.makeAndStart();

    PressRight(f.mockInput, *scene);   // → index 1
    PressRight(f.mockInput, *scene);   // → index 2 (SavePanel)

    scene->Update(0.0f);  // SavePanel update (no-op)
    PressConfirm(f.mockInput, *scene);  // Confirm on slot 0 → SaveState(0)
}

// -----------------------------------------------------------------------
// Test 9: Navigating to index 3 → LoadPanel
//         Confirmed by: GetSaveMetadata called for all slots on construction
// Validates: Requirements 7.3
// -----------------------------------------------------------------------
TEST(MenuSceneTest, ButtonIndex3_CreatesLoadPanel_RefreshesMetaOnConstruct)
{
    MenuSceneFixture f;

    // LoadPanel constructor calls GetSaveMetadata for all SAVE_SLOT_COUNT slots
    for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
        EXPECT_CALL(f.mockConfig, GetSaveMetadata(i)).Times(1).WillOnce(Return(SaveMetadata{}));

    auto scene = f.makeAndStart();

    PressRight(f.mockInput, *scene);   // → index 1
    PressRight(f.mockInput, *scene);   // → index 2
    PressRight(f.mockInput, *scene);   // → index 3 (LoadPanel)
}

// -----------------------------------------------------------------------
// Test 10: Navigating to index 4 → ExitPanel
//          Confirmed by: ExitApplication called when Confirm pressed
// Validates: Requirements 7.3
// -----------------------------------------------------------------------
TEST(MenuSceneTest, ButtonIndex4_CreatesExitPanel)
{
    MenuSceneFixture f;

    auto scene = f.makeAndStart();

    PressRight(f.mockInput, *scene);   // → index 1
    PressRight(f.mockInput, *scene);   // → index 2
    PressRight(f.mockInput, *scene);   // → index 3
    PressRight(f.mockInput, *scene);   // → index 4 (ExitPanel)

    scene->Update(0.0f);  // ExitPanel update (no-op)

    // Pressing Confirm on ExitPanel → ExitApplication
    EXPECT_CALL(f.mockRender, ExitApplication()).Times(1);
    PressConfirm(f.mockInput, *scene);
}

} // namespace
} // namespace mc
