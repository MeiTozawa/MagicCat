module;

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

    constexpr float PLAYER_ATTACK_X = 820.f;
    constexpr float PLAYER_ATTACK_Y = 400.f;
    constexpr float ENEMY_ATTACK_X = 1020.f;
    constexpr float ENEMY_ATTACK_Y = 400.f;
    constexpr float ATTACK_IMAGE_SCALE = 0.2f;

    constexpr int ACTION_MAGIC = 0;
    constexpr int ACTION_ROCK = 1;
    constexpr int ACTION_SCISSORS = 2;
    constexpr int ACTION_PAPER = 3;
    constexpr int ACTION_MAX = ACTION_PAPER;

    class CombatScene : public IScene
    {
        std::unique_ptr<CardView> cardView;
        std::unique_ptr<IAnimationPlayer> playerAnimation;
        std::unique_ptr<IAnimationPlayer> enemyAnimation;
        std::unique_ptr<IAnimationPlayer> playerAttackAnimation = nullptr;
        std::unique_ptr<IAnimationPlayer> enemyAttackAnimation = nullptr;
        std::unique_ptr<CharacterView> characterView;
        std::unique_ptr<ControlView> controlView;
        ICharacterService* characterService = nullptr;
        ISceneService* sceneService = nullptr;
        IInputService* inputService = nullptr;
        IAssetService* assetService = nullptr;
        int selectedActionIndex = ACTION_MAGIC;

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
                if (selectedActionIndex > ACTION_MAGIC)
                    selectedActionIndex--;
            }
            else if (inputService->IsPressed(InputAction::IgDown))
            {
                if (selectedActionIndex < ACTION_MAX)
                    selectedActionIndex++;
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

                    playerAttackAnimation = CreateAttackAnimation(
                        PLAYER_ATTACK_X, PLAYER_ATTACK_Y, ATTACK_IMAGE_SCALE, playerAttackIntent
                    );
                    enemyAttackAnimation = CreateAttackAnimation(
                        ENEMY_ATTACK_X, ENEMY_ATTACK_Y, ATTACK_IMAGE_SCALE, enemyAttackIntent
                    );
                    inputService->PushContext(InputContext::CutScene);
                }
            }
            else if (inputService->IsPressed(InputAction::IgDrawCard))
            {
                EventBus::Publish(DrawCardEvent());
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

            if (playerAttackAnimation != nullptr && enemyAttackAnimation != nullptr)
            {
                if (playerAttackAnimation->IsPlaying() == false && enemyAttackAnimation->IsPlaying() == false)
                {
                    inputService->PopContext();
                    playerAttackAnimation = nullptr;
                    enemyAttackAnimation = nullptr;
                }
                else
                {
                    playerAttackAnimation->Update(deltaTime);
                    enemyAttackAnimation->Update(deltaTime);
                }
            }
        }
    };

    std::unique_ptr<IScene> CreateCombatScene()
    {
        return std::make_unique<CombatScene>();
    }
} // namespace mc
