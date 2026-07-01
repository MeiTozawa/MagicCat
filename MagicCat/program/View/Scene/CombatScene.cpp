module;

#include <memory>
#include <format>
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

        // 各 Displayer の raw pointer（effector を追加するために保持）
        Displayer* playerAnimDisp = nullptr;
        Displayer* enemyAnimDisp = nullptr;
        AttackDisplayer* playerAttack = nullptr;
        AttackDisplayer* enemyAttack = nullptr;
        DialogDisplayer* playerDialog = nullptr;

        std::wstring progressText;

    public:
        CombatScene(ISceneService& scene, IAssetService& asset, ICardService& card,
                    IInputService& input, IRenderService& render, IBattleService& battle)
            : sceneService(scene), assetService(asset), cardService(card),
              inputService(input), renderService(render), battleService(battle) {}

        void Start() override
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

            displayers.clear();
            displayers.push_back(CreateCardDisplayer(cardService, assetService, renderService));
            displayers.push_back(CreateCharacterDisplayer(battleService, renderService));
            displayers.push_back(CreateControlDisplayer(assetService, renderService, inputService));

            combatController = CreateCombatController(inputService, battleService, sceneService, cardService);
            combatController->Reset();

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

            progressText = std::format(L"敵 {}/{}",
                battleService.GetCurrentEnemyIndex() + 1,
                battleService.GetTotalEnemyCount());

            healthChangedEvent = EventBus::Subscribe<HealthChangedEvent>([this](const HealthChangedEvent& event)
            {
                auto tags = event.Victim->GetTags();
                if (std::ranges::find(tags, ETag::Player) != tags.end())
                    playerAnimDisp->AddEffector(CreateHitFlashEffector(renderService, 0xFF0000));
                else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                    enemyAnimDisp->AddEffector(CreateHitFlashEffector(renderService, 0xFF0000));
            });

            combatEvent = EventBus::Subscribe<CombatEvent>([this](const CombatEvent& event)
            {
                playerAttack->SetImage(assetService.GetImageHandle(ToImage(event.playerAttackType)));
                enemyAttack->SetImage(assetService.GetImageHandle(ToImage(event.enemyAttackType)));

                playerAttack->ResetAndAddEffector(
                    CreateFadeEffector(renderService, ATTACK_FADE_IN_TIME, ATTACK_HOLD_TIME, ATTACK_FADE_OUT_TIME));
                enemyAttack->ResetAndAddEffector(
                    CreateFadeEffector(renderService, ATTACK_FADE_IN_TIME, ATTACK_HOLD_TIME, ATTACK_FADE_OUT_TIME));

                // クッソー条件：
                //   敵のいずれかの offset が他の2手の offset に対して差分 >= 閾値
                //   かつ プレイヤーがその「正解手」を打っていた
                //   かつ それでも負けた
                if (LosesTo(event.playerAttackType, event.enemyAttackType))
                {
                    const int offsets[3] = {
                        event.enemyWeightOffsets[0], // Rock
                        event.enemyWeightOffsets[1], // Scissors
                        event.enemyWeightOffsets[2]  // Paper
                    };
                    constexpr EAttackType types[3] = {
                        EAttackType::Rock, EAttackType::Scissors, EAttackType::Paper
                    };

                    bool playedCorrectCounter = false;
                    for (int i = 0; i < 3; ++i)
                    {
                        bool dominates = true;
                        for (int j = 0; j < 3; ++j)
                        {
                            if (i == j) continue;
                            if (offsets[i] - offsets[j] < KUSSOU_WEIGHT_DIFF_THRESHOLD)
                            {
                                dominates = false;
                                break;
                            }
                        }
                        if (dominates)
                        {
                            if (LosesTo(types[i], event.playerAttackType))
                            {
                                playedCorrectCounter = true;
                                break;
                            }
                        }
                    }

                    if (playedCorrectCounter)
                    {
                        playerDialog->SetMessage(L"クッソー", DIALOG_COLOR_DAMN);
                        playerDialog->ResetAndAddEffector(
                            CreateFadeEffector(renderService, DIALOG_FADE_IN_TIME, DIALOG_HOLD_TIME, DIALOG_FADE_OUT_TIME));
                    }
                }
            });

            stageClearHandle = EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent&)
            {
                progressText = L"全クリア！";
            });
        }

        ~CombatScene() override
        {
            if (healthChangedEvent.has_value()) EventBus::Unsubscribe(*healthChangedEvent);
            if (combatEvent.has_value()) EventBus::Unsubscribe(*combatEvent);
            if (stageClearHandle.has_value()) EventBus::Unsubscribe(*stageClearHandle);
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
                                              IBattleService& battleService)
    {
        return std::make_unique<CombatScene>(sceneService, assetService, cardService, inputService,
                                             renderService, battleService);
    }
} // namespace mc
