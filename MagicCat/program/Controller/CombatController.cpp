module;

#include <memory>

module CombatController;

import ServiceLocator;
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
        IInputService* inputService = nullptr;
        ICharacterService* characterService = nullptr;
        int selectedActionIndex = 0;

    public:
        CombatController()
        {
            inputService = ServiceLocator::Get<IInputService>();
            characterService = ServiceLocator::Get<ICharacterService>();
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
                if (selectedActionIndex > ACTION_MAGIC)
                {
                    EAttackType playerAttackIntent;
                    if (selectedActionIndex == ACTION_ROCK) playerAttackIntent = EAttackType::Rock;
                    else if (selectedActionIndex == ACTION_SCISSORS) playerAttackIntent = EAttackType::Scissors;
                    else if (selectedActionIndex == ACTION_PAPER) playerAttackIntent = EAttackType::Paper;
                    else return;

                    EAttackType enemyAttackIntent = characterService->GetEnemy().GetAttackIntent();
                    EventBus::Publish(
                        CombatEvent(playerAttackIntent, enemyAttackIntent,
                                    characterService->GetPlayer().GetDamage(playerAttackIntent),
                                    characterService->GetEnemy().GetDamage(enemyAttackIntent)
                        ));
                }
            }
            else if (inputService->IsPressed(InputAction::IgDrawCard))
            {
                EventBus::Publish(DrawCardEvent());
            }
            else if (inputService->IsPressed(InputAction::IgShowRules))
            {
                ServiceLocator::Get<ISceneService>()->PushScene(Rules);
            }
        }
    };

    std::unique_ptr<ICombatController> CreateCombatController()
    {
        return std::make_unique<CombatController>();
    }
}
