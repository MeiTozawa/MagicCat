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
    private:
        static constexpr int BOX_MARGIN_X = 100;
        static constexpr int LINE_SPACING = 60;
        static constexpr int INDENT_LEVEL_1 = 50;
        static constexpr int INDENT_LEVEL_2 = 80;

        static constexpr int VOLUME_ROW_COUNT = 3;
        static constexpr int LEVEL_MAX = 4;
        static constexpr int SQUARE_SIZE = 30;
        static constexpr int SQUARE_GAP = 8;
        static constexpr int INDICATOR_OFFSET = 200;

        static constexpr int ARROW_BTN_WIDTH = 40;
        static constexpr int ARROW_BTN_GAP = 8;

        static constexpr int ARROW_START_OFFSET =
            INDENT_LEVEL_2 + INDICATOR_OFFSET + LEVEL_MAX * (SQUARE_SIZE + SQUARE_GAP) + ARROW_BTN_GAP;

        static constexpr const wchar_t* VOLUME_LABELS[VOLUME_ROW_COUNT] = {
            L"Master", L"BGM", L"SFX"
        };

    public:
        VolumePanel(int boxX1, int textY,
                    IInputService& input, IRenderService& render,
                    IAudioService& audio, IOSService& os)
            : MenuPanel(boxX1, textY)
              , inputService(input)
              , renderService(render)
              , audioService(audio)
              , primaryGroup(BuildPrimaryRects(textY), input, os, ButtonGroupLayout::Vertical)
              , secondaryGroup{
                  {
                      ButtonGroup(BuildSecondaryRects(textY, 0), input, os, ButtonGroupLayout::Horizontal),
                      ButtonGroup(BuildSecondaryRects(textY, 1), input, os, ButtonGroupLayout::Horizontal),
                      ButtonGroup(BuildSecondaryRects(textY, 2), input, os, ButtonGroupLayout::Horizontal)
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

        bool UpdatePanel(float deltaTime) override
        {
            const bool wasRowSelect = (activeIndex == std::nullopt);
            Update(deltaTime);
            if (wasRowSelect && inputService.IsPressed(InputAction::Left))
            {
                return false;
            }
            return true;
        }

        bool IsPanelFocus() const override { return activeIndex >= 0; }

    private:
        void OnUpdate(float /*deltaTime*/) override
        {
            if (activeIndex == std::nullopt)
                UpdateRowSelect();
            else
                UpdateValueTweak();
        }

        void UpdateRowSelect()
        {
            primaryGroup.Update();
            if (auto confirmed = primaryGroup.ConsumeConfirm())
            {
                activeIndex = *confirmed;
                secondaryGroup[*activeIndex].SetFocusedIndex(2); //TODO: set focus on the selected option
                inputService.PushContext(InputContext::VolumeControl);
            }
        }

        void UpdateValueTweak()
        {
            secondaryGroup[*activeIndex].Update();
            if (auto btn = secondaryGroup[*activeIndex].ConsumeConfirm())
            {
                AdjustVolume(*activeIndex, *btn == 0 ? -1 : +1);
            }

            if (inputService.IsPressed(InputAction::Left) ||
                inputService.IsPressed(InputAction::ToggleMenu))
            {
                inputService.PopContext();
                activeIndex = std::nullopt;
            }
        }

        void AdjustVolume(int row, int delta) const
        {
            switch (row)
            {
            case 0: audioService.SetMasterVolume(audioService.GetMasterVolume() + delta);
                break;
            case 1: audioService.SetBgmVolume(audioService.GetBgmVolume() + delta);
                break;
            case 2: audioService.SetSfxVolume(audioService.GetSfxVolume() + delta);
                break;
            default: break;
            }
        }

        void OnDraw(float /*deltaTime*/) const override
        {
            const int volumes[VOLUME_ROW_COUNT] = {
                audioService.GetMasterVolume(),
                audioService.GetBgmVolume(),
                audioService.GetSfxVolume()
            };

            renderService.DrawString(x + INDENT_LEVEL_1, y, L"音量設定", COLOR_TEXT_NORMAL);

            for (int i = 0; i < VOLUME_ROW_COUNT; ++i)
            {
                const int rowY = y + LINE_SPACING * (i + 1);
                const bool focused = (primaryGroup.GetFocusedIndex() == i);
                const uint32_t col = focused ? COLOR_TEXT_GREEN : COLOR_TEXT_NORMAL;

                renderService.DrawString(x + INDENT_LEVEL_2, rowY, VOLUME_LABELS[i], col);

                const int startX = x + INDENT_LEVEL_2 + INDICATOR_OFFSET;
                for (int lv = 0; lv <= LEVEL_MAX; ++lv)
                {
                    const int sx = startX + lv * (SQUARE_SIZE + SQUARE_GAP);
                    if (lv < volumes[i])
                        renderService.DrawBoxAA(
                            static_cast<float>(sx), static_cast<float>(rowY),
                            static_cast<float>(sx + SQUARE_SIZE), static_cast<float>(rowY + SQUARE_SIZE),
                            col, true);
                    else
                        renderService.DrawHollowBox(sx, rowY,
                                                    sx + SQUARE_SIZE, rowY + SQUARE_SIZE, 2, col);
                }

                if (activeIndex == i)
                {
                    const int arrowX = x + ARROW_START_OFFSET;
                    const bool leftFocus = (secondaryGroup[i].GetFocusedIndex() == 0);
                    const bool rightFocus = (secondaryGroup[i].GetFocusedIndex() == 1);

                    renderService.DrawString(arrowX, rowY,
                                             L"◀", leftFocus ? COLOR_TEXT_GREEN : COLOR_TEXT_NORMAL);
                    renderService.DrawString(arrowX + ARROW_BTN_WIDTH + ARROW_BTN_GAP, rowY,
                                             L"▶", rightFocus ? COLOR_TEXT_GREEN : COLOR_TEXT_NORMAL);
                }
            }
        }

        static std::array<Rect<int>, VOLUME_ROW_COUNT> BuildPrimaryRects(int textY)
        {
            std::array<Rect<int>, VOLUME_ROW_COUNT> rects{};
            for (int i = 0; i < VOLUME_ROW_COUNT; ++i)
            {
                const int y1 = textY + LINE_SPACING * (i + 1);
                rects[i] = {
                    BOX_MARGIN_X + INDENT_LEVEL_2, y1,
                    WINDOW_WIDTH - BOX_MARGIN_X, y1 + LINE_SPACING
                };
            }
            return rects;
        }

        static std::array<Rect<int>, 2> BuildSecondaryRects(int textY, int row)
        {
            const int rowY = textY + LINE_SPACING * (row + 1);
            constexpr int startX = BOX_MARGIN_X + ARROW_START_OFFSET;
            return {
                {
                    {
                        startX, rowY,
                        startX + ARROW_BTN_WIDTH, rowY + LINE_SPACING
                    },
                    {
                        startX + ARROW_BTN_WIDTH + ARROW_BTN_GAP, rowY,
                        startX + ARROW_BTN_WIDTH * 2 + ARROW_BTN_GAP, rowY + LINE_SPACING
                    }
                }
            };
        }

        IInputService& inputService;
        IRenderService& renderService;
        IAudioService& audioService;

        ButtonGroup primaryGroup;
        std::array<ButtonGroup, VOLUME_ROW_COUNT> secondaryGroup;

        std::optional<int> activeIndex = std::nullopt;
    };

    std::unique_ptr<MenuPanel> CreateVolumePanel(int boxX1, int textY,
                                                 IInputService& input,
                                                 IRenderService& render,
                                                 IAudioService& audio,
                                                 IOSService& os)
    {
        return std::make_unique<VolumePanel>(boxX1, textY, input, render, audio, os);
    }
} // namespace mc
