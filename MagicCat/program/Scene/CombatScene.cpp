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


class CombatScene : public IScene
{
    std::unique_ptr<CardView> cardView;
    std::unique_ptr<CharacterView> characterView;
    std::unique_ptr<DataView> dataView;
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
        dataView = std::make_unique<DataView>();
        characterService = ServiceLocator::Get<ICharacterService>();
        sceneService = ServiceLocator::Get<ISceneService>();
        inputService = ServiceLocator::Get<IInputService>();
    }


    void Update(float deltaTime) override
    {
        if (inputService->IsPressed(InputAction::IgDrawCard))
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
        characterView->PrintSprites(deltaTime);
        dataView->PrintEnemyData();
        dataView->PrintPlayerData();
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
