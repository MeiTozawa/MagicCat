module;

#include <memory>
#include <RenderUtils.h>

module Displayer;

import InputService;
import RenderService;

namespace mc {

class ExitPanel : public MenuPanel {
public:
    ExitPanel(IInputService& input, IRenderService& render, IAssetService& asset)
        : MenuPanel()
        , inputService(input)
        , renderService(render)
        , assetService(asset)
    {}

    protected:
        void OnUpdate(float /*deltaTime*/) override
        {
            if (inputService.IsPressed(InputAction::Confirm))
            {
                renderService.ExitApplication();
                return;
            }
            auto click = inputService.OnMouseClick(InputAction::MouseClick);
            if (click.In(MENU_BOX_RECT))
                renderService.ExitApplication();
        }

        void OnDraw(float /*deltaTime*/) const override
        {
            renderService.DrawCenterString(WINDOW_WIDTH / 2, GetY(),
                                     L"ゲームを終了しますか？", COLOR_TEXT_NORMAL);
            renderService.DrawCenterString(WINDOW_WIDTH / 2, GetY() + 100,
                                     L"保存されていない内容は失われます！", COLOR_TEXT_RED);
            
            DrawNavigationHints();
        }

    private:
        void DrawNavigationHints() const
    {
        int icon;
        switch (inputService.GetActiveDevice())
        {
        case InputDevice::Keyboard: icon = assetService.GetImageHandle(EImage::KB_SPACE);
            break;
        case InputDevice::Mouse: icon = assetService.GetImageHandle(EImage::MOUSE_LEFT);
            break;
        case InputDevice::Gamepad: icon = assetService.GetImageHandle(EImage::XBOX_A);
            break;
        default: assert(false && "未知のデバイス");
            icon = assetService.GetImageHandle(EImage::MOUSE_LEFT);
        }
        renderService.DrawRotaGraphF(EXIT_ICON_X, FOOTER_TEXT_Y, 0.5, 0.0, icon, true);
        renderService.DrawCenterString(WINDOW_WIDTH / 2, FOOTER_TEXT_Y, L"でゲームを終了する", COLOR_TEXT_NORMAL);
    }

    IInputService& inputService;
    IRenderService& renderService;
    IAssetService& assetService;
    
    static constexpr int EXIT_ICON_X = 720;
    static constexpr int FOOTER_TEXT_Y = WINDOW_HEIGHT - MENU_BOX_MARGIN_Y_DOWN - 50;
};

std::unique_ptr<MenuPanel> CreateExitPanel(IInputService& input,
                                            IRenderService& render,
                                            IAssetService& asset)
{
    return std::make_unique<ExitPanel>(input, render, asset);
}

} // namespace mc
