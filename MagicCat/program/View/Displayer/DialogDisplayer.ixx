module;

#include <memory>
#include <string>
#include <RenderUtils.h>

export module Displayer:Dialog;
import DisplayerBase;
import RenderService;

namespace mc
{
    namespace
    {
        constexpr int DIALOG_PADDING_X = 24;
        constexpr int DIALOG_PADDING_Y = 12;
        constexpr int DIALOG_BORDER = 3;
    }

    export class DialogDisplayer : public IDisplayer
    {
        IRenderService& renderService;
        int centerX;
        int topY;
        std::wstring text;
        uint32_t color = COLOR_WHITE;

    public:
        DialogDisplayer(IRenderService& renderService, int centerX, int topY)
            : renderService(renderService), centerX(centerX), topY(topY) {}

        void SetMessage(const std::wstring& message, uint32_t messageColor)
        {
            text = message;
            color = messageColor;
        }

        void Update(float deltaTime) override {}

        void Draw(float deltaTime) const override
        {
            if (text.empty())
                return;

            int textWidth = renderService.GetDrawStringWidth(text.c_str());
            int textHeight = renderService.GetFontSize();

            int boxWidth = textWidth + DIALOG_PADDING_X * 2;
            int boxHeight = textHeight + DIALOG_PADDING_Y * 2;
            int x1 = centerX - boxWidth / 2;
            int y1 = topY;
            int x2 = x1 + boxWidth;
            int y2 = y1 + boxHeight;

            renderService.DrawBoxAA(x1, y1, x2, y2, COLOR_BLACK, true);
            DrawHollowBox(&renderService, x1, y1, x2, y2, DIALOG_BORDER, color);
            DrawCenterString(&renderService, centerX, y1 + boxHeight / 2, text, color);
        }
    };

    export std::unique_ptr<DialogDisplayer> CreateDialogDisplayer(
        IRenderService& renderService, int centerX, int topY)
    {
        return std::make_unique<DialogDisplayer>(renderService, centerX, topY);
    }
}
