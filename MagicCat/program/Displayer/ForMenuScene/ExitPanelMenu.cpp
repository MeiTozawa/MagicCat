module;

#include <memory>
#include <RenderUtils.h>

module Displayer;

import RenderService;

namespace mc {

class ExitPanel : public MenuPanel {
public:
    ExitPanel(int boxX1, int textY, IRenderService& render)
        : MenuPanel(boxX1, textY)
        , renderService(render)
    {}

    bool IsActive() const override { return false; }
    void Activate()       override {}
    void Deactivate()     override {}

private:
    void OnUpdate(float /*deltaTime*/) override {}

    void OnDraw(float /*deltaTime*/) const override
    {
        renderService.DrawCenterString(WINDOW_WIDTH / 2, y,
                                 L"ゲームを終了しますか？", COLOR_TEXT_NORMAL);
        renderService.DrawCenterString(WINDOW_WIDTH / 2, y + 100,
                                 L"保存されていない内容は失われます！", COLOR_TEXT_RED);
    }

    IRenderService& renderService;
};

std::unique_ptr<MenuPanel> CreateExitPanel(int boxX1, int textY,
                                            IRenderService& render)
{
    return std::make_unique<ExitPanel>(boxX1, textY, render);
}

} // namespace mc
