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
import DataView;
import ControlView;


class CombatScene : public IScene
{
    std::unique_ptr<CardView> cardView;
    std::unique_ptr<SpriteView> spriteView;
    std::unique_ptr<CharacterView> characterView;
    std::unique_ptr<ControlView> controlView;
    Shared<ICharacterService> characterService;
    Shared<ISceneService> sceneService;
    Shared<IInputService> inputService;
    bool readyToAttack = false;
    int focus = 0;

public:
    CombatScene() {}

    void Start() override
    {
        cardView = std::make_unique<CardView>();
        spriteView = std::make_unique<SpriteView>();
        characterView = std::make_unique<CharacterView>();
        controlView = std::make_unique<ControlView>();
        characterService = ServiceLocator::Get<ICharacterService>();
        sceneService = ServiceLocator::Get<ISceneService>();
        inputService = ServiceLocator::Get<IInputService>();
    }


    void Update(float deltaTime) override
    {
        if (inputService->IsPressed(InputAction::IgUp))
        {
            if (focus > 0)
                focus--;
        }
        else if (inputService->IsPressed(InputAction::IgDown))
        {
            if (focus < 3)
                focus++;
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
        spriteView->PrintSprites(deltaTime);
        characterView->PrintEnemyData();
        characterView->PrintPlayerActions(focus);
        controlView->PrintControl();
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
