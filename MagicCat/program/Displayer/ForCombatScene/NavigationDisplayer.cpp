module;

#include <memory>
#include <array>
#include <RenderUtils.h>

module Displayer;

import AssetService;
import RenderService;
import InputService;
import OSService;
import EventBus;


namespace mc {
    struct ClickableArea
    {
        int x1, y1, x2, y2;
        const wchar_t* hintText;
    };

    class NavigationDisplayer : public DelegatingDisplayer
    {
    public:
        NavigationDisplayer(IAssetService& asset, IRenderService& render,
                         IInputService& input, IOSService& os, uint32_t c = 0xFFFF00)
            : assetService(asset), renderService(render), inputService(input),
              osService(os), color(c) {}

    protected:
        void OnDraw(float) const override
        {
            const InputDevice activeDevice = inputService.GetActiveDevice();

            if (activeDevice == InputDevice::Gamepad)
                DrawGamepadHints();
            else if (activeDevice == InputDevice::Keyboard)
                DrawKeyboardHints();
            else if (activeDevice == InputDevice::Mouse)
                DrawMouseHints();
        }

    private:
        void DrawGamepadHints() const
        {
            osService.SetCursorArrow();
            renderService.DrawRotaGraphF(ICON_DRAW_X, Y, 0.5, 0.0,
                             assetService.GetImageHandle(EImage::XBOX_X), true);
            renderService.DrawString(ICON_DRAW_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"カードを引く", color);

            renderService.DrawRotaGraphF(ICON_NAV2_X, Y, 0.5, 0.0,
                                         assetService.GetImageHandle(EImage::XBOX_DPAD_HORIZONTAL), true);

            renderService.DrawRotaGraphF(ICON_CONF_X, Y, 0.5, 0.0,
                                         assetService.GetImageHandle(EImage::XBOX_A), true);
            renderService.DrawString(ICON_CONF_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"選択する", color);
        }

        void DrawKeyboardHints() const
        {
            osService.SetCursorArrow();

            renderService.DrawRotaGraphF(ICON_DRAW_X, Y, 0.5, 0.0,
                                         assetService.GetImageHandle(EImage::KB_Q), true);
            renderService.DrawString(ICON_DRAW_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"カードを引く", color);

            renderService.DrawRotaGraphF(ICON_RULES_X, Y, 0.5, 0.0,
                                         assetService.GetImageHandle(EImage::KB_ESCAPE), true);
            renderService.DrawString(ICON_RULES_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"メニューを開く", color);

            renderService.DrawRotaGraphF(ICON_NAV_X, Y, 0.5, 0.0,
                                         assetService.GetImageHandle(EImage::KB_UP), true);
            renderService.DrawRotaGraphF(ICON_NAV2_X, Y, 0.5, 0.0,
                                         assetService.GetImageHandle(EImage::KB_DOWN), true);

            renderService.DrawRotaGraphF(ICON_CONF_X, Y, 0.5, 0.0,
                                         assetService.GetImageHandle(EImage::KB_SPACE), true);
            renderService.DrawString(ICON_CONF_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"選択する", color);
        }

        void DrawMouseHints() const
        {
            const auto mousePos = inputService.GetMousePosition();
            const int mx = mousePos.x;
            const int my = mousePos.y;

            const std::array<ClickableArea, 6> areas = BuildMouseClickableAreas(MENU_ICON_X);

            const wchar_t* selectedHint = nullptr;
            bool hitAny = false;
            for (const auto& a : areas)
            {
                if (mx >= a.x1 && mx < a.x2 && my >= a.y1 && my < a.y2)
                {
                    selectedHint = a.hintText;
                    hitAny = true;
                    break;
                }
            }

            if (hitAny)
                osService.SetCursorPointer();
            else
                osService.SetCursorArrow();

            if (selectedHint != nullptr && selectedHint[0] != L'\0')
            {
                int icon = assetService.GetImageHandle(EImage::MOUSE_LEFT);
                renderService.DrawRotaGraphF(HINT_X, HINT_Y, 0.5, 0.0, icon, true);
                renderService.DrawString(HINT_TEXT_X, HINT_TEXT_Y, selectedHint, color);
            }
        }

        std::array<ClickableArea, 6> BuildMouseClickableAreas(int MENU_ICON_X) const
        {
            return {
                {
                    {
                        MENU_ICON_X - MENU_ICON_HALF_W, MENU_ICON_Y - MENU_ICON_HALF_H,
                        MENU_ICON_X + MENU_ICON_HALF_W, MENU_ICON_Y + MENU_ICON_HALF_H,
                        L"メニューを開く"
                    },
                    {
                        DRAW_PILE_X1, DRAW_PILE_Y1,
                        DRAW_PILE_X2, DRAW_PILE_Y2,
                        L"カードを引く"
                    },
                    {
                        ACTION_MENU_X, ACTION_MENU_Y + 0 * ACTION_MENU_STEP_Y,
                        ACTION_MENU_X + ACTION_MENU_W, ACTION_MENU_Y + 0 * ACTION_MENU_STEP_Y + ACTION_MENU_H,
                        L"魔法を使う"
                    },
                    {
                        ACTION_MENU_X, ACTION_MENU_Y + 1 * ACTION_MENU_STEP_Y,
                        ACTION_MENU_X + ACTION_MENU_W, ACTION_MENU_Y + 1 * ACTION_MENU_STEP_Y + ACTION_MENU_H,
                        L"グーを出す"
                    },
                    {
                        ACTION_MENU_X, ACTION_MENU_Y + 2 * ACTION_MENU_STEP_Y,
                        ACTION_MENU_X + ACTION_MENU_W, ACTION_MENU_Y + 2 * ACTION_MENU_STEP_Y + ACTION_MENU_H,
                        L"チョキを出す"
                    },
                    {
                        ACTION_MENU_X, ACTION_MENU_Y + 3 * ACTION_MENU_STEP_Y,
                        ACTION_MENU_X + ACTION_MENU_W, ACTION_MENU_Y + 3 * ACTION_MENU_STEP_Y + ACTION_MENU_H,
                        L"パーを出す"
                    },
                }
            };
        }

        IAssetService& assetService;
        IRenderService& renderService;
        IInputService& inputService;
        IOSService& osService;
        uint32_t color;

    private:
        static constexpr int ICON_DRAW_X = 100;
        static constexpr int ICON_NAV_X = 500;
        static constexpr int ICON_NAV2_X = 560;
        static constexpr int ICON_CONF_X = 620;
        static constexpr int ICON_RULES_X = 920;
        static constexpr int Y = 100;

        static constexpr int TEXT_OFFSET_X = 40;
        static constexpr int TEXT_OFFSET_Y = -25;

        static constexpr int HINT_X = 20 + ICON_SIZE_X / 4;
        static constexpr int HINT_Y = 20 + ICON_SIZE_Y / 4;
        static constexpr int HINT_TEXT_X = 20 + ICON_SIZE_X / 2 + 10;
        static constexpr int HINT_TEXT_Y = HINT_Y - FONT_SIZE / 2;
    };

    std::unique_ptr<IDisplayer> CreateControlDisplayer(
        IAssetService& assetService, IRenderService& renderService,
        IInputService& inputService, IOSService& osService, uint32_t color)
    {
        return std::make_unique<NavigationDisplayer>(assetService, renderService, inputService, osService, color);
    }
} // namespace mc
