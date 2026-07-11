module;

#include <array>
#include <memory>
#include <RenderUtils.h>

module Displayer;

import InputService;
import RenderService;
import AudioService;
import OSService;
import ButtonGroup;

namespace mc {
    class VolumePanel : public MenuPanel
    {
    public:
        VolumePanel(IInputService& input, IRenderService& render,
                    IAudioService& audio, IOSService& os)
            : MenuPanel()
              , inputService(input)
              , renderService(render)
              , audioService(audio)
              , primaryGroup(PRIMARY_RECTS, input, os, ButtonGroupLayout::Vertical)
              , secondaryGroup{
                  {
                      ButtonGroup(SECONDARY_RECTS[0], input, os, ButtonGroupLayout::Horizontal),
                      ButtonGroup(SECONDARY_RECTS[1], input, os, ButtonGroupLayout::Horizontal),
                      ButtonGroup(SECONDARY_RECTS[2], input, os, ButtonGroupLayout::Horizontal)
                  }
              }
        {
            primaryGroup.SetFocusedIndex(0);
        }

        ~VolumePanel() override
        {
            if (activeIndex != std::nullopt)
                inputService.PopContext();
        }

        bool IsPanelFocus() const override { return activeIndex != std::nullopt; }

    protected:
        void OnUpdate(float /*deltaTime*/) override
        {
            if (activeIndex == std::nullopt)
                UpdateRowSelect();
            else
                UpdateValueTweak();
        }

        void OnDraw(float /*deltaTime*/) const override
        {
            // 1. Draw outer row containers (primary group)
            for (int i = 0; i < VOLUME_ROW_COUNT; ++i)
            {
                const bool isRowFocused = (activeIndex == std::nullopt && primaryGroup.GetFocusedIndex() == i);
                const bool isRowActive = (activeIndex == i);
                
                const uint32_t fgColor = isRowActive ? COLOR_YELLOW : COLOR_WHITE;

                renderService.DrawButton(PRIMARY_RECTS[i].Expand(PRIMARY_RECT_EXPAND), L"", 
                    isRowFocused, COLOR_BG, fgColor);

                // Draw row name on the left (e.g. Master, BGM, SFX)
                renderService.DrawCenterString(
                    PRIMARY_RECTS[i].x1 + TEXT_CENTER_X,
                    PRIMARY_RECTS[i].y1 + TEXT_CENTER_Y,
                    VOLUME_LABELS[i], fgColor);
            }

            // 2. Draw inner volume level buttons (secondary group)
            for (int i = 0; i < VOLUME_ROW_COUNT; ++i)
            {
                int currentVol = 0;
                if (i == 0) currentVol = audioService.GetMasterVolume();
                else if (i == 1) currentVol = audioService.GetBgmVolume();
                else if (i == 2) currentVol = audioService.GetSfxVolume();

                for (int j = 0; j < VOLUME_LEVEL_COUNT; ++j)
                {
                    const bool isBtnFocused = (activeIndex == i && secondaryGroup[i].GetFocusedIndex() == j);
                    const bool isBtnCurrent = (currentVol == j);

                    const uint32_t fgColor = isBtnCurrent ? COLOR_YELLOW : COLOR_WHITE;

                    renderService.DrawButton(SECONDARY_RECTS[i][j], TEXTS[j], isBtnFocused, COLOR_BG, fgColor);
                }
            }
        }

    private:

        void UpdateRowSelect()
        {
            primaryGroup.Update();
            if ((activeIndex = primaryGroup.ConsumeConfirm()))
            {
                switch (*activeIndex)
                {
                case 0:
                    secondaryGroup[0].SetFocusedIndex(
                        audioService.GetMasterVolume());
                    break;
                case 1:
                    secondaryGroup[1].SetFocusedIndex(
                        audioService.GetBgmVolume());
                    break;
                case 2:
                    secondaryGroup[2].SetFocusedIndex(
                        audioService.GetSfxVolume());
                    break;
                default: ;
                }

                inputService.PushContext(InputContext::VolumeControl);
                auto click = inputService.OnMouseClick(InputAction::MouseClick);
                if (click.x != -1 && click.y != -1)
                {
                    justEnteredTweak = true;
                }
            }
        }

        void UpdateValueTweak()
        {
            secondaryGroup[*activeIndex].Update();
            if (justEnteredTweak)
            {
                secondaryGroup[*activeIndex].ConsumeConfirm();
                justEnteredTweak = false;
            }
            else
            {
                if (auto btn = secondaryGroup[*activeIndex].ConsumeConfirm())
                {
                    SetVolume(*activeIndex, *btn);
                    activeIndex = std::nullopt;
                    inputService.PopContext();
                }
            }

            if (inputService.IsPressed(InputAction::ToggleMenu))
            {
                inputService.PopContext();
                activeIndex = std::nullopt;
            }
        }

        void SetVolume(int row, int level) const
        {
            switch (row)
            {
            case 0: audioService.SetMasterVolume(level);
                break;
            case 1: audioService.SetBgmVolume(level);
                break;
            case 2: audioService.SetSfxVolume(level);
                break;
            default: break;
            }
        }


        

        IInputService& inputService;
        IRenderService& renderService;
        IAudioService& audioService;

        ButtonGroup primaryGroup;
        std::array<ButtonGroup, VOLUME_ROW_COUNT> secondaryGroup;

        std::optional<int> activeIndex = std::nullopt;
        bool justEnteredTweak = false;

    private:
        static constexpr int START_X = WINDOW_WIDTH / 10;
        static constexpr int START_Y = WINDOW_HEIGHT / 7;

        static constexpr int RECT_WIDTH = 220;
        static constexpr int RECT_HEIGHT = 100;

        static constexpr int TEXT_CENTER_X = RECT_WIDTH / 2;
        static constexpr int TEXT_CENTER_Y = RECT_HEIGHT / 2;

        static constexpr int OFFSET_X = RECT_WIDTH + 50;
        static constexpr int OFFSET_Y = WINDOW_HEIGHT / 5;


        static constexpr Rect SECONDARY_RECT = {
            START_X, START_Y, START_X + RECT_WIDTH, START_Y + RECT_HEIGHT
        };

        static constexpr Rect PRIMARY_RECT = Rect<int>{
            START_X, START_Y,
            START_X + OFFSET_X * VOLUME_LEVEL_COUNT + RECT_WIDTH, START_Y + RECT_HEIGHT
        };

        static constexpr std::array<const wchar_t*, VOLUME_ROW_COUNT> VOLUME_LABELS = {
            L"Master", L"BGM", L"SFX"
        };

        static constexpr std::array<const wchar_t*, VOLUME_LEVEL_COUNT> TEXTS = {
            L"小さすぎ", L"小さい", L"普通", L"大きい", L"大きすぎ",
        };
        
        static constexpr int PRIMARY_RECT_EXPAND = 20;

        static constexpr auto PRIMARY_RECTS = []()
        {
            std::array<Rect<int>, VOLUME_ROW_COUNT> rects{};
            for (int i = 0; i < VOLUME_ROW_COUNT; ++i)
            {
                rects[i] = PRIMARY_RECT + Point{0, i * OFFSET_Y};
            }
            return rects;
        }();

        static constexpr auto SECONDARY_RECTS = []()
        {
            std::array<std::array<Rect<int>, VOLUME_LEVEL_COUNT>, VOLUME_ROW_COUNT> rects{};
            for (int i = 0; i < VOLUME_ROW_COUNT; ++i)
            {
                for (int j = 0; j < VOLUME_LEVEL_COUNT; ++j)
                {
                    rects[i][j] = SECONDARY_RECT + Point{(j + 1) * OFFSET_X, i * OFFSET_Y};
                }
            }
            return rects;
        }();
    };

    std::unique_ptr<MenuPanel> CreateVolumePanel(IInputService& input,
                                                 IRenderService& render,
                                                 IAudioService& audio,
                                                 IOSService& os)
    {
        return std::make_unique<VolumePanel>(input, render, audio, os);
    }
} // namespace mc
