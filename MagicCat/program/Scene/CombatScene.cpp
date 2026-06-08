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
        if (characterService->GetPlayer().IsDead())
        {
            sceneService->ChangeSceneTo(START);
            characterService->Reset();
        }
        if (auto p = inputService->OnMouseClick(InputAction::IgMouseClick); p.x >= 0 && p.y >= 0)
        {
            // TODO: Handling mouse events
        }
        if (inputService->IsPressed(InputAction::IgLeft))
        {
            EventBus::Publish(MoveFocusToLeftEvent());
        }
        else if (inputService->IsPressed(InputAction::IgRight))
        {
            EventBus::Publish(MoveFocusToRightEvent());
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
                manager->RegisterScene(COMBAT, std::unique_ptr<CombatScene>());
            }
        });
    }
} autoRegister_CombatScene;
