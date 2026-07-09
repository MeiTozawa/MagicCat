module;

#include <memory>
#include <RenderUtils.h>

module Displayer;

import InputService;
import RenderService;

namespace mc {

class ExitPanel : public MenuPanel {
public:
    ExitPanel(IInputService& input, IRenderService& render)
        : MenuPanel()
        , inputService(input)
        , renderService(render)
    {}

private:
    void OnUpdate(float /*deltaTime*/) override
    {
        if (inputService.IsPressed(InputAction::Confirm))
        {
            renderService.ExitApplication();
            return;
        }
    }

    void OnDraw(float /*deltaTime*/) const override
    {
        renderService.DrawCenterString(WINDOW_WIDTH / 2, y,
                                 L"ゲームを終了しますか？", COLOR_TEXT_NORMAL);
        renderService.DrawCenterString(WINDOW_WIDTH / 2, y + 100,
                                 L"保存されていない内容は失われます！", COLOR_TEXT_RED);
    }

    IInputService& inputService;
    IRenderService& renderService;
};

std::unique_ptr<MenuPanel> CreateExitPanel(IInputService& input,
                                            IRenderService& render)
{
    return std::make_unique<ExitPanel>(input, render);
}

} // namespace mc
