module;

#include <app_build_setting.h>
#include <memory>
#include <RenderUtils.h>

module SceneService;
import BattleService;
import InputService;
import RenderService;
import EventBus;

namespace mc {

namespace {
}

    class InfoScene : public IScene
    {
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
            HandleInput();
            DrawScene();
        }

    private:
        void HandleInput() const
        {
            if (inputService.IsPressed(InputAction::Confirm))
            {
                battleService.StartStage();
                return;
            }
            if (inputService.IsPressed(InputAction::ToggleMenu))
            {
                sceneService.PushScene(ESceneState::Menu);
                return;
            }
            auto click = inputService.OnMouseClick(InputAction::MouseClick);
            if (click.x != -1 && click.y != -1)
            {
                if (click.x >= 0 && click.x < WINDOW_WIDTH && click.y >= 0 && click.y < WINDOW_HEIGHT)
                    battleService.StartStage();
            }
        }

        void DrawScene() const
        {
            if (!info.empty())
            {
                renderService.SetFontSize(240);
                renderService.DrawCenterString(WINDOW_WIDTH / 2,
                                               WINDOW_HEIGHT * 4 / 10,
                                               info.c_str(), infoColor);
                renderService.SetFontSize(48);
            }
            else
            {
                DrawTitleScreen();
            }

            renderService.DrawLeftString(10, WINDOW_HEIGHT - 42,
                                         std::format(L" 勝利回数: {} ", winCount).c_str(), COLOR_WHITE);
            renderService.DrawRightString(WINDOW_WIDTH - 10, WINDOW_HEIGHT - 42,
                                          std::format(L" 失敗回数: {} ", failCount).c_str(), COLOR_WHITE);
            renderService.DrawCenterString(WINDOW_WIDTH / 2,
                                           WINDOW_HEIGHT * 8 / 10,
                                           L"SPACEキーを押してゲームをスタートにゃ！", COLOR_WHITE);
        }
        void DrawTitleScreen() const
        {
            renderService.SetFontSize(160);
            renderService.DrawCenterString(WINDOW_WIDTH / 2,
                                           WINDOW_HEIGHT * 2 / 10,
                                           L"MagicCat", infoColor);
            renderService.SetFontSize(48);
            renderService.DrawCenterString(WINDOW_WIDTH / 2,
                                           WINDOW_HEIGHT * 4 / 10,
                                           L"吾輩はマジックキャットである！\n"
                                           "これはじゃんけんの対決にゃん！\n"
                                           "吾輩は魔法のカードを使って敵の精神状態を操り、\n"
                                           "相手の出し手を左右できるにゃん。\n"
                                           "\nメニューにはルールの説明があるにゃん\n"
                                           "スタート後もいつでも読めるにゃん。"
                                           , COLOR_WHITE);
        }

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
    };

    std::unique_ptr<IScene> CreateInfoScene(IInputService& inputService, ISceneService& sceneService,
                                            IRenderService& renderService, IBattleService& battleService)
    {
        return std::make_unique<InfoScene>(inputService, sceneService, renderService, battleService);
    }
} // namespace mc
