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
import ViewEnumMapper;

namespace mc {
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
        ICharacterService& characterService;
        ISceneService& sceneService;
        IAssetService& assetService;
        ICardService& cardService;
        IInputService& inputService;
        IRenderService& renderService;

        Displayers displayers;

        int playerAttackImageHandle = -1;
        int enemyAttackImageHandle = -1;
        std::unique_ptr<ICombatController> combatController;
        EventHandle healthChangedEvent = -1;
        EventHandle combatEvent = -1;

    public:
        CombatScene(ICharacterService& character, ISceneService& scene, IAssetService& asset, ICardService& card,
                    IInputService& input, IRenderService& render)
            : characterService(character), sceneService(scene), assetService(asset), cardService(card),
              inputService(input), renderService(render) {}

        void Start() override
        {
            characterService.Start();
            cardService.Start();
            if (healthChangedEvent != -1)
            {
                EventBus::Unsubscribe(healthChangedEvent);
                healthChangedEvent = -1;
            }
            if (combatEvent != -1)
            {
                EventBus::Unsubscribe(combatEvent);
                combatEvent = -1;
            }
            displayers.clear();
            displayers.push_back(CreateCardDisplayer(cardService, assetService, renderService));
            displayers.push_back(CreateCharacterDisplayer(characterService, renderService));
            displayers.push_back(CreateControlDisplayer(assetService, renderService));

            combatController = CreateCombatController(inputService, characterService, sceneService, cardService);
            combatController->Reset();

            auto playerAnimation = CreateSpriteAnimation(
                &assetService, characterService.GetPlayer().GetSprite(), EXTRA_RATE
            );
            playerAnimation->SetPosition(PLAYER_START_X, PLAYER_START_Y);
            auto temp1 = CreateHitFlashEffector(std::move(playerAnimation), 0xFF0000);
            auto& playerAnimationEffector = *temp1.get();
            displayers.push_back(std::move(temp1));

            auto enemyAnimation = CreateSpriteAnimation(
                &assetService, characterService.GetEnemy().GetSprite(), EXTRA_RATE, true
            );
            enemyAnimation->SetPosition(ENEMY_START_X, ENEMY_START_Y);
            auto temp2 = CreateHitFlashEffector(std::move(enemyAnimation), 0xFF0000);
            auto& enemyAnimationEffector = *temp2.get();
            displayers.push_back(std::move(temp2));

            auto temp3 = CreateFadeEffector(
                CreateAttackDisplayer(
                    PLAYER_ATTACK_X, PLAYER_ATTACK_Y, ATTACK_IMAGE_SCALE,
                    &playerAttackImageHandle
                ), FADE_IN_TIME, HOLD_TIME, FADE_OUT_TIME
            );
            auto& playerAttackEffector = *temp3.get();
            displayers.push_back(std::move(temp3));

            auto temp4 = CreateFadeEffector(
                CreateAttackDisplayer(
                    ENEMY_ATTACK_X, ENEMY_ATTACK_Y, ATTACK_IMAGE_SCALE,
                    &enemyAttackImageHandle
                ), FADE_IN_TIME, HOLD_TIME, FADE_OUT_TIME
            );
            auto& enemyAttackEffector = *temp4.get();
            displayers.push_back(std::move(temp4));

            healthChangedEvent = EventBus::Subscribe<HealthChangedEvent>([&](const HealthChangedEvent& event)
            {
                auto tags = event.Victim->GetTags();
                if (std::ranges::find(tags, ETag::Player) != tags.end())
                    playerAnimationEffector.Play();
                else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                    enemyAnimationEffector.Play();
            });

            combatEvent = EventBus::Subscribe<CombatEvent>([&](const CombatEvent& event)
            {
                playerAttackImageHandle = assetService.GetImageHandle(ToImage(event.playerAttackType));
                enemyAttackImageHandle = assetService.GetImageHandle(ToImage(event.enemyAttackType));

                playerAttackEffector.Play();
                enemyAttackEffector.Play();
            });
        }

        ~CombatScene() override
        {
            if (healthChangedEvent != -1) EventBus::Unsubscribe(healthChangedEvent);
            if (combatEvent != -1) EventBus::Unsubscribe(combatEvent);
        }


        void Update(float deltaTime) override
        {
            combatController->Update(deltaTime);

            displayers.Update(deltaTime);
            displayers.Draw(deltaTime);
        }
    };

    std::unique_ptr<IScene> CreateCombatScene(ICharacterService& characterService, ISceneService& sceneService,
                                              IAssetService& assetService, ICardService& cardService,
                                              IInputService& inputService, IRenderService& renderService)
    {
        return std::make_unique<CombatScene>(characterService, sceneService, assetService, cardService, inputService,
                                             renderService);
    }
} // namespace mc
