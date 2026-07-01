module;

#include <memory>

module CombatController;

import InputService;
import BattleService;
import EventBus;
import CardService;
import Character;
import Player;
import SceneService;

namespace mc {
    // ActionMenu HitBox (same values as CharacterDisplayer.ixx constants)
    constexpr int MC_ACTION_START_X  = 400;   // PLAYER_DAMAGE_START_X
    constexpr int MC_ACTION_START_Y  = 200;   // PLAYER_DAMAGE_START_Y
    constexpr int MC_ACTION_RECT_W   = 300;   // RECT_X
    constexpr int MC_ACTION_RECT_H   = 100;   // RECT_Y
    constexpr int MC_ACTION_OFFSET_Y = 120;   // OFFSET_Y

    // DrawCard HitBox (same values as CardDisplayer.ixx constants)
    constexpr int MC_DRAW_CARD_X1 = 50;
    constexpr int MC_DRAW_CARD_Y1 = 400;
    constexpr int MC_DRAW_CARD_X2 = 250;   // DRAW_PILE_X + CARD_WIDTH
    constexpr int MC_DRAW_CARD_Y2 = 700;   // DRAW_PILE_Y + CARD_HEIGHT

    class CombatController : public ICombatController
    {
        IInputService& inputService;
        IBattleService& characterService;
        ISceneService& sceneService;
        ICardService& cardService;
        int selectedActionIndex = 0;
        bool isMagicMenuOpen = false;

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
                if (selectedActionIndex == ACTION_MAGIC)
                {
                    isMagicMenuOpen = !isMagicMenuOpen;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
                else
                {
                    if (isMagicMenuOpen)
                    {
                        bool success = false;
                        if (selectedActionIndex == 1)
                        {
                            success = characterService.GetPlayer().UseMagic(EMagic::Clairvoyance);
                            if (success)
                            {
                                characterService.GetEnemy().SetExposed(true);
                            }
                        }
                        else if (selectedActionIndex == 2)
                        {
                            success = characterService.GetPlayer().UseMagic(EMagic::PowerBoost);
                        }
                        else if (selectedActionIndex == 3)
                        {
                            success = characterService.GetPlayer().UseMagic(EMagic::Heal);
                        }

                        if (success)
                        {
                            isMagicMenuOpen = false;
                            selectedActionIndex = ACTION_MAGIC;
                            EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                        }
                    }
                    else
                    {
                        EAttackType playerAttackIntent;
                        if (selectedActionIndex == ACTION_ROCK) playerAttackIntent = EAttackType::Rock;
                        else if (selectedActionIndex == ACTION_SCISSORS) playerAttackIntent = EAttackType::Scissors;
                        else if (selectedActionIndex == ACTION_PAPER) playerAttackIntent = EAttackType::Paper;
                        else return;

                        EAttackType enemyAttackIntent = characterService.GetEnemy().GetAttackIntent();

                        float playerWinRate = characterService.GetEnemy().GetLoseRateAgainst(playerAttackIntent);

                        int rockOffset     = characterService.GetEnemy().GetWeightOffset(EAttackType::Rock);
                        int scissorsOffset = characterService.GetEnemy().GetWeightOffset(EAttackType::Scissors);
                        int paperOffset    = characterService.GetEnemy().GetWeightOffset(EAttackType::Paper);

                        int playerDamage = characterService.GetPlayer().GetDamage(playerAttackIntent);
                        int enemyDamage = characterService.GetEnemy().GetDamage(enemyAttackIntent);

                        if (LosesTo(playerAttackIntent, enemyAttackIntent))
                        {
                            characterService.GetPlayer().TakeDamage(enemyDamage);
                        }
                        if (LosesTo(enemyAttackIntent, playerAttackIntent))
                        {
                            characterService.GetEnemy().TakeDamage(playerDamage);
                        }

                        characterService.GetEnemy().ResetWeights();
                        cardService.DiscardHand();
                        characterService.GetPlayer().ResetDamageOffset();

                        EventBus::Publish(
                            CombatEvent(playerAttackIntent, enemyAttackIntent, playerDamage, enemyDamage,
                                        playerWinRate, rockOffset, scissorsOffset, paperOffset)
                        );
                    }
                }
            }
            else if (inputService.IsPressed(InputAction::DrawCard))
            {
                auto c = cardService.DrawCard();
                if (c.CardType == ECardType::Magic)
                {
                    characterService.GetPlayer().ChangeMp(c.Power);
                }
                else if (c.CardType == ECardType::Rock || c.CardType == ECardType::Scissors || c.CardType ==
                    ECardType::Paper)
                {
                    characterService.GetEnemy().AddWeight(ToAttackType(c.CardType), c.Power);
                }
                EventBus::Publish(DrawCardEvent());
            }
            else if (inputService.IsPressed(InputAction::ToggleMenu))
            {
                sceneService.PushScene(ESceneState::Rules);
            }

            // Mouse hover → update focus (no action, just selection highlight)
            {
                auto mousePos = inputService.GetMousePosition();
                int mx = mousePos.x;
                int my = mousePos.y;
                for (int i = 0; i < 4; ++i)
                {
                    int hx1 = MC_ACTION_START_X;
                    int hy1 = MC_ACTION_START_Y + i * MC_ACTION_OFFSET_Y;
                    int hx2 = hx1 + MC_ACTION_RECT_W;
                    int hy2 = hy1 + MC_ACTION_RECT_H;
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

            // Mouse click handling
            auto click = inputService.OnMouseClick(InputAction::MouseClick);
            if (click.x != -1 && click.y != -1)
            {
                // DrawCard HitBox
                if (click.x >= MC_DRAW_CARD_X1 && click.x < MC_DRAW_CARD_X2 &&
                    click.y >= MC_DRAW_CARD_Y1 && click.y < MC_DRAW_CARD_Y2)
                {
                    auto c = cardService.DrawCard();
                    if (c.CardType == ECardType::Magic)
                    {
                        characterService.GetPlayer().ChangeMp(c.Power);
                    }
                    else if (c.CardType == ECardType::Rock || c.CardType == ECardType::Scissors || c.CardType == ECardType::Paper)
                    {
                        characterService.GetEnemy().AddWeight(ToAttackType(c.CardType), c.Power);
                    }
                    EventBus::Publish(DrawCardEvent());
                    return;
                }

                // ActionMenu row HitBox
                for (int i = 0; i < 4; ++i)
                {
                    int x1 = MC_ACTION_START_X;
                    int y1 = MC_ACTION_START_Y + i * MC_ACTION_OFFSET_Y;
                    int x2 = x1 + MC_ACTION_RECT_W;
                    int y2 = y1 + MC_ACTION_RECT_H;
                    if (click.x >= x1 && click.x < x2 && click.y >= y1 && click.y < y2)
                    {
                        if (isMagicMenuOpen && i >= 1)
                        {
                            // MagicSubMenu: j=i (1=Clairvoyance, 2=PowerBoost, 3=Heal)
                            bool success = false;
                            if (i == 1)
                            {
                                success = characterService.GetPlayer().UseMagic(EMagic::Clairvoyance);
                                if (success)
                                {
                                    characterService.GetEnemy().SetExposed(true);
                                }
                            }
                            else if (i == 2)
                            {
                                success = characterService.GetPlayer().UseMagic(EMagic::PowerBoost);
                            }
                            else if (i == 3)
                            {
                                success = characterService.GetPlayer().UseMagic(EMagic::Heal);
                            }
                            if (success)
                            {
                                isMagicMenuOpen = false;
                                selectedActionIndex = ACTION_MAGIC;
                                EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                            }
                        }
                        else if (selectedActionIndex == i)
                        {
                            // Same row re-click → Confirm equivalent
                            if (selectedActionIndex == ACTION_MAGIC)
                            {
                                isMagicMenuOpen = !isMagicMenuOpen;
                                EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                            }
                            else
                            {
                                EAttackType playerAttackIntent;
                                if (selectedActionIndex == ACTION_ROCK) playerAttackIntent = EAttackType::Rock;
                                else if (selectedActionIndex == ACTION_SCISSORS) playerAttackIntent = EAttackType::Scissors;
                                else if (selectedActionIndex == ACTION_PAPER) playerAttackIntent = EAttackType::Paper;
                                else return;

                                EAttackType enemyAttackIntent = characterService.GetEnemy().GetAttackIntent();

                                float playerWinRate = characterService.GetEnemy().GetLoseRateAgainst(playerAttackIntent);

                                int rockOffset     = characterService.GetEnemy().GetWeightOffset(EAttackType::Rock);
                                int scissorsOffset = characterService.GetEnemy().GetWeightOffset(EAttackType::Scissors);
                                int paperOffset    = characterService.GetEnemy().GetWeightOffset(EAttackType::Paper);

                                int playerDamage = characterService.GetPlayer().GetDamage(playerAttackIntent);
                                int enemyDamage  = characterService.GetEnemy().GetDamage(enemyAttackIntent);

                                if (LosesTo(playerAttackIntent, enemyAttackIntent))
                                {
                                    characterService.GetPlayer().TakeDamage(enemyDamage);
                                }
                                if (LosesTo(enemyAttackIntent, playerAttackIntent))
                                {
                                    characterService.GetEnemy().TakeDamage(playerDamage);
                                }

                                characterService.GetEnemy().ResetWeights();
                                cardService.DiscardHand();
                                characterService.GetPlayer().ResetDamageOffset();

                                EventBus::Publish(
                                    CombatEvent(playerAttackIntent, enemyAttackIntent, playerDamage, enemyDamage,
                                                playerWinRate, rockOffset, scissorsOffset, paperOffset)
                                );
                            }
                        }
                        else
                        {
                            // Different row click → select it
                            selectedActionIndex = i;
                            EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                        }
                        return;
                    }
                }
            }
        }
    };

    std::unique_ptr<ICombatController> CreateCombatController(IInputService& inputService,
                                                              IBattleService& characterService,
                                                              ISceneService& sceneService, ICardService& cardService)
    {
        return std::make_unique<CombatController>(inputService, characterService, sceneService, cardService);
    }
}
