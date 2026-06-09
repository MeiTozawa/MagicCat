module;

#include <dxe.h>
#include <vector>
#include <memory>

module SceneService;

import CardView;
import CharacterView;
import CharacterService;
import SceneService;
import InputService;
import EventBus;


class CombatScene : public IScene
{
    std::unique_ptr<CardView> cardView;
    std::unique_ptr<CharacterView> characterView;
    Shared<ICharacterService> characterService;
    Shared<ISceneService> sceneService;
    Shared<IInputService> inputService;
    bool readyToAttack = false;

public:
    CombatScene() {}

    void Start() override
    {
        cardView = std::make_unique<CardView>();
        characterView = std::make_unique<CharacterView>();
        characterService = ServiceLocator::Get<ICharacterService>();
        sceneService = ServiceLocator::Get<ISceneService>();
        inputService = ServiceLocator::Get<IInputService>();
    }


    void Update(float deltaTime) override
    {
        if (inputService->IsPressed(InputAction::IgLeft))
        {
            EventBus::Publish(MoveFocusToLeftEvent());
        }
        else if (inputService->IsPressed(InputAction::IgRight))
        {
            EventBus::Publish(MoveFocusToRightEvent());
        }
        else if (inputService->IsPressed(InputAction::IgDrawCard))
        {
            EventBus::Publish(DrawCardEvent());
        }
        else if (inputService->IsPressed(InputAction::IgCancel))
        {
            readyToAttack = false;
        }
        else if (inputService->IsPressed(InputAction::IgAttack))
        {
            readyToAttack = true;
        }

        cardView->PrintCards();
        cardView->PrintDrawPile();
        cardView->PrintDiscardPile();
        
    }
};

static struct RegisterCombatScene
{
    RegisterCombatScene()
    {
        SceneRegistry::GetRegistrations().push_back([]()
        {
            auto manager = ServiceLocator::Get<ISceneService>();
            if (manager)
            {
                manager->RegisterScene(COMBAT, std::make_unique<CombatScene>());
            }
        });
    }
} autoRegister_CombatScene;
