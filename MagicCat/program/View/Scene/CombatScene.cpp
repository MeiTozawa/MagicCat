module;

#include <memory>
#include <format>
#include <string>

module SceneService;

import BattleService;
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
import RenderService;

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
        ISceneService& sceneService;
        IAssetService& assetService;
        ICardService& cardService;
        IInputService& inputService;
        IRenderService& renderService;
        IBattleService& BattleService;

        Displayers displayers;

        int playerAttackImageHandle = -1;
        int enemyAttackImageHandle = -1;
        std::unique_ptr<ICombatController> combatController;
        EventHandle healthChangedEvent = -1;
        EventHandle combatEvent = -1;
        EventHandle enemyDefeatedHandle = -1;
        EventHandle stageClearHandle = -1;

        std::wstring progressText;

    public:
        CombatScene(ISceneService& scene, IAssetService& asset, ICardService& card,
                    IInputService& input, IRenderService& render, IBattleService& battleSequence)
            : sceneService(scene), assetService(asset), cardService(card),
              inputService(input), renderService(render), BattleService(battleSequence) {}

        void Start() override
        {
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
            if (enemyDefeatedHandle != -1)
            {
                EventBus::Unsubscribe(enemyDefeatedHandle);
                enemyDefeatedHandle = -1;
            }
            if (stageClearHandle != -1)
            {
                EventBus::Unsubscribe(stageClearHandle);
                stageClearHandle = -1;
            }

            displayers.clear();
            displayers.push_back(CreateCardDisplayer(cardService, assetService, renderService));
            displayers.push_back(CreateCharacterDisplayer(BattleService, renderService));
            displayers.push_back(CreateControlDisplayer(assetService, renderService));

            combatController = CreateCombatController(inputService, BattleService, sceneService, cardService);
            combatController->Reset();

            auto playerAnimation = CreateSpriteAnimation(
                &assetService, BattleService.GetPlayer().GetSprite(), EXTRA_RATE
            );
            playerAnimation->SetPosition(PLAYER_START_X, PLAYER_START_Y);
            auto temp1 = CreateHitFlashEffector(std::move(playerAnimation), 0xFF0000);
            auto& playerFlashEffector = *temp1.get();
            displayers.push_back(std::move(temp1));

            auto enemyAnimation = CreateSpriteAnimation(
                &assetService, BattleService.GetEnemy().GetSprite(), EXTRA_RATE, true
            );
            enemyAnimation->SetPosition(ENEMY_START_X, ENEMY_START_Y);
            auto& enemyAnimationPlayer = *enemyAnimation;
            auto temp2 = CreateHitFlashEffector(std::move(enemyAnimation), 0xFF0000);
            auto& enemyFlashEffector = *temp2.get();
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

            progressText = std::format(L"敵 {}/3", BattleService.GetCurrentEnemyIndex() + 1);

            healthChangedEvent = EventBus::Subscribe<HealthChangedEvent>([&](const HealthChangedEvent& event)
            {
                auto tags = event.Victim->GetTags();
                if (std::ranges::find(tags, ETag::Player) != tags.end())
                    playerFlashEffector.Play();
                else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                    enemyFlashEffector.Play();
            });

            combatEvent = EventBus::Subscribe<CombatEvent>([&](const CombatEvent& event)
            {
                playerAttackImageHandle = assetService.GetImageHandle(ToImage(event.playerAttackType));
                enemyAttackImageHandle = assetService.GetImageHandle(ToImage(event.enemyAttackType));

                playerAttackEffector.Play();
                enemyAttackEffector.Play();
            });

            enemyDefeatedHandle = EventBus::Subscribe<EnemyDefeatedEvent>([&](const EnemyDefeatedEvent& e)
            {
                int idx = BattleService.GetCurrentEnemyIndex();
                progressText = std::format(L"敵 {}/3", idx + 1);
                enemyAnimationPlayer.SetSprite(BattleService.GetEnemy().GetSprite());
            });

            stageClearHandle = EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent& e)
            {
                progressText = L"全クリア！";
            });
        }

        ~CombatScene() override
        {
            if (healthChangedEvent != -1) EventBus::Unsubscribe(healthChangedEvent);
            if (combatEvent != -1) EventBus::Unsubscribe(combatEvent);
            if (enemyDefeatedHandle != -1) EventBus::Unsubscribe(enemyDefeatedHandle);
            if (stageClearHandle != -1) EventBus::Unsubscribe(stageClearHandle);
        }


        void Update(float deltaTime) override
        {
            combatController->Update(deltaTime);

            displayers.Update(deltaTime);
            displayers.Draw(deltaTime);

            renderService.DrawString(20, 10, progressText.c_str(), 0xFFFFFF);
        }
    };

    std::unique_ptr<IScene> CreateCombatScene(ISceneService& sceneService,
                                              IAssetService& assetService, ICardService& cardService,
                                              IInputService& inputService, IRenderService& renderService,
                                              IBattleService& BattleService)
    {
        return std::make_unique<CombatScene>(sceneService, assetService, cardService, inputService,
                                             renderService, BattleService);
    }
} // namespace mc
