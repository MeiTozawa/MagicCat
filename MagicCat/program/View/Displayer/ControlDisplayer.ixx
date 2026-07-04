module;

#include <memory>
#include <array>
#include <RenderUtils.h>

export module Displayer:Control;
import DisplayerBase;

import AssetService;
import RenderService;
import InputService;
import EventBus;


namespace mc {
    export struct DrawPileHoveredEvent : IEvent {};
    
    export struct DrawPileUnhoveredEvent : IEvent {};

    namespace {
        constexpr int ICON_DRAW_X  = 100;   
        constexpr int ICON_NAV_X   = 500;   
        constexpr int ICON_CONF_X  = 620;   
        constexpr int ICON_RULES_X = 920;   
        constexpr int Y = 100;

        constexpr int TEXT_OFFSET_X = 40;
        constexpr int TEXT_OFFSET_Y = -25;

        constexpr int ICON_HALF_W = 40;
        constexpr int ICON_HALF_H = 28;

        constexpr int MENU_ICON_Y = 60;

        constexpr int HINT_X = 20;
        constexpr int HINT_Y = 20;

        constexpr int DRAW_CARD_X1 = 50;
        constexpr int DRAW_CARD_Y1 = 400;
        constexpr int DRAW_CARD_X2 = 250;
        constexpr int DRAW_CARD_Y2 = 700;

        struct ClickableArea {
            int x1, y1, x2, y2;
            const wchar_t* hintText;
        };
    }

    class ControlDisplayer : public Displayer
    {
        IAssetService& assetService;
        IRenderService& renderService;
        IInputService& inputService;
        uint32_t color;

        mutable bool drawPileHovered = false; 

    public:
        ControlDisplayer(IAssetService& asset, IRenderService& render,
                         IInputService& input, uint32_t c = 0xFFFFFF)
            : assetService(asset), renderService(render), inputService(input), color(c) {}

    private:
        void OnDraw(float) const override
        {
            if ((inputService.GetActiveDevice() == InputDevice::Gamepad))
            {
                if (drawPileHovered)
                {
                    drawPileHovered = false;
                    EventBus::Publish(DrawPileUnhoveredEvent{});
                }
                renderService.DrawRotaGraphF(ICON_NAV_X,   Y, 0.5, 0.0,
                    assetService.GetImageHandle(EImage::XBOX_DPAD_HORIZONTAL), true);
          
                renderService.DrawRotaGraphF(ICON_CONF_X,  Y, 0.5, 0.0,
                    assetService.GetImageHandle(EImage::XBOX_A), true);
       
                renderService.DrawRotaGraphF(ICON_DRAW_X,  Y, 0.5, 0.0,
                    assetService.GetImageHandle(EImage::XBOX_X), true);
     
                renderService.DrawRotaGraphF(ICON_RULES_X, Y, 0.5, 0.0,
                    assetService.GetImageHandle(EImage::BUTTON_MENU), true);
                return;
            }

            auto mousePos = inputService.GetMousePosition();
            int mx = mousePos.x;
            int my = mousePos.y;
            int menuIconX = renderService.GetWindowWidth() - 60;

            const std::array<ClickableArea, 2> areas = {{
                {
                    menuIconX - ICON_HALF_W, MENU_ICON_Y - ICON_HALF_H,
                    menuIconX + ICON_HALF_W, MENU_ICON_Y + ICON_HALF_H,
                    L"クリックしてメニューを開く"
                },
                {
                    DRAW_CARD_X1, DRAW_CARD_Y1,
                    DRAW_CARD_X2, DRAW_CARD_Y2,
                    L"クリックしてカードを引く"
                }
            }};

            const wchar_t* selectedHint = nullptr;
            int hitIndex = -1;
            for (int i = 0; i < static_cast<int>(areas.size()); ++i)
            {
                const auto& a = areas[i];
                if (mx >= a.x1 && mx < a.x2 && my >= a.y1 && my < a.y2)
                {
                    selectedHint = a.hintText;
                    hitIndex = i;
                    break;
                }
            }

            {
                const bool currentlyHovered = (hitIndex == 1); // DrawCardHitBox は index 1
                if (currentlyHovered && !drawPileHovered)
                {
                    drawPileHovered = true;
                    EventBus::Publish(DrawPileHoveredEvent{});
                }
                else if (!currentlyHovered && drawPileHovered)
                {
                    drawPileHovered = false;
                    EventBus::Publish(DrawPileUnhoveredEvent{});
                }
            }

            auto icon = assetService.GetImageHandle(EImage::BUTTON_MENU);
            renderService.DrawRotaGraphF(menuIconX, MENU_ICON_Y, 0.5, 0.0, icon, true);

            if (selectedHint != nullptr && selectedHint[0] != L'\0')
            {
                renderService.DrawString(HINT_X, HINT_Y, selectedHint, color);

                if (hitIndex == 0)
                {
                    renderService.DrawHollowBox(
                        menuIconX - ICON_HALF_W, MENU_ICON_Y - ICON_HALF_H,
                        menuIconX + ICON_HALF_W, MENU_ICON_Y + ICON_HALF_H,
                        2, color);
                }
            }
        }
    };

    export std::unique_ptr<Displayer> CreateControlDisplayer(IAssetService& assetService,
                                                             IRenderService& renderService,
                                                             IInputService& inputService,
                                                             uint32_t color = 0xFFFFFF)
    {
        return std::make_unique<ControlDisplayer>(assetService, renderService, inputService, color);
    }
} // namespace mc
