module;

#include <dxe.h>
#include <memory>
#include <RenderUtils.h>

module SceneService;
import BattleService;
import InputService;
import RenderService;
import EventBus;

namespace mc {
    class InfoScene : public IScene
    {
        IInputService& inputService;
        ISceneService& sceneService;
        IRenderService& renderService;
        IBattleService& battleService;
        EventHandle stageClearHandle;
        EventHandle stageFailHandle;
        std::wstring info = {};
        uint32_t infoColor;
        int winCount = 0;
        int failCount = 0;
        float blinkTimer = 0;

    public:
        InfoScene(IInputService& input, ISceneService& scene, IRenderService& render, IBattleService& battle)
            : inputService(input), sceneService(scene), renderService(render), battleService(battle)
        {
            infoColor = 0xF259FF;
            stageClearHandle = EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent&)
            {
                winCount++;
                info = L"勝利にゃあ！！";
                infoColor = 0xFFD700;
            });
            stageFailHandle = EventBus::Subscribe<StageFailEvent>([this](const StageFailEvent&)
            {
                failCount++;
                info = L"失敗にゃの！？";
                infoColor = 0xff3333;
            });
        }

        ~InfoScene() override
        {
            EventBus::Unsubscribe(stageClearHandle);
            EventBus::Unsubscribe(stageFailHandle);
        }

        void Start() override {}

        void Update(float deltaTime) override
        {
            blinkTimer += deltaTime * 60;
            if (inputService.IsPressed(InputAction::IgConfirm))
            {
                battleService.StartStage();
                return;
            }
            if (inputService.IsPressed(InputAction::IgShowRules))
            {
                sceneService.PushScene(ESceneState::Rules);
                return;
            }

            if (!info.empty())
            {
                SetFontSize(240);
                renderService.DrawCenterString(dxe::GetWindowWidthF(.5f), dxe::GetWindowHeightF(.4f),
                                               info.c_str(), infoColor);
                SetFontSize(48);
            }
            else
            {
                SetFontSize(160);
                renderService.DrawCenterString(dxe::GetWindowWidthF(.5f), dxe::GetWindowHeightF(.2f),
                                               L"MagicCat", infoColor);
                SetFontSize(48);
                renderService.DrawCenterString(dxe::GetWindowWidthF(.5f), dxe::GetWindowHeightF(.4f),
                                               L"吾輩はマジックキャットである！\n"
                                               "これはじゃんけんの対決にゃん！\n"
                                               "吾輩は魔法のカードを使って敵の精神状態を操り、\n"
                                               "相手の出し手を左右できるにゃん。\n"
                                               "Rキーを押してルールを読んでご覧。\n"
                                               "スタート後もいつでも読めるにゃん。"
                                               , COLOR_WHITE);
            }

            renderService.DrawLeftString(20, 40,
                                         std::format(L" 勝利回数: {} ", winCount).c_str(), COLOR_WHITE);
            renderService.DrawRightString(dxe::GetWindowWidthF(1.f), 40,
                                          std::format(L" 失敗回数: {} ", failCount).c_str(), COLOR_WHITE);
            renderService.DrawCenterString(dxe::GetWindowWidthF(.5f), dxe::GetWindowHeightF(.8f),
                                           L"Enterキーを押してゲームをスタートにゃ！", COLOR_WHITE);
        }
    };

    std::unique_ptr<IScene> CreateInfoScene(IInputService& inputService, ISceneService& sceneService,
                                            IRenderService& renderService, IBattleService& battleService)
    {
        return std::make_unique<InfoScene>(inputService, sceneService, renderService, battleService);
    }
} // namespace mc
