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
                return;
            }
            if (inputService.IsPressed(InputAction::Down))
            {
                if (selectedActionIndex < ACTION_MAX)
                {
                    selectedActionIndex++;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
                return;
            }
            if (inputService.IsPressed(InputAction::Confirm))
            {
                HandleConfirm();
                return;
            }
            if (inputService.IsPressed(InputAction::DrawCard))
            {
                ProcessDrawCard(cardService.DrawCard());
                EventBus::Publish(DrawCardEvent());
                return;
            }
            if (inputService.IsPressed(InputAction::ToggleMenu))
            {
                sceneService.PushScene(ESceneState::Rules);
                return;
            }
        }

        void HandleConfirm()
        {
            if (selectedActionIndex == ACTION_MAGIC)
            {
                isMagicMenuOpen = !isMagicMenuOpen;
                EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                return;
            }
            if (isMagicMenuOpen)
            {
                if (TryUseMagic(selectedActionIndex))
                {
                    isMagicMenuOpen = false;
                    selectedActionIndex = ACTION_MAGIC;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
                return;
            }
            ResolveAttack(selectedActionIndex);
        }

        bool TryUseMagic(int index) const
        {
            switch (index)
            {
            case 1:
                {
                    bool success = characterService.GetPlayer().UseMagic(EMagic::Clairvoyance);
                    if (success) characterService.GetEnemy().SetExposed(true);
                    return success;
                }
            case 2:
                return characterService.GetPlayer().UseMagic(EMagic::PowerBoost);
            case 3:
                return characterService.GetPlayer().UseMagic(EMagic::Heal);
            default:
                return false;
            }
        }

        std::optional<EAttackType> GetAttackTypeFromIndex(int index) const
        {
            switch (index)
            {
            case ACTION_ROCK:     return EAttackType::Rock;
            case ACTION_SCISSORS: return EAttackType::Scissors;
            case ACTION_PAPER:    return EAttackType::Paper;
            default:              return std::nullopt;
            }
        }

        void ResolveAttack(int actionIndex) const
        {
            auto playerAttackIntentOpt = GetAttackTypeFromIndex(actionIndex);
            if (!playerAttackIntentOpt) return;
            EAttackType playerAttackIntent = *playerAttackIntentOpt;

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

        void ProcessDrawCard(Card c) const
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

        std::optional<int> GetHoveredActionMenuRow(int mx, int my) const
        {
            for (int i = 0; i < 4; ++i)
            {
                int x1 = ACTION_MENU_X;
                int y1 = ACTION_MENU_Y + i * ACTION_MENU_STEP_Y;
                int x2 = x1 + ACTION_MENU_W;
                int y2 = y1 + ACTION_MENU_H;
                if (mx >= x1 && mx < x2 && my >= y1 && my < y2)
                    return i;
            }
            return std::nullopt;
        }

        void HandleMouseHover()
        {
            if (inputService.GetActiveDevice() != InputDevice::Mouse) return;

            auto mousePos = inputService.GetMousePosition();
            auto rowOpt = GetHoveredActionMenuRow(mousePos.x, mousePos.y);
            if (rowOpt && selectedActionIndex != *rowOpt)
            {
                selectedActionIndex = *rowOpt;
                EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
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
            if (auto rowOpt = GetHoveredActionMenuRow(click.x, click.y))
            {
                HandleActionMenuClick(*rowOpt);
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
                return;
            }

            if (selectedActionIndex == clickedRow)
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
                return;
            }

            selectedActionIndex = clickedRow;
            EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
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
