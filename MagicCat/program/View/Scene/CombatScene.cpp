module;

#include <memory>
#include <optional>
#include <RenderUtils.h>
#include <string>

module SceneService;

import BattleService;
import CardService;
import InputService;
import EventBus;
import Displayer;
import Character;
import AssetService;
import EffectorFactory;
import CombatController;
import HealthComponent;
import Character;
import AssetEnumMapper;
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

    constexpr int ATTACK_FADE_IN_TIME = 150;
    constexpr int ATTACK_HOLD_TIME = 800;
    constexpr int ATTACK_FADE_OUT_TIME = 250;

    constexpr int PLAYER_DIALOG_X = PLAYER_START_X + 50;
    constexpr int PLAYER_DIALOG_Y = PLAYER_START_Y + 100;
    constexpr int DIALOG_FADE_IN_TIME = 150;
    constexpr int DIALOG_HOLD_TIME = 1000;
    constexpr int DIALOG_FADE_OUT_TIME = 250;
    /// @brief 敵のいずれかの手の weight offset が他の手との差分がこの値以上の場合、
    /// プレイヤーは「正解の手」を判断できるとみなす。
    constexpr int KUSSOU_WEIGHT_DIFF_THRESHOLD = 7;

    constexpr uint32_t DIALOG_COLOR_DAMN = COLOR_RED;

    class CombatScene : public IScene
    {
    public:
        CombatScene(ISceneService& scene, IAssetService& asset, ICardService& card,
                    IInputService& input, IRenderService& render, IBattleService& battle)
            : sceneService(scene), assetService(asset), cardService(card),
              inputService(input), renderService(render), battleService(battle) {}

        void Start() override
        {
            UnsubscribeAll();
            displayers.clear();
            SetupDisplayers();
            SetupController();
            SetupEventHandlers();
        }

        ~CombatScene() override
        {
            UnsubscribeAll();
        }

        void Update(float deltaTime) override
        {
            if (HandleMenuClick(deltaTime)) return;

            combatController->Update(deltaTime);
            displayers.Update(deltaTime);
            displayers.Draw(deltaTime);
        }

    private:
        void UnsubscribeAll()
        {
            if (healthChangedEvent.has_value())
            {
                EventBus::Unsubscribe(*healthChangedEvent);
                healthChangedEvent = std::nullopt;
            }
            if (combatEvent.has_value())
            {
                EventBus::Unsubscribe(*combatEvent);
                combatEvent = std::nullopt;
            }
            if (stageClearHandle.has_value())
            {
                EventBus::Unsubscribe(*stageClearHandle);
                stageClearHandle = std::nullopt;
            }
        }

        void SetupDisplayers()
        {
            displayers.push_back(CreateCardDisplayer(cardService, assetService, renderService));
            displayers.push_back(CreateCharacterDisplayer(battleService, renderService));
            displayers.push_back(CreateControlDisplayer(assetService, renderService, inputService));

            auto playerAnim = CreateSpriteDisplayer(&assetService, &renderService,
                                                    battleService.GetPlayer().GetSprite(), EXTRA_RATE);
            playerAnim->SetPosition(PLAYER_START_X, PLAYER_START_Y);
            playerAnimDisp = playerAnim.get();
            displayers.push_back(std::move(playerAnim));

            auto enemyAnim = CreateSpriteDisplayer(&assetService, &renderService,
                                                   battleService.GetEnemy().GetSprite(), EXTRA_RATE, true);
            enemyAnim->SetPosition(ENEMY_START_X, ENEMY_START_Y);
            enemyAnimDisp = enemyAnim.get();
            displayers.push_back(std::move(enemyAnim));

            auto pAtk = CreateAttackDisplayer(renderService, PLAYER_ATTACK_X, PLAYER_ATTACK_Y, ATTACK_IMAGE_SCALE);
            playerAttack = pAtk.get();
            displayers.push_back(std::move(pAtk));

            auto eAtk = CreateAttackDisplayer(renderService, ENEMY_ATTACK_X, ENEMY_ATTACK_Y, ATTACK_IMAGE_SCALE);
            enemyAttack = eAtk.get();
            displayers.push_back(std::move(eAtk));

            auto dlg = CreateDialogDisplayer(renderService, PLAYER_DIALOG_X, PLAYER_DIALOG_Y);
            playerDialog = dlg.get();
            displayers.push_back(std::move(dlg));
        }

        void SetupController()
        {
            combatController = CreateCombatController(inputService, battleService, sceneService, cardService);
            combatController->Reset();
        }

        void SetupEventHandlers()
        {
            healthChangedEvent = EventBus::Subscribe<HealthChangedEvent>(
                [this](const HealthChangedEvent& event)
                {
                    HandleHealthChanged(event);
                });

            combatEvent = EventBus::Subscribe<CombatEvent>(
                [this](const CombatEvent& event)
                {
                    OnCombatEvent(event);
                });
        }

        void OnCombatEvent(const CombatEvent& event) const
        {
            playerAttack->SetImage(assetService.GetImageHandle(ToImage(event.playerAttackType)));
            enemyAttack->SetImage(assetService.GetImageHandle(ToImage(event.enemyAttackType)));

            playerAttack->ResetAndAddEffector(
                CreateFadeEffector(renderService, ATTACK_FADE_IN_TIME, ATTACK_HOLD_TIME, ATTACK_FADE_OUT_TIME));
            enemyAttack->ResetAndAddEffector(
                CreateFadeEffector(renderService, ATTACK_FADE_IN_TIME, ATTACK_HOLD_TIME, ATTACK_FADE_OUT_TIME));

            if (LosesTo(event.playerAttackType, event.enemyAttackType) && DidPlayerPlayCorrectCounter(event))
            {
                playerDialog->SetMessage(L"クッソー", DIALOG_COLOR_DAMN);
                playerDialog->ResetAndAddEffector(
                    CreateFadeEffector(renderService, DIALOG_FADE_IN_TIME, DIALOG_HOLD_TIME, DIALOG_FADE_OUT_TIME));
            }
        }

        /// @brief 敵のいずれかの手の weight offset が他の手との差分がこの値以上の場合、
        /// プレイヤーは「正解の手」を判断できるとみなす。
        bool DidPlayerPlayCorrectCounter(const CombatEvent& event) const
        {
            const int offsets[3] = {
                event.enemyWeightOffsets[0], // Rock
                event.enemyWeightOffsets[1], // Scissors
                event.enemyWeightOffsets[2] // Paper
            };
            constexpr EAttackType types[3] = {
                EAttackType::Rock, EAttackType::Scissors, EAttackType::Paper
            };

            for (int i = 0; i < 3; ++i)
            {
                if (IsDominantHand(i, offsets) && LosesTo(types[i], event.playerAttackType))
                    return true;
            }
            return false;
        }

        /// @brief メニューボタンのマウスクリックを処理する。クリックされた場合 true を返す。
        bool HandleMenuClick(float deltaTime)
        {
            auto click = inputService.OnMouseClick(InputAction::MouseClick);
            if (click.x == -1 || click.y == -1) return false;

            const int menuIconX = renderService.GetWindowWidth() - MENU_ICON_X_OFFSET;
            if (click.x >= menuIconX - MENU_ICON_HALF_W && click.x < menuIconX + MENU_ICON_HALF_W &&
                click.y >= MENU_ICON_Y - MENU_ICON_HALF_H && click.y < MENU_ICON_Y + MENU_ICON_HALF_H)
            {
                sceneService.PushScene(ESceneState::Rules);
                displayers.Update(deltaTime);
                displayers.Draw(deltaTime);
                return true;
            }
            return false;
        }

        void HandleHealthChanged(const HealthChangedEvent& event) const
        {
            auto tags = event.Victim->GetTags();
            if (std::ranges::find(tags, ETag::Player) != tags.end())
                playerAnimDisp->AddEffector(CreateHitFlashEffector(renderService, 0xFF0000));
            else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                enemyAnimDisp->AddEffector(CreateHitFlashEffector(renderService, 0xFF0000));
        }

        bool IsDominantHand(int i, const int offsets[3]) const
        {
            for (int j = 0; j < 3; ++j)
            {
                if (i == j) continue;
                if (offsets[i] - offsets[j] < KUSSOU_WEIGHT_DIFF_THRESHOLD)
                    return false;
            }
            return true;
        }

        ISceneService& sceneService;
        IAssetService& assetService;
        ICardService& cardService;
        IInputService& inputService;
        IRenderService& renderService;
        IBattleService& battleService;

        Displayers displayers;
        std::unique_ptr<ICombatController> combatController;
        std::optional<EventHandle> healthChangedEvent;
        std::optional<EventHandle> combatEvent;
        std::optional<EventHandle> stageClearHandle;

        Displayer* playerAnimDisp = nullptr;
        Displayer* enemyAnimDisp = nullptr;
        AttackDisplayer* playerAttack = nullptr;
        AttackDisplayer* enemyAttack = nullptr;
        DialogDisplayer* playerDialog = nullptr;
    };

    std::unique_ptr<IScene> CreateCombatScene(ISceneService& sceneService,
                                              IAssetService& assetService, ICardService& cardService,
                                              IInputService& inputService, IRenderService& renderService,
                                              IBattleService& battleService)
    {
        return std::make_unique<CombatScene>(sceneService, assetService, cardService, inputService,
                                             renderService, battleService);
    }
} // namespace mc
