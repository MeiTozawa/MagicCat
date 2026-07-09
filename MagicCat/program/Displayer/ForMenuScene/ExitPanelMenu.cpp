module;

#include <memory>
#include <RenderUtils.h>

module Displayer;

import InputService;
import RenderService;

namespace mc {

class ExitPanel : public MenuPanel {
public:
    ExitPanel(int boxX1, int textY, IInputService& input, IRenderService& render)
        : MenuPanel(boxX1, textY)
        , inputService(input)
        , renderService(render)
    {}
    bool UpdatePanel(float deltaTime) override
    {
        Update(deltaTime);
        if (inputService.IsPressed(InputAction::Left))
        {
            return false;
        }
        return true;
    }

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

std::unique_ptr<MenuPanel> CreateExitPanel(int boxX1, int textY,
                                            IInputService& input,
                                            IRenderService& render)
{
    return std::make_unique<ExitPanel>(boxX1, textY, input, render);
}

} // namespace mc
