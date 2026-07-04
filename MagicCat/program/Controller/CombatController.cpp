module;

#include <memory>
#include <RenderUtils.h>

module CombatController;

import InputService;
import BattleService;
import EventBus;
import CardService;
import Character;
import Player;
import SceneService;

namespace mc {
    class CombatController : public ICombatController
    {
    public:
        CombatController(IInputService& input, IBattleService& character, ISceneService& scene, ICardService& card)
            : inputService(input), characterService(character), sceneService(scene), cardService(card) {}

        void Reset() override
        {
            selectedActionIndex = 0;
            isMagicMenuOpen = false;
            EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen, true));
        }

        void Update(float deltaTime) override
        {
            HandleKeyboardInput();
            HandleMouseHover();
            HandleMouseClick();
        }

    private:
        void HandleKeyboardInput()
        {
            if (inputService.IsPressed(InputAction::Up))
            {
                if (selectedActionIndex > ACTION_MAGIC)
                {
                    selectedActionIndex--;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
            }
            else if (inputService.IsPressed(InputAction::Down))
            {
                if (selectedActionIndex < ACTION_MAX)
                {
                    selectedActionIndex++;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
            }
            else if (inputService.IsPressed(InputAction::Confirm))
            {
                HandleConfirm();
            }
            else if (inputService.IsPressed(InputAction::DrawCard))
            {
                ProcessDrawCard(cardService.DrawCard());
                EventBus::Publish(DrawCardEvent());
            }
            else if (inputService.IsPressed(InputAction::ToggleMenu))
            {
                sceneService.PushScene(ESceneState::Rules);
            }
        }

        void HandleConfirm()
        {
            if (selectedActionIndex == ACTION_MAGIC)
            {
                isMagicMenuOpen = !isMagicMenuOpen;
                EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
            }
            else if (isMagicMenuOpen)
            {
                if (TryUseMagic(selectedActionIndex))
                {
                    isMagicMenuOpen = false;
                    selectedActionIndex = ACTION_MAGIC;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
            }
            else
            {
                ResolveAttack(selectedActionIndex);
            }
        }

        bool TryUseMagic(int index)
        {
            bool success = false;
            if (index == 1)
            {
                success = characterService.GetPlayer().UseMagic(EMagic::Clairvoyance);
                if (success) characterService.GetEnemy().SetExposed(true);
            }
            else if (index == 2)
            {
                success = characterService.GetPlayer().UseMagic(EMagic::PowerBoost);
            }
            else if (index == 3)
            {
                success = characterService.GetPlayer().UseMagic(EMagic::Heal);
            }
            return success;
        }

        void ResolveAttack(int actionIndex)
        {
            EAttackType playerAttackIntent;
            if      (actionIndex == ACTION_ROCK)     playerAttackIntent = EAttackType::Rock;
            else if (actionIndex == ACTION_SCISSORS) playerAttackIntent = EAttackType::Scissors;
            else if (actionIndex == ACTION_PAPER)    playerAttackIntent = EAttackType::Paper;
            else return;

            EAttackType enemyAttackIntent = characterService.GetEnemy().GetAttackIntent();
            float playerWinRate = characterService.GetEnemy().GetLoseRateAgainst(playerAttackIntent);

            int rockOffset     = characterService.GetEnemy().GetWeightOffset(EAttackType::Rock);
            int scissorsOffset = characterService.GetEnemy().GetWeightOffset(EAttackType::Scissors);
            int paperOffset    = characterService.GetEnemy().GetWeightOffset(EAttackType::Paper);

            int playerDamage = characterService.GetPlayer().GetDamage(playerAttackIntent);
            int enemyDamage  = characterService.GetEnemy().GetDamage(enemyAttackIntent);

            if (LosesTo(playerAttackIntent, enemyAttackIntent))
                characterService.GetPlayer().TakeDamage(enemyDamage);
            if (LosesTo(enemyAttackIntent, playerAttackIntent))
                characterService.GetEnemy().TakeDamage(playerDamage);

            characterService.GetEnemy().ResetWeights();
            cardService.DiscardHand();
            characterService.GetPlayer().ResetDamageOffset();

            EventBus::Publish(
                CombatEvent(playerAttackIntent, enemyAttackIntent, playerDamage, enemyDamage,
                            playerWinRate, rockOffset, scissorsOffset, paperOffset)
            );
        }

        void ProcessDrawCard(Card c)
        {
            if (c.CardType == ECardType::Magic)
            {
                characterService.GetPlayer().ChangeMp(c.Power);
            }
            else if (c.CardType == ECardType::Rock || c.CardType == ECardType::Scissors || c.CardType == ECardType::Paper)
            {
                characterService.GetEnemy().AddWeight(ToAttackType(c.CardType), c.Power);
            }
        }

        void HandleMouseHover()
        {
            if (inputService.GetActiveDevice() != InputDevice::Mouse) return;

            auto mousePos = inputService.GetMousePosition();
            int mx = mousePos.x;
            int my = mousePos.y;
            for (int i = 0; i < 4; ++i)
            {
                int hx1 = ACTION_MENU_X;
                int hy1 = ACTION_MENU_Y + i * ACTION_MENU_STEP_Y;
                int hx2 = hx1 + ACTION_MENU_W;
                int hy2 = hy1 + ACTION_MENU_H;
                if (mx >= hx1 && mx < hx2 && my >= hy1 && my < hy2)
                {
                    if (selectedActionIndex != i)
                    {
                        selectedActionIndex = i;
                        EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                    }
                    break;
                }
            }
        }

        void HandleMouseClick()
        {
            auto click = inputService.OnMouseClick(InputAction::MouseClick);
            if (click.x == -1 || click.y == -1) return;

            // 山札クリック
            if (click.x >= DRAW_PILE_X1 && click.x < DRAW_PILE_X2 &&
                click.y >= DRAW_PILE_Y1 && click.y < DRAW_PILE_Y2)
            {
                ProcessDrawCard(cardService.DrawCard());
                EventBus::Publish(DrawCardEvent());
                return;
            }

            // ActionMenu 行クリック
            for (int i = 0; i < 4; ++i)
            {
                int x1 = ACTION_MENU_X;
                int y1 = ACTION_MENU_Y + i * ACTION_MENU_STEP_Y;
                int x2 = x1 + ACTION_MENU_W;
                int y2 = y1 + ACTION_MENU_H;
                if (click.x >= x1 && click.x < x2 && click.y >= y1 && click.y < y2)
                {
                    HandleActionMenuClick(i);
                    return;
                }
            }
        }

        void HandleActionMenuClick(int clickedRow)
        {
            if (isMagicMenuOpen && clickedRow >= 1)
            {
                if (TryUseMagic(clickedRow))
                {
                    isMagicMenuOpen = false;
                    selectedActionIndex = ACTION_MAGIC;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
            }
            else if (selectedActionIndex == clickedRow)
            {
                // 同じ行を再クリック → Confirm 相当
                if (selectedActionIndex == ACTION_MAGIC)
                {
                    isMagicMenuOpen = !isMagicMenuOpen;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
                else
                {
                    ResolveAttack(selectedActionIndex);
                }
            }
            else
            {
                selectedActionIndex = clickedRow;
                EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
            }
        }

        IInputService& inputService;
        IBattleService& characterService;
        ISceneService& sceneService;
        ICardService& cardService;
        int selectedActionIndex = 0;
        bool isMagicMenuOpen = false;
    };

    std::unique_ptr<ICombatController> CreateCombatController(IInputService& inputService,
                                                              IBattleService& characterService,
                                                              ISceneService& sceneService, ICardService& cardService)
    {
        return std::make_unique<CombatController>(inputService, characterService, sceneService, cardService);
    }
}
