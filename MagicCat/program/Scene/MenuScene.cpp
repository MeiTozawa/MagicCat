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
    enum class MenuNavMode
    {
        OptionFocus,
        PanelFocus
    };

    class MenuScene : public IScene
    {
    private:
        static constexpr int BOX_MARGIN_X = 100, BOX_MARGIN_Y_UP = 75, BOX_MARGIN_Y_DOWN = 250;
        static constexpr int TEXT_START_OFFSET_X = 50, TEXT_START_OFFSET_Y = 50;
        static constexpr int CONTENT_START_OFFSET_Y = 140;
        static constexpr uint32_t COLOR_BOX_BG = 0x1E1E28;
        static constexpr int BUTTON_HEIGHT = 100, BUTTON_WIDTH = 300, BUTTON_OFFSET_X = 50, BUTTON_COUNT = 5;
        static constexpr int BUTTON_Y1 = 880, BUTTON_Y2 = BUTTON_Y1 + BUTTON_HEIGHT;
        static constexpr int BUTTON0_X = (WINDOW_WIDTH - BUTTON_WIDTH * BUTTON_COUNT - BUTTON_OFFSET_X * (BUTTON_COUNT - 1)) /
            2;

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
              buttons(BTN_RECTS, input, os, ButtonGroupLayout::Horizontal)
        {
        }

        void Start() override
        {
            inputService.PushContext(InputContext::Menu);
            navMode = MenuNavMode::OptionFocus;
            activePanel = 0;
            currentPanel = CreatePanel(0);
        }

        void Update(float deltaTime) override
        {
            if (inputService.IsPressed(InputAction::ToggleMenu))
            {
                currentPanel.reset();
                inputService.PopContext();
                sceneService.PopScene();
                return;
            }

            if (currentPanel && currentPanel->IsPanelFocus())
            {
                navMode = MenuNavMode::PanelFocus;
            }
            else
            {
                navMode = MenuNavMode::OptionFocus;
            }

            if (navMode == MenuNavMode::OptionFocus && activePanel == 0)
            {
                buttons.Update();
            }

            if (currentPanel)
            {
                if (!currentPanel->UpdatePanel(deltaTime))
                {
                    activePanel = 0;
                    currentPanel = CreatePanel(0);
                    navMode = MenuNavMode::OptionFocus;
                }
                if (currentPanel)
                {
                    currentPanel->Draw(deltaTime);
                }
            }

            if (navMode == MenuNavMode::OptionFocus && activePanel == 0)
            {
                if (auto idx = buttons.ConsumeConfirm())
                {
                    if (*idx != 0)
                    {
                        activePanel = *idx;
                        currentPanel = CreatePanel(activePanel);
                    }
                    // idx == 0: RulesPanel handles paging internally
                }
            }

            DrawBox();
            DrawButtons();
        }

    private:
        std::unique_ptr<MenuPanel> CreatePanel(int index)
        {
            constexpr int boxX1 = BOX_MARGIN_X;
            constexpr int textY = BOX_MARGIN_Y_UP + CONTENT_START_OFFSET_Y;
            switch (index)
            {
            case 0: return CreateRulesPanel(boxX1, textY, inputService, renderService, assetService);
            case 1: return CreateVolumePanel(boxX1, textY, inputService, renderService, audioService, osService);
            case 2: return CreateSavePanel(boxX1, textY, inputService, renderService, battleService, configService, sceneService, osService);
            case 3: return CreateLoadPanel(boxX1, textY, inputService, renderService, battleService, configService, sceneService, osService);
            case 4: return CreateExitPanel(boxX1, textY, inputService, renderService);
            default: return nullptr;
            }
        }

        void DrawBox() const
        {
            constexpr int x1 = BOX_MARGIN_X, y1 = BOX_MARGIN_Y_UP;
            constexpr int x2 = WINDOW_WIDTH - BOX_MARGIN_X, y2 = WINDOW_HEIGHT - BOX_MARGIN_Y_DOWN;
            renderService.DrawBoxAA(static_cast<float>(x1), static_cast<float>(y1),
                                    static_cast<float>(x2), static_cast<float>(y2), COLOR_BOX_BG, true);
            renderService.DrawHollowBox(x1, y1, x2, y2, 3, COLOR_WHITE);
            renderService.DrawString(x1 + TEXT_START_OFFSET_X, y1 + TEXT_START_OFFSET_Y, L"【メニュー】", COLOR_WHITE);
        }

        void DrawButtons() const
        {
            for (int i = 0; i < BUTTON_COUNT; ++i)
                renderService.DrawButton(BTN_RECTS[i], BTN_LABELS[i], buttons.GetFocusedIndex() == i);
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
        int activePanel = 0;
        MenuNavMode navMode = MenuNavMode::OptionFocus;
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
