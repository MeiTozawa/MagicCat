module;

#include <memory>

module CombatController;

import InputService;
import CharacterService;
import EventBus;
import CardService;
import Character;
import Player;
import SceneService;

namespace mc
{
    class CombatController : public ICombatController
    {
        IInputService& inputService;
        ICharacterService& characterService;
        ISceneService& sceneService;
        ICardService& cardService;
        int selectedActionIndex = 0;
        bool isMagicMenuOpen = false;

    public:
        CombatController(IInputService& input, ICharacterService& character, ISceneService& scene, ICardService& card)
            : inputService(input), characterService(character), sceneService(scene), cardService(card)
        {
        }

        void Reset() override
        {
            selectedActionIndex = 0;
            isMagicMenuOpen = false;
            EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
        }

        void Update(float deltaTime) override
        {
            if (inputService.IsPressed(InputAction::IgUp))
            {
                if (selectedActionIndex > ACTION_MAGIC)
                {
                    selectedActionIndex--;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
            }
            else if (inputService.IsPressed(InputAction::IgDown))
            {
                if (selectedActionIndex < ACTION_MAX)
                {
                    selectedActionIndex++;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex, isMagicMenuOpen));
                }
            }
            else if (inputService.IsPressed(InputAction::IgConfirm))
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
                        if (selectedActionIndex == 1) // Clairvoyance
                        {
                            success = characterService.GetPlayer().UseMagic(EMagic::Clairvoyance);
                            if (success)
                            {
                                characterService.GetEnemy().SetExposed(true);
                            }
                        }
                        else if (selectedActionIndex == 2) // PowerBoost
                        {
                            success = characterService.GetPlayer().UseMagic(EMagic::PowerBoost);
                        }
                        else if (selectedActionIndex == 3) // Heal
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
                        
                        int playerDamage = characterService.GetPlayer().GetBaseDamage(playerAttackIntent);
                        int enemyDamage = characterService.GetEnemy().GetBaseDamage(enemyAttackIntent);

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
                        characterService.GetPlayer().ResetAttackOffset();

                        EventBus::Publish(
                            CombatEvent(playerAttackIntent, enemyAttackIntent, playerDamage, enemyDamage)
                        );
                    }
                }
            }
            else if (inputService.IsPressed(InputAction::IgDrawCard))
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
            }
            else if (inputService.IsPressed(InputAction::IgShowRules))
            {
                sceneService.PushScene(ESceneState::Rules);
            }
        }
    };

    std::unique_ptr<ICombatController> CreateCombatController(IInputService& inputService, ICharacterService& characterService, ISceneService& sceneService, ICardService& cardService)
    {
        return std::make_unique<CombatController>(inputService, characterService, sceneService, cardService);
    }
}
