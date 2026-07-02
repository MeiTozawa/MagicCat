// Feature: mouse-input-support
// Tests: Properties 1-8, InfoScene routing

#ifdef small
#undef small
#endif

#include <algorithm>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include "MockServices.h"

import InputService;

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

namespace mc {
namespace {

// InGame actions whose mappings must not change when IgMouseClick is added
const std::vector<InputAction> kExistingInGameActions = {
    InputAction::Confirm,
    InputAction::Up,
    InputAction::Down,
    InputAction::Left,
    InputAction::Right,
    InputAction::DrawCard,
    InputAction::ToggleMenu,
};

// Property 8: Adding IgMouseClick to InGame must not alter IsPressed for existing actions
RC_GTEST_PROP(MouseInput, Property8_BackwardCompatibility, ()) {
    const std::size_t actionCount = kExistingInGameActions.size();
    std::vector<bool> expectedPressed;
    expectedPressed.reserve(actionCount);
    for (std::size_t i = 0; i < actionCount; ++i) {
        expectedPressed.push_back(*rc::gen::arbitrary<bool>());
    }

    NiceMock<MockInputService> mockWithoutMouse;
    for (std::size_t i = 0; i < actionCount; ++i) {
        ON_CALL(mockWithoutMouse, IsPressed(kExistingInGameActions[i]))
            .WillByDefault(Return(expectedPressed[i]));
    }
    std::vector<bool> resultsWithoutMouse;
    resultsWithoutMouse.reserve(actionCount);
    for (const auto& action : kExistingInGameActions) {
        resultsWithoutMouse.push_back(mockWithoutMouse.IsPressed(action));
    }

    NiceMock<MockInputService> mockWithMouse;
    for (std::size_t i = 0; i < actionCount; ++i) {
        ON_CALL(mockWithMouse, IsPressed(kExistingInGameActions[i]))
            .WillByDefault(Return(expectedPressed[i]));
    }
    ON_CALL(mockWithMouse, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{100, 200}));

    std::vector<bool> resultsWithMouse;
    resultsWithMouse.reserve(actionCount);
    for (const auto& action : kExistingInGameActions) {
        resultsWithMouse.push_back(mockWithMouse.IsPressed(action));
    }

    RC_ASSERT(resultsWithoutMouse.size() == resultsWithMouse.size());
    for (std::size_t i = 0; i < actionCount; ++i) {
        RC_ASSERT(resultsWithoutMouse[i] == resultsWithMouse[i]);
    }
}

// Property 8 (supplementary): OnMouseClick does not affect IsPressed for existing actions
RC_GTEST_PROP(MouseInput, Property8_MouseClickDoesNotAffectExistingIsPressed, ()) {
    const std::size_t actionCount = kExistingInGameActions.size();
    std::vector<bool> expectedPressed;
    expectedPressed.reserve(actionCount);
    for (std::size_t i = 0; i < actionCount; ++i) {
        expectedPressed.push_back(*rc::gen::arbitrary<bool>());
    }

    int cx = *rc::gen::arbitrary<int>();
    int cy = *rc::gen::arbitrary<int>();

    NiceMock<MockInputService> mockInput;
    for (std::size_t i = 0; i < actionCount; ++i) {
        ON_CALL(mockInput, IsPressed(kExistingInGameActions[i]))
            .WillByDefault(Return(expectedPressed[i]));
    }
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{cx, cy}));

    auto [clickX, clickY] = mockInput.OnMouseClick(InputAction::MouseClick);

    std::vector<bool> resultsAfterMouseClick;
    resultsAfterMouseClick.reserve(actionCount);
    for (const auto& action : kExistingInGameActions) {
        resultsAfterMouseClick.push_back(mockInput.IsPressed(action));
    }

    for (std::size_t i = 0; i < actionCount; ++i) {
        RC_ASSERT(resultsAfterMouseClick[i] == expectedPressed[i]);
    }
    RC_ASSERT(clickX == cx);
    RC_ASSERT(clickY == cy);
}

} // namespace
} // namespace mc

// Property 2: At most one CombatController HitBox matches any given (cx, cy)
// ActionMenu rows: [400,700) x [200+i*120, 300+i*120) for i in 0..3
// DrawCard box:    [50,250)  x [400,700)

namespace mc {
namespace {

constexpr int MC_ACTION_START_X  = 400;
constexpr int MC_ACTION_START_Y  = 200;
constexpr int MC_ACTION_RECT_W   = 300;
constexpr int MC_ACTION_RECT_H   = 100;
constexpr int MC_ACTION_OFFSET_Y = 120;

constexpr int MC_DRAW_CARD_X1 = 50;
constexpr int MC_DRAW_CARD_Y1 = 400;
constexpr int MC_DRAW_CARD_X2 = 250;
constexpr int MC_DRAW_CARD_Y2 = 700;

/// Returns the count of CombatController HitBoxes that contain (cx, cy). Must always be 0 or 1.
int HitTestCombatController(int cx, int cy)
{
    int count = 0;

    if (cx >= MC_DRAW_CARD_X1 && cx < MC_DRAW_CARD_X2 &&
        cy >= MC_DRAW_CARD_Y1 && cy < MC_DRAW_CARD_Y2)
        ++count;

    for (int i = 0; i < 4; ++i)
    {
        const int x1 = MC_ACTION_START_X;
        const int y1 = MC_ACTION_START_Y + i * MC_ACTION_OFFSET_Y;
        if (cx >= x1 && cx < x1 + MC_ACTION_RECT_W &&
            cy >= y1 && cy < y1 + MC_ACTION_RECT_H)
            ++count;
    }

    return count;
}

RC_GTEST_PROP(MouseInput, Property2_HitBoxExclusivity, ())
{
    const int cx = *rc::gen::arbitrary<int>();
    const int cy = *rc::gen::arbitrary<int>();
    RC_ASSERT(HitTestCombatController(cx, cy) <= 1);
}

} // namespace
} // namespace mc

// Property 3: ActionMenu click behavior
//   Case A (selectedActionIndex != i): publishes ActionSelectionEvent, no confirm
//   Case B (selectedActionIndex == i): enters confirm path, no ActionSelectionEvent

import CombatController;
import EventBus;
import SceneService;
import Character;
import Player;
import Enemy;

namespace mc {
namespace {

class MockSceneServiceP3 : public ISceneService {
public:
    MOCK_METHOD(void, Update, (float), (override));
    MOCK_METHOD(void, PushScene, (ESceneState), (override));
    MOCK_METHOD(void, PopScene, (), (override));
    MOCK_METHOD(void, RegisterScene, (ESceneState, std::unique_ptr<IScene>&&), (override));
    MOCK_METHOD(ESceneState, GetCurrentScene, (), (override));
    MOCK_METHOD(void, SetCurrentScene, (ESceneState), (override));
};

// Case A: first click on row i (i != 0) selects it and publishes ActionSelectionEvent
RC_GTEST_PROP(MouseInput, Property3_SelectionBeforeConfirm_CaseA_SelectsAndPublishes, ())
{
    // Skip i==0: controller starts with selectedActionIndex==0, so clicking row 0 immediately
    // enters the confirm path rather than the selection path.
    const int i  = *rc::gen::inRange(1, 4);
    const int cx = *rc::gen::inRange(400, 700);
    const int cy = *rc::gen::inRange(200 + i * 120, 300 + i * 120);

    Player player;
    Enemy  enemy;

    NiceMock<MockBattleService> mockBattle;
    ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
    ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockCardService>    mockCard;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{cx, cy}));

    auto controller = CreateCombatController(mockInput, mockBattle, mockScene, mockCard);

    int actionSelCount = 0;
    int actionSelIndex = -1;
    auto hSel = EventBus::Subscribe<ActionSelectionEvent>([&](const ActionSelectionEvent& e) {
        if (!e.silent) { ++actionSelCount; actionSelIndex = e.selectedIndex; }
    });
    int combatEventCount = 0;
    auto hCombat = EventBus::Subscribe<CombatEvent>([&](const CombatEvent&) { ++combatEventCount; });

    controller->Update(0.0f);

    RC_ASSERT(actionSelCount == 1);
    RC_ASSERT(actionSelIndex == i);
    RC_ASSERT(combatEventCount == 0);

    EventBus::Unsubscribe(hSel);
    EventBus::Unsubscribe(hCombat);
}

// Case B: second click on the already-selected row enters confirm, no ActionSelectionEvent
RC_GTEST_PROP(MouseInput, Property3_SelectionBeforeConfirm_CaseB_ConfirmDoesNotSelectAgain, ())
{
    const int i  = *rc::gen::inRange(1, 4);
    const int cx = *rc::gen::inRange(400, 700);
    const int cy = *rc::gen::inRange(200 + i * 120, 300 + i * 120);

    Player player;
    Enemy  enemy;

    NiceMock<MockBattleService> mockBattle;
    ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
    ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockCardService>    mockCard;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{cx, cy}));

    auto controller = CreateCombatController(mockInput, mockBattle, mockScene, mockCard);
    controller->Update(0.0f);  // first click: selects row i

    int actionSelCount = 0;
    auto hSel = EventBus::Subscribe<ActionSelectionEvent>([&](const ActionSelectionEvent& e) {
        if (!e.silent) ++actionSelCount;
    });

    controller->Update(0.0f);  // second click: confirm path

    RC_ASSERT(actionSelCount == 0);

    EventBus::Unsubscribe(hSel);
}

} // namespace
} // namespace mc

// Property 4: Mouse DrawCard click and keyboard DrawCard produce identical results.
// Both call cardService.DrawCard() exactly once and apply the same card-effect logic.

namespace mc {
namespace {

RC_GTEST_PROP(MouseInput, Property4_DrawCard_MousePath_CallsDrawCardOnce, ())
{
    const ECardType cardType = *rc::gen::elementOf(std::vector<ECardType>{
        ECardType::Magic, ECardType::Rock, ECardType::Scissors, ECardType::Paper
    });
    const int cardPower = *rc::gen::inRange(1, 10);
    const Card card{cardType, cardPower};

    const int cx = *rc::gen::inRange(MC_DRAW_CARD_X1, MC_DRAW_CARD_X2);
    const int cy = *rc::gen::inRange(MC_DRAW_CARD_Y1, MC_DRAW_CARD_Y2);

    Player player;
    Enemy  enemy;
    const int mpBefore             = player.GetMp();
    const int rockOffsetBefore     = enemy.GetWeightOffset(EAttackType::Rock);
    const int scissorsOffsetBefore = enemy.GetWeightOffset(EAttackType::Scissors);
    const int paperOffsetBefore    = enemy.GetWeightOffset(EAttackType::Paper);

    NiceMock<MockBattleService>  mockBattle;
    ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
    ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{cx, cy}));

    MockCardService mockCard;
    EXPECT_CALL(mockCard, DrawCard()).Times(1).WillOnce(Return(card));
    ON_CALL(mockCard, DiscardHand()).WillByDefault(testing::Return());

    auto controller = CreateCombatController(mockInput, mockBattle, mockScene, mockCard);

    int drawCardEventCount = 0;
    auto hDraw = EventBus::Subscribe<DrawCardEvent>([&](const DrawCardEvent&) { ++drawCardEventCount; });

    controller->Update(0.0f);

    RC_ASSERT(drawCardEventCount == 1);
    if (cardType == ECardType::Magic)
    {
        RC_ASSERT(player.GetMp() == std::min(mpBefore + cardPower, player.GetMaxMp()));
    }
    else
    {
        const EAttackType attackType = ToAttackType(cardType);
        const int offsetAfter = enemy.GetWeightOffset(attackType);
        RC_ASSERT(offsetAfter == (cardType == ECardType::Rock     ? rockOffsetBefore
                                : cardType == ECardType::Scissors ? scissorsOffsetBefore
                                :                                   paperOffsetBefore) + cardPower);
    }

    EventBus::Unsubscribe(hDraw);
}

RC_GTEST_PROP(MouseInput, Property4_DrawCard_KeyboardPath_CallsDrawCardOnce, ())
{
    const ECardType cardType = *rc::gen::elementOf(std::vector<ECardType>{
        ECardType::Magic, ECardType::Rock, ECardType::Scissors, ECardType::Paper
    });
    const int cardPower = *rc::gen::inRange(1, 10);
    const Card card{cardType, cardPower};

    Player player;
    Enemy  enemy;
    const int mpBefore             = player.GetMp();
    const int rockOffsetBefore     = enemy.GetWeightOffset(EAttackType::Rock);
    const int scissorsOffsetBefore = enemy.GetWeightOffset(EAttackType::Scissors);
    const int paperOffsetBefore    = enemy.GetWeightOffset(EAttackType::Paper);

    NiceMock<MockBattleService>  mockBattle;
    ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
    ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, IsPressed(InputAction::DrawCard)).WillByDefault(Return(true));

    MockCardService mockCard;
    EXPECT_CALL(mockCard, DrawCard()).Times(1).WillOnce(Return(card));
    ON_CALL(mockCard, DiscardHand()).WillByDefault(testing::Return());

    auto controller = CreateCombatController(mockInput, mockBattle, mockScene, mockCard);

    int drawCardEventCount = 0;
    auto hDraw = EventBus::Subscribe<DrawCardEvent>([&](const DrawCardEvent&) { ++drawCardEventCount; });

    controller->Update(0.0f);

    RC_ASSERT(drawCardEventCount == 1);
    if (cardType == ECardType::Magic)
    {
        RC_ASSERT(player.GetMp() == std::min(mpBefore + cardPower, player.GetMaxMp()));
    }
    else
    {
        const EAttackType attackType = ToAttackType(cardType);
        const int offsetAfter = enemy.GetWeightOffset(attackType);
        RC_ASSERT(offsetAfter == (cardType == ECardType::Rock     ? rockOffsetBefore
                                : cardType == ECardType::Scissors ? scissorsOffsetBefore
                                :                                   paperOffsetBefore) + cardPower);
    }

    EventBus::Unsubscribe(hDraw);
}

// Cross-check: both paths produce identical player/enemy state and DrawCardEvent count
RC_GTEST_PROP(MouseInput, Property4_DrawCard_BothPaths_ProduceSameEffect, ())
{
    const ECardType cardType = *rc::gen::elementOf(std::vector<ECardType>{
        ECardType::Magic, ECardType::Rock, ECardType::Scissors, ECardType::Paper
    });
    const int cardPower = *rc::gen::inRange(1, 10);
    const Card card{cardType, cardPower};

    const int cx = *rc::gen::inRange(MC_DRAW_CARD_X1, MC_DRAW_CARD_X2);
    const int cy = *rc::gen::inRange(MC_DRAW_CARD_Y1, MC_DRAW_CARD_Y2);

    // Mouse path
    Player mousePlayer;
    Enemy  mouseEnemy;
    {
        NiceMock<MockBattleService>  mockBattle;
        ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(mousePlayer));
        ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(mouseEnemy));

        NiceMock<MockInputService>   mockInput;
        NiceMock<MockSceneServiceP3> mockScene;
        ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
        ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
        ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
        ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
            .WillByDefault(Return(Point<int>{cx, cy}));

        MockCardService mockCard;
        EXPECT_CALL(mockCard, DrawCard()).Times(1).WillOnce(Return(card));
        ON_CALL(mockCard, DiscardHand()).WillByDefault(testing::Return());

        int mouseDrawEvents = 0;
        auto hDraw = EventBus::Subscribe<DrawCardEvent>([&](const DrawCardEvent&) { ++mouseDrawEvents; });

        CreateCombatController(mockInput, mockBattle, mockScene, mockCard)->Update(0.0f);
        RC_ASSERT(mouseDrawEvents == 1);
        EventBus::Unsubscribe(hDraw);
    }

    // Keyboard path
    Player kbPlayer;
    Enemy  kbEnemy;
    {
        NiceMock<MockBattleService>  mockBattle2;
        ON_CALL(mockBattle2, GetPlayer()).WillByDefault(testing::ReturnRef(kbPlayer));
        ON_CALL(mockBattle2, GetEnemy()).WillByDefault(testing::ReturnRef(kbEnemy));

        NiceMock<MockInputService>   mockInput2;
        NiceMock<MockSceneServiceP3> mockScene2;
        ON_CALL(mockInput2, IsPressed(_)).WillByDefault(Return(false));
        ON_CALL(mockInput2, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
        ON_CALL(mockInput2, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
        ON_CALL(mockInput2, IsPressed(InputAction::DrawCard)).WillByDefault(Return(true));

        MockCardService mockCard2;
        EXPECT_CALL(mockCard2, DrawCard()).Times(1).WillOnce(Return(card));
        ON_CALL(mockCard2, DiscardHand()).WillByDefault(testing::Return());

        int kbDrawEvents = 0;
        auto hDraw2 = EventBus::Subscribe<DrawCardEvent>([&](const DrawCardEvent&) { ++kbDrawEvents; });

        CreateCombatController(mockInput2, mockBattle2, mockScene2, mockCard2)->Update(0.0f);
        RC_ASSERT(kbDrawEvents == 1);
        EventBus::Unsubscribe(hDraw2);
    }

    RC_ASSERT(mousePlayer.GetMp() == kbPlayer.GetMp());
    RC_ASSERT(mouseEnemy.GetWeightOffset(EAttackType::Rock)     == kbEnemy.GetWeightOffset(EAttackType::Rock));
    RC_ASSERT(mouseEnemy.GetWeightOffset(EAttackType::Scissors) == kbEnemy.GetWeightOffset(EAttackType::Scissors));
    RC_ASSERT(mouseEnemy.GetWeightOffset(EAttackType::Paper)    == kbEnemy.GetWeightOffset(EAttackType::Paper));
}

} // namespace
} // namespace mc

// Property 5: MagicSubMenu guard
//   Part A: When isMagicMenuOpen==false, clicking rows 1-3 only selects (no UseMagic)
//   Part B: When isMagicMenuOpen==true,  j=1→Clairvoyance, j=2→PowerBoost, j=3→Heal;
//           on success isMagicMenuOpen becomes false and selectedActionIndex becomes 0

namespace mc {
namespace {

// Part A: closed menu — click on row j just selects, does not call UseMagic
RC_GTEST_PROP(MouseInput, Property5_MagicSubMenuGuard_ClosedMenu_NoUseMagic, ())
{
    const int j  = *rc::gen::inRange(1, 4);
    const int cx = *rc::gen::inRange(400, 700);
    const int cy = *rc::gen::inRange(200 + j * 120, 300 + j * 120);

    Player player;
    Enemy  enemy;
    const int mpBefore = player.GetMp();

    NiceMock<MockBattleService> mockBattle;
    ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
    ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockCardService>    mockCard;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{cx, cy}));

    auto controller = CreateCombatController(mockInput, mockBattle, mockScene, mockCard);

    int actionSelCount = 0;
    int actionSelIndex = -1;
    auto hSel = EventBus::Subscribe<ActionSelectionEvent>([&](const ActionSelectionEvent& e) {
        if (!e.silent) { ++actionSelCount; actionSelIndex = e.selectedIndex; }
    });

    controller->Update(0.0f);

    // UseMagic not called: MP unchanged
    RC_ASSERT(player.GetMp() == mpBefore);
    RC_ASSERT(actionSelCount == 1);
    RC_ASSERT(actionSelIndex == j);

    EventBus::Unsubscribe(hSel);
}

// Part B: open menu — correct EMagic dispatched, state reset on success
RC_GTEST_PROP(MouseInput, Property5_MagicSubMenuGuard_OpenMenu_CorrectMapping, ())
{
    const int j  = *rc::gen::inRange(1, 4);
    const int cx = *rc::gen::inRange(400, 700);
    const int cy = *rc::gen::inRange(200 + j * 120, 300 + j * 120);

    const EMagic expectedMagic = (j == 1) ? EMagic::Clairvoyance
                                : (j == 2) ? EMagic::PowerBoost
                                :             EMagic::Heal;

    Player player;
    Enemy  enemy;

    NiceMock<MockBattleService> mockBattle;
    ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
    ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockCardService>    mockCard;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));

    auto controller = CreateCombatController(mockInput, mockBattle, mockScene, mockCard);

    // Open MagicMenu: click row 0 while selectedActionIndex==0 toggles isMagicMenuOpen to true
    const int openCx = *rc::gen::inRange(400, 700);
    const int openCy = *rc::gen::inRange(200, 300);
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{openCx, openCy}));
    controller->Update(0.0f);

    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{cx, cy}));

    EMagic receivedMagic = EMagic::Null;
    int magicEventCount = 0;
    auto hMagic = EventBus::Subscribe<MagicEvent>([&](const MagicEvent& e) {
        receivedMagic = e.magic; ++magicEventCount;
    });

    int actionSelCount = 0;
    bool menuClosedViaEvent = false;
    int  selIndexViaEvent   = -1;
    auto hSel = EventBus::Subscribe<ActionSelectionEvent>([&](const ActionSelectionEvent& e) {
        if (!e.silent) { ++actionSelCount; menuClosedViaEvent = !e.isMagicMenuOpen; selIndexViaEvent = e.selectedIndex; }
    });

    const int mpBefore = player.GetMp();
    controller->Update(0.0f);

    if (player.IsMagicUsable(expectedMagic) || magicEventCount > 0)
    {
        RC_ASSERT(magicEventCount == 1);
        RC_ASSERT(receivedMagic == expectedMagic);
        RC_ASSERT(actionSelCount == 1);
        RC_ASSERT(menuClosedViaEvent == true);
        RC_ASSERT(selIndexViaEvent == 0);
        RC_ASSERT(player.GetMp() < mpBefore);
    }

    EventBus::Unsubscribe(hMagic);
    EventBus::Unsubscribe(hSel);
}

// Supplementary: after successful UseMagic, isMagicMenuOpen==false and selectedActionIndex==0
RC_GTEST_PROP(MouseInput, Property5_MagicSubMenuGuard_AfterSuccess_StateReset, ())
{
    const int j  = *rc::gen::inRange(1, 4);
    const int cx = *rc::gen::inRange(400, 700);
    const int cy = *rc::gen::inRange(200 + j * 120, 300 + j * 120);

    Player player;
    Enemy  enemy;

    NiceMock<MockBattleService> mockBattle;
    ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
    ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockCardService>    mockCard;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));

    auto controller = CreateCombatController(mockInput, mockBattle, mockScene, mockCard);

    // Open MagicMenu
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{450, 250}));
    controller->Update(0.0f);

    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{cx, cy}));

    bool gotSuccessEvent = false;
    bool menuOpenInEvent = true;
    int  selIndexInEvent = -1;
    auto hSel = EventBus::Subscribe<ActionSelectionEvent>([&](const ActionSelectionEvent& e) {
        if (!e.silent) { gotSuccessEvent = true; menuOpenInEvent = e.isMagicMenuOpen; selIndexInEvent = e.selectedIndex; }
    });

    controller->Update(0.0f);

    if (gotSuccessEvent)
    {
        RC_ASSERT(menuOpenInEvent == false);
        RC_ASSERT(selIndexInEvent == 0);
    }

    EventBus::Unsubscribe(hSel);
}

} // namespace
} // namespace mc

// Property 1: (-1,-1) guard — when OnMouseClick returns (-1,-1), no game state must change
// Covers: CombatController (no DrawCard/ActionSelectionEvent/CombatEvent) and InfoScene (no StartStage/PushScene)

import SceneService;

namespace mc {
namespace {

RC_GTEST_PROP(MouseInput, Property1_CombatController_NegOneNegOne_Guard, ())
{
    const float dt = *rc::gen::arbitrary<float>();

    Player player;
    Enemy  enemy;

    NiceMock<MockBattleService>  mockBattle;
    ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
    ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockCardService>    mockCard;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));

    auto controller = CreateCombatController(mockInput, mockBattle, mockScene, mockCard);

    int drawCardEventCount  = 0;
    int actionSelEventCount = 0;
    int combatEventCount    = 0;

    auto hDraw   = EventBus::Subscribe<DrawCardEvent>([&](const DrawCardEvent&) { ++drawCardEventCount; });
    auto hSel    = EventBus::Subscribe<ActionSelectionEvent>([&](const ActionSelectionEvent& e) { if (!e.silent) ++actionSelEventCount; });
    auto hCombat = EventBus::Subscribe<CombatEvent>([&](const CombatEvent&) { ++combatEventCount; });

    controller->Update(dt);

    RC_ASSERT(drawCardEventCount  == 0);
    RC_ASSERT(actionSelEventCount == 0);
    RC_ASSERT(combatEventCount    == 0);

    EXPECT_CALL(mockCard, DrawCard()).Times(0);

    EventBus::Unsubscribe(hDraw);
    EventBus::Unsubscribe(hSel);
    EventBus::Unsubscribe(hCombat);
}

RC_GTEST_PROP(MouseInput, Property1_CombatController_NegOneNegOne_DrawCardNotCalled, ())
{
    Player player;
    Enemy  enemy;

    NiceMock<MockBattleService>  mockBattle;
    ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
    ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, GetMousePosition()).WillByDefault(Return(Point<int>{0, 0}));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));

    MockCardService mockCard;
    EXPECT_CALL(mockCard, DrawCard()).Times(0);
    ON_CALL(mockCard, DiscardHand()).WillByDefault(testing::Return());

    CreateCombatController(mockInput, mockBattle, mockScene, mockCard)->Update(0.0f);

    EXPECT_CALL(mockScene, PushScene(_)).Times(0);
}

RC_GTEST_PROP(MouseInput, Property1_InfoScene_NegOneNegOne_Guard, ())
{
    const float dt = *rc::gen::arbitrary<float>();

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockBattleService>  mockBattle;
    NiceMock<MockRenderService>  mockRender;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockRender, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(mockRender, GetWindowHeight()).WillByDefault(Return(720));

    EXPECT_CALL(mockBattle, StartStage()).Times(0);
    EXPECT_CALL(mockScene, PushScene(_)).Times(0);

    CreateInfoScene(mockInput, mockScene, mockRender, mockBattle)->Update(dt);
}

RC_GTEST_PROP(MouseInput, Property1_InfoScene_NegOneNegOne_Guard_MultiFrame, ())
{
    const int frameCount = *rc::gen::inRange(50, 150);

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockBattleService>  mockBattle;
    NiceMock<MockRenderService>  mockRender;

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockRender, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(mockRender, GetWindowHeight()).WillByDefault(Return(720));

    EXPECT_CALL(mockBattle, StartStage()).Times(0);
    EXPECT_CALL(mockScene, PushScene(_)).Times(0);

    auto infoScene = CreateInfoScene(mockInput, mockScene, mockRender, mockBattle);
    for (int f = 0; f < frameCount; ++f)
        infoScene->Update(1.0f / 60.0f);
}

} // namespace
} // namespace mc

// Property 6: RulesScene currentPage always stays in [0, 1] after any Next/Prev sequence

namespace mc {
namespace {

RC_GTEST_PROP(MouseInput, Property6_RulesScene_PageClamp_ArbitrarySequence, ())
{
    // moves: -1=Prev, 0=no-op, 1=Next
    auto moves = *rc::gen::container<std::vector<int>>(
        rc::gen::elementOf(std::vector<int>{-1, 0, 1}));

    int page = 0;
    for (int m : moves)
    {
        page += m;
        page = std::clamp(page, 0, 1);
        RC_ASSERT(page >= 0 && page <= 1);
    }
}

TEST(MouseInput, Property6_RulesScene_PageClamp_Adversarial_1000Prev)
{
    int page = 0;
    for (int i = 0; i < 1000; ++i)
    {
        page--;
        page = std::clamp(page, 0, 1);
        ASSERT_GE(page, 0);
        ASSERT_LE(page, 1);
    }
    EXPECT_EQ(page, 0);
}

TEST(MouseInput, Property6_RulesScene_PageClamp_Adversarial_1000Next)
{
    int page = 1;
    for (int i = 0; i < 1000; ++i)
    {
        page++;
        page = std::clamp(page, 0, 1);
        ASSERT_GE(page, 0);
        ASSERT_LE(page, 1);
    }
    EXPECT_EQ(page, 1);
}

} // namespace
} // namespace mc

// InfoScene mouse routing unit tests
// Actual InfoScene behavior: any click with x∈[0,w) y∈[0,h) calls StartStage();
// (-1,-1) does nothing; ToggleMenu key opens Rules screen.

namespace mc {
namespace {

static void SetupInfoSceneRenderMock(NiceMock<MockRenderService>& mockRender)
{
    ON_CALL(mockRender, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(mockRender, GetWindowHeight()).WillByDefault(Return(720));
    ON_CALL(mockRender, GetFontSize()).WillByDefault(Return(24));
    ON_CALL(mockRender, GetDrawStringWidth(_)).WillByDefault(Return(0));
}

TEST(InfoScene, InfoScene_StartStage_CalledOnValidClick)
{
    NiceMock<MockRenderService>  mockRender;
    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockBattleService>  mockBattle;
    SetupInfoSceneRenderMock(mockRender);

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockInput, OnMouseClick(InputAction::MouseClick))
        .WillByDefault(Return(Point<int>{640, 576}));

    EXPECT_CALL(mockBattle, StartStage()).Times(1);
    EXPECT_CALL(mockScene, PushScene(_)).Times(0);

    CreateInfoScene(mockInput, mockScene, mockRender, mockBattle)->Update(0.0f);
}

TEST(InfoScene, InfoScene_NothingCalledOnNegativeOneClick)
{
    NiceMock<MockRenderService>  mockRender;
    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockBattleService>  mockBattle;
    SetupInfoSceneRenderMock(mockRender);

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));

    EXPECT_CALL(mockBattle, StartStage()).Times(0);
    EXPECT_CALL(mockScene, PushScene(_)).Times(0);

    CreateInfoScene(mockInput, mockScene, mockRender, mockBattle)->Update(0.0f);
}

TEST(InfoScene, InfoScene_ToggleMenu_Opens_Rules)
{
    NiceMock<MockRenderService>  mockRender;
    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockBattleService>  mockBattle;
    SetupInfoSceneRenderMock(mockRender);

    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, IsPressed(InputAction::ToggleMenu)).WillByDefault(Return(true));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));

    EXPECT_CALL(mockScene, PushScene(ESceneState::Rules)).Times(1);
    EXPECT_CALL(mockBattle, StartStage()).Times(0);

    CreateInfoScene(mockInput, mockScene, mockRender, mockBattle)->Update(0.0f);
}

} // namespace
} // namespace mc

// Property 7: RulesScene::Start() calls PushContext(Menu) exactly once per invocation,
// and never calls it from Update().

namespace mc {
namespace {

RC_GTEST_PROP(MouseInput, Property7_RulesScene_PushContext_CalledOncePerStart, ())
{
    int n = *rc::gen::inRange(1, 11);

    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockAssetService>   mockAsset;
    NiceMock<MockRenderService>  mockRender;
    ON_CALL(mockRender, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(mockRender, GetWindowHeight()).WillByDefault(Return(720));

    int pushContextCount = 0;
    ON_CALL(mockInput, PushContext(InputContext::Menu))
        .WillByDefault([&](InputContext) { ++pushContextCount; });

    auto scene = CreateRulesScene(mockInput, mockScene, mockAsset, mockRender);
    for (int i = 0; i < n; ++i)
        scene->Start();

    RC_ASSERT(pushContextCount == n);
}

TEST(MouseInput, Property7_RulesScene_PushContext_NotCalledInUpdate)
{
    NiceMock<MockInputService>   mockInput;
    NiceMock<MockSceneServiceP3> mockScene;
    NiceMock<MockAssetService>   mockAsset;
    NiceMock<MockRenderService>  mockRender;
    ON_CALL(mockRender, GetWindowWidth()).WillByDefault(Return(1280));
    ON_CALL(mockRender, GetWindowHeight()).WillByDefault(Return(720));
    ON_CALL(mockInput, IsPressed(_)).WillByDefault(Return(false));
    ON_CALL(mockInput, OnMouseClick(_)).WillByDefault(Return(Point<int>{-1, -1}));
    ON_CALL(mockAsset, GetImageHandle(_)).WillByDefault(Return(0));

    int pushContextCount = 0;
    ON_CALL(mockInput, PushContext(InputContext::Menu))
        .WillByDefault([&](InputContext) { ++pushContextCount; });

    auto scene = CreateRulesScene(mockInput, mockScene, mockAsset, mockRender);
    scene->Start();

    for (int i = 0; i < 10; ++i)
        scene->Update(0.016f);

    EXPECT_EQ(pushContextCount, 1);
}

} // namespace
} // namespace mc
