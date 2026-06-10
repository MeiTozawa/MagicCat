module;

#include <dxe.h>
#include <vector>
#include <memory>

module SceneService;

import CardView;
import CharacterView;
import CharacterService;
import CardService;
import SceneService;
import InputService;
import EventBus;
import DataView;
import ControlView;
import Character;

namespace mc {


class CombatScene : public IScene
{
    std::unique_ptr<CardView> cardView;
    std::unique_ptr<SpriteView> spriteView;
    std::unique_ptr<CharacterView> characterView;
    std::unique_ptr<ControlView> controlView;
    ICharacterService* characterService;
    ISceneService* sceneService;
    IInputService* inputService;
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
        else if (inputService->IsPressed(InputAction::IgCombat))
        {
            if (focus > 0)
            {
                EAttackType playerAttackIntent = static_cast<EAttackType>(focus - 1);
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
        else if (inputService->IsPressed(InputAction::IgCancel))
        {
            readyToAttack = false;
        }
        else if (inputService->IsPressed(InputAction::IgCombat))
        {
            readyToAttack = true;
        }

        cardView->PrintCards();
        cardView->PrintDrawPile();
        cardView->PrintDiscardPile();
        spriteView->PrintSprites(deltaTime);
        characterView->PrintEnemyInfo();
        characterView->PrintPlayerInfo();
        characterView->PrintPlayerActions(focus);
        controlView->PrintControl();
    }
};

std::unique_ptr<IScene> CreateCombatScene()
{
    return std::make_unique<CombatScene>();
}

} // namespace mc

