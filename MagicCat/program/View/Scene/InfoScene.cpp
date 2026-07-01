module;

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

    public:
        InfoScene(IInputService& input, ISceneService& scene, IRenderService& render, IBattleService& battle)
            : inputService(input), sceneService(scene), renderService(render), battleService(battle)
        {
            infoColor = 0xF259FF;
            // これらのサブスクリプションはコンストラクタに配置し、デストラクタで Unsubscribe する。
            // InfoScene はセッション開始時に一度だけ構築され、複数のステージにまたがって生存し続けるため、
            // winCount と failCount をステージ間で正確に累積できる。
            // Start() に移動すると、Start() はシーンがアクティブになるたびに呼ばれるため、
            // 重複サブスクリプションが発生し、カウントが二重以上に加算されてしまう。
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
            if (inputService.IsPressed(InputAction::IgConfirm))
            {
                battleService.StartStage();
            }
            else if (inputService.IsPressed(InputAction::IgShowRules))
            {
                sceneService.PushScene(ESceneState::Rules);
                return;
            }

            const int w = renderService.GetWindowWidth();
            const int h = renderService.GetWindowHeight();

            auto click = inputService.OnMouseClick(InputAction::IgMouseClick);
            if (click.x != -1 && click.y != -1)
            {
                if (click.x >= 0 && click.x < w &&
                    click.y >= 0 && click.y < h)
                {
                    battleService.StartStage();
                    return;
                }
            }

            if (!info.empty())
            {
                renderService.SetFontSize(240);
                renderService.DrawCenterString(renderService.GetWindowWidth() / 2, renderService.GetWindowHeight() * 4 / 10,
                                               info.c_str(), infoColor);
                renderService.SetFontSize(48);
            }
            else
            {
                renderService.SetFontSize(160);
                renderService.DrawCenterString(renderService.GetWindowWidth() / 2, renderService.GetWindowHeight() * 2 / 10,
                                               L"MagicCat", infoColor);
                renderService.SetFontSize(48);
                renderService.DrawCenterString(renderService.GetWindowWidth() / 2, renderService.GetWindowHeight() * 4 / 10,
                                               L"吾輩はマジックキャットである！\n"
                                               "これはじゃんけんの対決にゃん！\n"
                                               "吾輩は魔法のカードを使って敵の精神状態を操り、\n"
                                               "相手の出し手を左右できるにゃん。\n"
                                               "ESCキーを押してルールを読んでご覧。\n"
                                               "スタート後もいつでも読めるにゃん。"
                                               , COLOR_WHITE);
            }

            renderService.DrawLeftString(20, 40,
                                         std::format(L" 勝利回数: {} ", winCount).c_str(), COLOR_WHITE);
            renderService.DrawRightString(renderService.GetWindowWidth(), 40,
                                          std::format(L" 失敗回数: {} ", failCount).c_str(), COLOR_WHITE);
            renderService.DrawCenterString(renderService.GetWindowWidth() / 2, renderService.GetWindowHeight() * 8 / 10,
                                           L"Enterキーを押してゲームをスタートにゃ！", COLOR_WHITE);
        }
    };

    std::unique_ptr<IScene> CreateInfoScene(IInputService& inputService, ISceneService& sceneService,
                                            IRenderService& renderService, IBattleService& battleService)
    {
        return std::make_unique<InfoScene>(inputService, sceneService, renderService, battleService);
    }
} // namespace mc
