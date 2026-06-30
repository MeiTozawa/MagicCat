module;

#include <memory>
#include <string>
#include <RenderUtils.h>

export module Displayer:Dialog;
import DisplayerBase;
import RenderService;

namespace mc {
    namespace {
        constexpr int DIALOG_PADDING_X = 24;
        constexpr int DIALOG_PADDING_Y = 12;
        constexpr int DIALOG_BORDER = 3;
        constexpr int DIALOG_RADIUS = 14;
        constexpr int DIALOG_TAIL_WIDTH = 22;
        constexpr int DIALOG_TAIL_HEIGHT = 16;
    }

    export class DialogDisplayer : public Displayer
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

    private:
        void OnDraw(float deltaTime) const override
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

            // プレイヤー（吹き出しの左上方向）を指す小さな三角形
            int tailCenterX = x1 + boxWidth / 4;
            int tailHalf = DIALOG_TAIL_WIDTH / 2;
            renderService.DrawFilledTriangle(tailCenterX - tailHalf, y1, tailCenterX + tailHalf, y1,
                                             tailCenterX, y1 - DIALOG_TAIL_HEIGHT, color);

            // 背景は透明にして、角丸の枠線のみを描画する
            renderService.DrawRoundRectFrame(x1, y1, x2, y2, DIALOG_RADIUS, DIALOG_BORDER, color);

            renderService.DrawCenterString(centerX, y1 + boxHeight / 2, text.c_str(), color);
        }
    };

    export std::unique_ptr<DialogDisplayer> CreateDialogDisplayer(
        IRenderService& renderService, int centerX, int topY)
    {
        return std::make_unique<DialogDisplayer>(renderService, centerX, topY);
    }
}
