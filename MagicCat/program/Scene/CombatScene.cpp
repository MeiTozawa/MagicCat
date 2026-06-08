module;

#include <dxe.h>
#include <vector>
#include <memory>
#include <span>

module SceneService;

import CardView;
import CharacterView;


class CombatScene : public IScene
{
    std::unique_ptr<CardView> cardView;
    std::unique_ptr<CharacterView> characterView;

public:
    CombatScene(){}
    
    void Start() override
    {
        cardView = std::make_unique<CardView>();
        characterView = std::make_unique<CharacterView>();
    }


    void Update(float deltaTime) override
    {
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
                manager->RegisterScene(COMBAT, std::make_shared<CombatScene>());
            }
        });
    }
} autoRegister_CombatScene;
