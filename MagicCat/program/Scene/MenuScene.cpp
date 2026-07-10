module;

#include <array>
#include <app_build_setting.h>
#include <memory>
#include <RenderUtils.h>

module SceneService;

import Displayer;
import InputService;
import RenderService;
import AssetService;
import OSService;
import ButtonGroup;

namespace mc {
    class MenuScene : public IScene
    {
    private:
        static constexpr int BUTTON_HEIGHT = 100, BUTTON_WIDTH = 300, BUTTON_OFFSET_X = 50, BUTTON_COUNT = 5;
        static constexpr int BUTTON_Y1 = 880, BUTTON_Y2 = BUTTON_Y1 + BUTTON_HEIGHT;
        static constexpr int BUTTON0_X = (WINDOW_WIDTH - BUTTON_WIDTH * BUTTON_COUNT - BUTTON_OFFSET_X
            * (BUTTON_COUNT - 1)) / 2;

        static constexpr auto BTN_RECTS = []()
        {
            std::array<Rect<int>, BUTTON_COUNT> r{};
            for (int i = 0; i < BUTTON_COUNT; ++i)
            {
                const int x = BUTTON0_X + i * (BUTTON_WIDTH + BUTTON_OFFSET_X);
                r[i] = {x, BUTTON_Y1, x + BUTTON_WIDTH, BUTTON_Y2};
            }
            return r;
        }();

        static constexpr const wchar_t* BTN_LABELS[BUTTON_COUNT] = {
            L"ルール", L"音量設定", L"セーブ", L"ロード", L"終了"
        };

    public:
        MenuScene(IInputService& input, ISceneService& scene, IAssetService& asset,
                  IRenderService& render, IAudioService& audio, IConfigService& config,
                  IBattleService& battle, IOSService& os)
            : inputService(input), sceneService(scene), assetService(asset),
              renderService(render), audioService(audio), configService(config),
              battleService(battle), osService(os),
              buttons(BTN_RECTS, input, os, ButtonGroupLayout::Horizontal) {}

        void Start() override
        {
            inputService.PushContext(InputContext::Menu);
            buttons.SetFocusedIndex(0);
            currentPanel = CreatePanel(EMenuPanel::Rules);
        }

        void Update(float deltaTime) override
        {
            DrawBox();
            DrawButtons();

            if (inputService.IsPressed(InputAction::ToggleMenu))
            {
                currentPanel.reset();
                inputService.PopContext();
                sceneService.PopScene();
                return;
            }

            if (!currentPanel->IsPanelFocus())
            {
                buttons.Update();
                int idx = buttons.GetFocusedIndex();
                EMenuPanel newType = static_cast<EMenuPanel>(idx);
                if (newType != currentPanelType)
                {
                    currentPanelType = newType;
                    currentPanel = CreatePanel(newType);
                }
            }


            currentPanel->Update(deltaTime);
            currentPanel->Draw(deltaTime);
        }

    private:
        std::unique_ptr<MenuPanel> CreatePanel(EMenuPanel panel) const
        {
            switch (panel)
            {
            case EMenuPanel::Rules: return CreateRulesPanel(
                    inputService, renderService, assetService);
            case EMenuPanel::Volume: return CreateVolumePanel(
                    inputService, renderService, audioService, osService);
            case EMenuPanel::Save: return CreateSavePanel(
                    inputService, renderService, battleService, configService, sceneService, osService);
            case EMenuPanel::Load: return CreateLoadPanel(
                    inputService, renderService, battleService, configService, sceneService, osService);
            case EMenuPanel::Exit: return CreateExitPanel(
                    inputService, renderService, assetService);
            default: return nullptr;
            }
        }

        void DrawBox() const
        {
            constexpr int x1 = MENU_BOX_MARGIN_X, y1 = MENU_BOX_MARGIN_Y_UP;
            constexpr int x2 = WINDOW_WIDTH - MENU_BOX_MARGIN_X, y2 = WINDOW_HEIGHT - MENU_BOX_MARGIN_Y_DOWN;
            renderService.DrawBoxAA(static_cast<float>(x1), static_cast<float>(y1),
                                    static_cast<float>(x2), static_cast<float>(y2), MENU_COLOR_BOX_BG, true);
            renderService.DrawHollowBox(x1, y1, x2, y2, 3, COLOR_WHITE);
        }

        void DrawButtons() const
        {
            for (int i = 0; i < BUTTON_COUNT; ++i)
                renderService.DrawOption(BTN_RECTS[i], BTN_LABELS[i], buttons.GetFocusedIndex() == i);
        }

        IInputService& inputService;
        ISceneService& sceneService;
        IAssetService& assetService;
        IRenderService& renderService;
        IAudioService& audioService;
        IConfigService& configService;
        IBattleService& battleService;
        IOSService& osService;
        ButtonGroup buttons;
        std::unique_ptr<MenuPanel> currentPanel;
        EMenuPanel currentPanelType = EMenuPanel::Rules;
    };

    std::unique_ptr<IScene> CreateMenuScene(IInputService& inputService, ISceneService& sceneService,
                                            IAssetService& assetService, IRenderService& renderService,
                                            IAudioService& audioService, IConfigService& configService,
                                            IBattleService& battleService, IOSService& osService)
    {
        return std::make_unique<MenuScene>(inputService, sceneService, assetService, renderService,
                                           audioService, configService, battleService, osService);
    }
} // namespace mc
