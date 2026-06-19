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
        IInputService* inputService;
        ICharacterService* characterService;
        ISceneService* sceneService;
        ICardService* cardService;
        int selectedActionIndex = 0;

    public:
        CombatController(IInputService* input, ICharacterService* character, ISceneService* scene, ICardService* card)
            : inputService(input), characterService(character), sceneService(scene), cardService(card)
        {
        }

        void Reset() override
        {
            selectedActionIndex = 0;
            EventBus::Publish(ActionSelectionEvent(selectedActionIndex));
        }

        void Update(float deltaTime) override
        {
            if (inputService->IsPressed(InputAction::IgUp))
            {
                if (selectedActionIndex > ACTION_MAGIC)
                {
                    selectedActionIndex--;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex));
                }
            }
            else if (inputService->IsPressed(InputAction::IgDown))
            {
                if (selectedActionIndex < ACTION_MAX)
                {
                    selectedActionIndex++;
                    EventBus::Publish(ActionSelectionEvent(selectedActionIndex));
                }
            }
            else if (inputService->IsPressed(InputAction::IgConfirm))
            {
                if (selectedActionIndex == ACTION_MAGIC)
                {
                    if (characterService->GetPlayer().UseMagic(EMagic::Clairvoyance))
                    {
                        characterService->GetEnemy().SetExposed(true);
                    }
                }
                else
                {
                    EAttackType playerAttackIntent;
                    if (selectedActionIndex == ACTION_ROCK) playerAttackIntent = EAttackType::Rock;
                    else if (selectedActionIndex == ACTION_SCISSORS) playerAttackIntent = EAttackType::Scissors;
                    else if (selectedActionIndex == ACTION_PAPER) playerAttackIntent = EAttackType::Paper;
                    else return;

                    EAttackType enemyAttackIntent = characterService->GetEnemy().GetAttackIntent();
                    
                    int playerDamage = characterService->GetPlayer().GetDamage(playerAttackIntent);
                    int enemyDamage = characterService->GetEnemy().GetDamage(enemyAttackIntent);

                    if (LosesTo(playerAttackIntent, enemyAttackIntent))
                    {
                        characterService->GetPlayer().TakeDamage(enemyDamage);
                    }
                    if (LosesTo(enemyAttackIntent, playerAttackIntent))
                    {
                        characterService->GetEnemy().TakeDamage(playerDamage);
                    }

                    characterService->GetEnemy().ResetWeights();
                    cardService->DiscardHand();

                    EventBus::Publish(
                        CombatEvent(playerAttackIntent, enemyAttackIntent, playerDamage, enemyDamage)
                    );
                }
            }
            else if (inputService->IsPressed(InputAction::IgDrawCard))
            {
                auto c = cardService->DrawCard();
                if (c.CardType == Magic)
                {
                    characterService->GetPlayer().ChangeMp(c.Value);
                }
                else if (c.CardType == Rock || c.CardType == Scissors || c.CardType == Paper)
                {
                    characterService->GetEnemy().AddWeight(ToAttackType(c.CardType), c.Value);
                }
                EventBus::Publish(DrawCardEvent());
            }
            else if (inputService->IsPressed(InputAction::IgShowRules))
            {
                sceneService->PushScene(Rules);
            }
        }
    };

    std::unique_ptr<ICombatController> CreateCombatController(IInputService* inputService, ICharacterService* characterService, ISceneService* sceneService, ICardService* cardService)
    {
        return std::make_unique<CombatController>(inputService, characterService, sceneService, cardService);
    }
}
