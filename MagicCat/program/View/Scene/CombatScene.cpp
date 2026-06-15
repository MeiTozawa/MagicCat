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
import CombatController;
import HealthComponent;
import Character;
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



    constexpr int FADE_IN_TIME = 200;
    constexpr int HOLD_TIME = 600;
    constexpr int FADE_OUT_TIME = 100;

    class CombatScene : public IScene
    {
        std::vector<std::unique_ptr<IDisplayer>> displayers;
        std::unique_ptr<EffectorPlayer> playerAnimationEffector;
        std::unique_ptr<EffectorPlayer> enemyAnimationEffector;
        std::unique_ptr<EffectorPlayer> playerAttackEffector = nullptr;
        std::unique_ptr<EffectorPlayer> enemyAttackEffector = nullptr;
        int playerAttackImageHandle = -1;
        int enemyAttackImageHandle = -1;
        ICharacterService* characterService = nullptr;
        ISceneService* sceneService = nullptr;
        IAssetService* assetService = nullptr;
        std::unique_ptr<ICombatController> combatController;
        EventHandle healthChangedEvent = -1;
        EventHandle combatEventHandle = -1;

    public:
        CombatScene() {}

        void Start() override
        {
            if (healthChangedEvent != -1)
            {
                EventBus::Unsubscribe(healthChangedEvent);
                healthChangedEvent = -1;
            }
            displayers.clear();
            displayers.push_back(CreateCardDisplayer());
            displayers.push_back(CreateCharacterDisplayer());
            displayers.push_back(CreateControlDisplayer());

            characterService = ServiceLocator::Get<ICharacterService>();
            sceneService = ServiceLocator::Get<ISceneService>();
            assetService = ServiceLocator::Get<IAssetService>();
            combatController = CreateCombatController();
            combatController->Reset();

            auto playerAnimation = CreateSpriteAnimation(
                assetService->GetSpriteHandle(ESprite::Bunny), EXTRA_RATE
            );
            playerAnimation->SetPosition(PLAYER_START_X, PLAYER_START_Y);
            playerAnimationEffector = CreateHitFlashEffector(std::move(playerAnimation));

            auto enemyAnimation = CreateSpriteAnimation(
                assetService->GetSpriteHandle(ESprite::Wolf), EXTRA_RATE, true
            );
            enemyAnimation->SetPosition(ENEMY_START_X, ENEMY_START_Y);
            enemyAnimationEffector = CreateHitFlashEffector(std::move(enemyAnimation));

            playerAttackEffector = CreateFadeEffector(
                CreateAttackDisplayer(
                    PLAYER_ATTACK_X, PLAYER_ATTACK_Y, ATTACK_IMAGE_SCALE,
                    &playerAttackImageHandle
                ), FADE_IN_TIME, HOLD_TIME, FADE_OUT_TIME
            );

            enemyAttackEffector = CreateFadeEffector(
                CreateAttackDisplayer(
                    ENEMY_ATTACK_X, ENEMY_ATTACK_Y, ATTACK_IMAGE_SCALE,
                    &enemyAttackImageHandle
                ), FADE_IN_TIME, HOLD_TIME, FADE_OUT_TIME
            );

            healthChangedEvent = EventBus::Subscribe<HealthChangedEvent>([this](const HealthChangedEvent& event)
            {
                auto tags = event.Victim->GetTags();
                if (std::ranges::find(tags, ETag::Player) != tags.end())
                    playerAnimationEffector->Play();
                else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                    enemyAnimationEffector->Play();
            });

            combatEventHandle = EventBus::Subscribe<CombatEvent>([this](const CombatEvent& event)
            {
                playerAttackImageHandle = assetService->GetImageHandle(static_cast<EImage>(event.playerAttackType));
                enemyAttackImageHandle = assetService->GetImageHandle(static_cast<EImage>(event.enemyAttackType));

                playerAttackEffector->Play();
                enemyAttackEffector->Play();
            });
        }
        
        ~CombatScene() override
        {
            if (healthChangedEvent != -1) EventBus::Unsubscribe(healthChangedEvent);
            if (combatEventHandle != -1) EventBus::Unsubscribe(combatEventHandle);
        }


        void Update(float deltaTime) override
        {
            combatController->Update(deltaTime);

            for (auto& displayer : displayers)
            {
                displayer->Update(deltaTime);
                displayer->Draw(deltaTime);
            }
            playerAnimationEffector->Update(deltaTime);
            playerAnimationEffector->Draw(deltaTime);
            enemyAnimationEffector->Update(deltaTime);
            enemyAnimationEffector->Draw(deltaTime);
            playerAttackEffector->Update(deltaTime);
            playerAttackEffector->Draw(deltaTime);
            enemyAttackEffector->Update(deltaTime);
            enemyAttackEffector->Draw(deltaTime);
        }
    };

    std::unique_ptr<IScene> CreateCombatScene()
    {
        return std::make_unique<CombatScene>();
    }
} // namespace mc
