module;

#include <dxe.h>
#include <vector>
#include <memory>

module SceneService;

import CardView;
import CharacterService;
import CardService;
import SceneService;
import InputService;
import EventBus;
import DataView;
import ControlView;
import Character;
import AnimationFactory;
import AssetService;

namespace mc
{
    constexpr int PLAYER_START_X = 800;
    constexpr int PLAYER_START_Y = 450;
    constexpr int ENEMY_START_X = 1000;
    constexpr int ENEMY_START_Y = 450;
    constexpr float EXTRA_RATE = 8.f;

    constexpr int ACTION_NONE = 0;
    constexpr int ACTION_ROCK = 1;
    constexpr int ACTION_SCISSORS = 2;
    constexpr int ACTION_PAPER = 3;
    constexpr int ACTION_MAX = ACTION_PAPER;
    
    class CombatScene : public IScene
    {
        std::unique_ptr<CardView> cardView;
        std::unique_ptr<IAnimationPlayer> playerAnimation;
        std::unique_ptr<IAnimationPlayer> enemyAnimation;
        std::unique_ptr<CharacterView> characterView;
        std::unique_ptr<ControlView> controlView;
        ICharacterService* characterService = nullptr;
        ISceneService* sceneService = nullptr;
        IInputService* inputService = nullptr;
        IAssetService* assetService = nullptr;
        bool readyToAttack = false;
        int selectedActionIndex = ACTION_NONE;

    public:
        CombatScene() {}

        void Start() override
        {
            cardView = std::make_unique<CardView>();
            characterView = std::make_unique<CharacterView>();
            controlView = std::make_unique<ControlView>();
            characterService = ServiceLocator::Get<ICharacterService>();
            sceneService = ServiceLocator::Get<ISceneService>();
            inputService = ServiceLocator::Get<IInputService>();
            assetService = ServiceLocator::Get<IAssetService>();
            playerAnimation = CreateSpriteAnimation(
                assetService->GetSpriteHandle(ESprite::Bunny), PLAYER_START_X, PLAYER_START_Y, EXTRA_RATE
            );
            enemyAnimation = CreateSpriteAnimation(
                assetService->GetSpriteHandle(ESprite::Wolf), ENEMY_START_X, ENEMY_START_Y, EXTRA_RATE, true
            );
        }


        void Update(float deltaTime) override
        {
            if (inputService->IsPressed(InputAction::IgUp))
            {
                if (selectedActionIndex > ACTION_NONE)
                    selectedActionIndex--;
            }
            else if (inputService->IsPressed(InputAction::IgDown))
            {
                if (selectedActionIndex < ACTION_MAX)
                    selectedActionIndex++;
            }
            else if (inputService->IsPressed(InputAction::IgCombat))
            {
                if (selectedActionIndex > ACTION_NONE)
                {
                    EAttackType playerAttackIntent = EAttackType::Rock;
                    if (selectedActionIndex == ACTION_ROCK) playerAttackIntent = EAttackType::Rock;
                    else if (selectedActionIndex == ACTION_SCISSORS) playerAttackIntent = EAttackType::Scissors;
                    else if (selectedActionIndex == ACTION_PAPER) playerAttackIntent = EAttackType::Paper;

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
            characterView->PrintEnemyInfo();
            characterView->PrintPlayerInfo();
            characterView->PrintPlayerActions(selectedActionIndex);
            controlView->PrintControl();
            playerAnimation->Update(deltaTime);
            enemyAnimation->Update(deltaTime);
        }
    };

    std::unique_ptr<IScene> CreateCombatScene()
    {
        return std::make_unique<CombatScene>();
    }
} // namespace mc
