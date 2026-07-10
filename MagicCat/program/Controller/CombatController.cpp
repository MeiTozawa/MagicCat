module;

#include <array>
#include <memory>
#include <optional>
#include <RenderUtils.h>

module CombatController;

import InputService;
import BattleService;
import EventBus;
import CardService;
import Character;
import Player;
import SceneService;
import ButtonGroup;

namespace mc {
    class CombatController : public ICombatController
    {
    public:
        CombatController(IInputService& input, IBattleService& character, ISceneService& scene,
                         ICardService& card, IOSService& os)
            : inputService(input), battleService(character), sceneService(scene), cardService(card)
        {
            for (int i = 0; i < 4; ++i)
            {
                actionMenuRects[i] = {
                    ACTION_MENU_X,
                    ACTION_MENU_Y + i * ACTION_MENU_STEP_Y,
                    ACTION_MENU_X + ACTION_MENU_W,
                    ACTION_MENU_Y + i * ACTION_MENU_STEP_Y + ACTION_MENU_H
                };
            }
            actionMenu = std::make_unique<ButtonGroup>(actionMenuRects, input, os,
                                                       ButtonGroupLayout::Vertical, ACTION_MAGIC);
        }

        void Reset() override
        {
            actionMenu->SetFocusedIndex(ACTION_MAGIC);
            isMagicMenuOpen = false;
            EventBus::Publish(ActionSelectionEvent(actionMenu->GetFocusedIndex(), isMagicMenuOpen, true));
        }

        void Update(float deltaTime) override
        {
            actionMenu->Update();

            // フォーカス変化を ActionSelectionEvent として発行
            const int newIndex = actionMenu->GetFocusedIndex();
            if (newIndex != lastPublishedIndex)
            {
                lastPublishedIndex = newIndex;
                EventBus::Publish(ActionSelectionEvent(newIndex, isMagicMenuOpen, true));
            }

            HandleKeyboardInput();
            HandleMouseClick();
        }

    private:
        void HandleKeyboardInput()
        {
            if (inputService.IsPressed(InputAction::Confirm))
            {
                HandleConfirm(actionMenu->GetFocusedIndex());
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
                sceneService.PushScene(ESceneState::Menu);
                return;
            }
        }

        void HandleConfirm(int index)
        {
            if (index == ACTION_MAGIC)
            {
                isMagicMenuOpen = !isMagicMenuOpen;
                EventBus::Publish(ActionSelectionEvent(index, isMagicMenuOpen));
                return;
            }
            if (isMagicMenuOpen)
            {
                if (TryUseMagic(index))
                {
                    isMagicMenuOpen = false;
                    actionMenu->SetFocusedIndex(ACTION_MAGIC);
                    lastPublishedIndex = ACTION_MAGIC;
                    EventBus::Publish(ActionSelectionEvent(ACTION_MAGIC, isMagicMenuOpen, true));
                }
                return;
            }
            ResolveAttack(index);
        }

        bool TryUseMagic(int index) const
        {
            switch (index)
            {
            case 1:
                {
                    bool success = battleService.GetPlayer().UseMagic(EMagic::Clairvoyance);
                    if (success) battleService.GetEnemy().SetExposed(true);
                    return success;
                }
            case 2:
                return battleService.GetPlayer().UseMagic(EMagic::PowerBoost);
            case 3:
                return battleService.GetPlayer().UseMagic(EMagic::Heal);
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

        void ResolveAttack(int actionIndex)
        {
            auto playerAttackIntentOpt = GetAttackTypeFromIndex(actionIndex);
            if (!playerAttackIntentOpt) return;
            EAttackType playerAttackIntent = *playerAttackIntentOpt;

            EAttackType enemyAttackIntent = battleService.GetEnemy().GetAttackIntent();
            float playerWinRate = battleService.GetEnemy().GetLoseRateAgainst(playerAttackIntent);

            int rockOffset     = battleService.GetEnemy().GetWeightOffset(EAttackType::Rock);
            int scissorsOffset = battleService.GetEnemy().GetWeightOffset(EAttackType::Scissors);
            int paperOffset    = battleService.GetEnemy().GetWeightOffset(EAttackType::Paper);

            int playerDamage = battleService.GetPlayer().GetDamage(playerAttackIntent);
            int enemyDamage  = battleService.GetEnemy().GetDamage(enemyAttackIntent);

            if (LosesTo(playerAttackIntent, enemyAttackIntent))
                battleService.GetPlayer().TakeDamage(enemyDamage);
            if (LosesTo(enemyAttackIntent, playerAttackIntent))
                battleService.GetEnemy().TakeDamage(playerDamage);

            battleService.GetEnemy().ResetWeights();
            cardService.DiscardHand();
            battleService.GetPlayer().ResetDamageOffset();

            EventBus::Publish(
                CombatEvent(playerAttackIntent, enemyAttackIntent, playerDamage, enemyDamage,
                            playerWinRate, rockOffset, scissorsOffset, paperOffset)
            );

            battleService.SaveState(0);
        }

        void ProcessDrawCard(Card c) const
        {
            if (c.CardType == ECardType::Magic)
            {
                battleService.GetPlayer().ChangeMp(c.Power);
            }
            else if (c.CardType == ECardType::Rock || c.CardType == ECardType::Scissors ||
                     c.CardType == ECardType::Paper)
            {
                battleService.GetEnemy().AddWeight(ToAttackType(c.CardType), c.Power);
            }
        }

        void HandleMouseClick()
        {
            auto click = inputService.OnMouseClick(InputAction::MouseClick);
            if (click.x == -1 || click.y == -1) return;

            // 山札クリック
            if (click.In(DRAW_PILE_RECT))
            {
                ProcessDrawCard(cardService.DrawCard());
                EventBus::Publish(DrawCardEvent());
                return;
            }

            // ActionMenu クリック — クリック座標でヒットテストして行を特定する
            for (int i = 0; i < actionMenu->Count(); ++i)
            {
                const Rect<int>& r = actionMenuRects[i];
                if (click.x >= r.x1 && click.x < r.x2 && click.y >= r.y1 && click.y < r.y2)
                {
                    if (isMagicMenuOpen && i >= 1)
                    {
                        if (TryUseMagic(i))
                        {
                            isMagicMenuOpen = false;
                            actionMenu->SetFocusedIndex(ACTION_MAGIC);
                            lastPublishedIndex = ACTION_MAGIC;
                            EventBus::Publish(ActionSelectionEvent(ACTION_MAGIC, isMagicMenuOpen, true));
                        }
                        return;
                    }

                    if (actionMenu->GetFocusedIndex() == i && lastPublishedIndex == i)
                    {
                        // 同じ行を再クリック → Confirm 相当
                        HandleConfirm(i);
                    }
                    else
                    {
                        // 新しい行をクリック → フォーカス更新のみ（Update() 内で既に発行済み）
                        actionMenu->SetFocusedIndex(i);
                        if (lastPublishedIndex != i)
                        {
                            lastPublishedIndex = i;
                            EventBus::Publish(ActionSelectionEvent(i, isMagicMenuOpen, true));
                        }
                    }
                    return;
                }
            }
        }

        IInputService& inputService;
        IBattleService& battleService;
        ISceneService& sceneService;
        ICardService& cardService;

        std::array<Rect<int>, 4> actionMenuRects{};
        std::unique_ptr<ButtonGroup> actionMenu;
        int lastPublishedIndex = ACTION_MAGIC;
        bool isMagicMenuOpen = false;
    };

    std::unique_ptr<ICombatController> CreateCombatController(IInputService& inputService,
                                                              IBattleService& battleService,
                                                              ISceneService& sceneService,
                                                              ICardService& cardService,
                                                              IOSService& osService)
    {
        return std::make_unique<CombatController>(inputService, battleService, sceneService,
                                                  cardService, osService);
    }
}
