module;

#include <memory>

module SceneService;

import CharacterService;
import CardService;
import InputService;
import EventBus;
import Displayer;
import Character;
import AnimationFactory;
import AssetService;
import EffectorFactory;
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
    
    constexpr int FADE_IN_TIME = 100;
    constexpr int HOLD_TIME = 300;
    constexpr int FADE_OUT_TIME = 100;

    class CombatScene : public IScene
    {
        std::vector<std::unique_ptr<IDisplayer>> displayers;
        std::unique_ptr<AnimationPlayer> playerAnimation;
        std::unique_ptr<AnimationPlayer> enemyAnimation;
        std::unique_ptr<EffectorPlayer> playerAttackEffector = nullptr;
        std::unique_ptr<EffectorPlayer> enemyAttackEffector = nullptr;
        ICharacterService* characterService = nullptr;
        ISceneService* sceneService = nullptr;
        IInputService* inputService = nullptr;
        IAssetService* assetService = nullptr;
        int selectedActionIndex = ACTION_MAGIC;

    public:
        CombatScene() {}

        void Start() override
        {
            displayers.push_back(CreateCardDisplayer());
            displayers.push_back(CreateCharacterDisplayer());
            displayers.push_back(CreateControlDisplayer());
            characterService = ServiceLocator::Get<ICharacterService>();
            sceneService = ServiceLocator::Get<ISceneService>();
            inputService = ServiceLocator::Get<IInputService>();
            assetService = ServiceLocator::Get<IAssetService>();
            
            playerAnimation = CreateSpriteAnimation(
                assetService->GetSpriteHandle(ESprite::Bunny), EXTRA_RATE
            );
            playerAnimation->SetPosition(PLAYER_START_X, PLAYER_START_Y);
            
            enemyAnimation = CreateSpriteAnimation(
                assetService->GetSpriteHandle(ESprite::Wolf), EXTRA_RATE, true
            );
            enemyAnimation->SetPosition(ENEMY_START_X, ENEMY_START_Y);
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

                    playerAttackEffector = GetFadeEffector(CreateAttackDisplayer(
                        PLAYER_ATTACK_X, PLAYER_ATTACK_Y, ATTACK_IMAGE_SCALE, playerAttackIntent
                    ), FADE_IN_TIME, HOLD_TIME, FADE_OUT_TIME);
                    
                    enemyAttackEffector = GetFadeEffector(CreateAttackDisplayer(
                        ENEMY_ATTACK_X, ENEMY_ATTACK_Y, ATTACK_IMAGE_SCALE, enemyAttackIntent
                    ), FADE_IN_TIME, HOLD_TIME, FADE_OUT_TIME);
                    
                    inputService->PushContext(InputContext::CutScene);
                }
            }
            else if (inputService->IsPressed(InputAction::IgDrawCard))
            {
                EventBus::Publish(DrawCardEvent());
            }

            for (auto& displayer : displayers)
            {
                displayer->Update(deltaTime);
                displayer->Draw(deltaTime);
            }
            playerAnimation->Update(deltaTime);
            playerAnimation->Draw(deltaTime);
            enemyAnimation->Update(deltaTime);
            enemyAnimation->Draw(deltaTime);

            if (playerAttackEffector != nullptr && enemyAttackEffector != nullptr)
            {
                if (playerAttackEffector->IsPlaying() == false && enemyAttackEffector->IsPlaying() == false)
                {
                    inputService->PopContext();
                    playerAttackEffector = nullptr;
                    enemyAttackEffector = nullptr;
                }
                else
                {
                    playerAttackEffector->Update(deltaTime);
                    playerAttackEffector->Draw(deltaTime);
                    enemyAttackEffector->Update(deltaTime);
                    enemyAttackEffector->Draw(deltaTime);
                }
            }
        }
    };

    std::unique_ptr<IScene> CreateCombatScene()
    {
        return std::make_unique<CombatScene>();
    }
} // namespace mc
