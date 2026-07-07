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
            : inputService(input), characterService(character), sceneService(scene), cardService(card)
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
                EventBus::Publish(ActionSelectionEvent(newIndex, isMagicMenuOpen));
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
                    EventBus::Publish(ActionSelectionEvent(ACTION_MAGIC, isMagicMenuOpen));
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
            else if (c.CardType == ECardType::Rock || c.CardType == ECardType::Scissors ||
                     c.CardType == ECardType::Paper)
            {
                characterService.GetEnemy().AddWeight(ToAttackType(c.CardType), c.Power);
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
                            EventBus::Publish(ActionSelectionEvent(ACTION_MAGIC, isMagicMenuOpen));
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
                            EventBus::Publish(ActionSelectionEvent(i, isMagicMenuOpen));
                        }
                    }
                    return;
                }
            }
        }

        IInputService& inputService;
        IBattleService& characterService;
        ISceneService& sceneService;
        ICardService& cardService;

        std::array<Rect<int>, 4> actionMenuRects{};
        std::unique_ptr<ButtonGroup> actionMenu;
        int lastPublishedIndex = ACTION_MAGIC;
        bool isMagicMenuOpen = false;
    };

    std::unique_ptr<ICombatController> CreateCombatController(IInputService& inputService,
                                                              IBattleService& characterService,
                                                              ISceneService& sceneService,
                                                              ICardService& cardService,
                                                              IOSService& osService)
    {
        return std::make_unique<CombatController>(inputService, characterService, sceneService,
                                                  cardService, osService);
    }
}
