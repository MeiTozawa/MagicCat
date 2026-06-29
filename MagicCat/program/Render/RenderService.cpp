module;

#include <string>
#include <cstdint>
#include "DxLib.h"

module RenderService;
namespace mc {
    class DxLibRenderService : public IRenderService
    {
    public:
        int GetFontSize() override
        {
            return ::GetFontSize();
        }

        void DrawString(int x, int y, const wchar_t* text, uint32_t color) override
        {
            ::DrawString(x, y, text, color);
        }

        void DrawBoxAA(float x1, float y1, float x2, float y2, uint32_t color, bool fillFlag) override
        {
            ::DrawBoxAA(x1, y1, x2, y2, color, fillFlag ? TRUE : FALSE);
        }

        int GetDrawStringWidth(const wchar_t* text) override
        {
            return ::GetDrawStringWidth(text, -1);
        }

        void DrawGraph(int x, int y, int handle, bool transFlag) override
        {
            ::DrawGraph(x, y, handle, transFlag ? TRUE : FALSE);
        }

        void DrawRectRotaGraph(int x, int y, int srcX, int srcY, int width, int height, double extRate, double angle,
                               int graphHandle, bool transFlag, bool turnFlag) override
        {
            ::DrawRectRotaGraph(x, y, srcX, srcY, width, height, extRate, angle, graphHandle, transFlag ? TRUE : FALSE,
                                turnFlag ? TRUE : FALSE);
        }

        void SetDrawBlendMode(int blendMode, int pal) override
        {
            ::SetDrawBlendMode(blendMode, pal);
        }

        void DrawRoundRectFrame(int x1, int y1, int x2, int y2,
                                int cornerRadius, int thickness, uint32_t color) override
        {
            for (int i = 0; i < thickness; ++i)
                ::DrawRoundRect(x1 + i, y1 + i, x2 - i, y2 - i,
                                cornerRadius, cornerRadius, color, FALSE);
        }

        void DrawCenterString(int x, int y, const wchar_t* text, uint32_t color) override
        {
            const int drawX = x - GetDrawStringWidth(text) / 2;
            const int drawY = y - GetFontSize() / 2;
            DrawString(drawX, drawY, text, color);
        }

        void DrawLeftString(int x, int y, const wchar_t* text, uint32_t color) override
        {
            const int drawY = y - GetFontSize() / 2;
            DrawString(x, drawY, text, color);
        }

        void DrawRightString(int x, int y, const wchar_t* text, uint32_t color) override
        {
            const int drawX = x - GetDrawStringWidth(text);
            const int drawY = y - GetFontSize() / 2;
            DrawString(drawX, drawY, text, color);
        }

        void DrawHollowBox(int x1, int y1, int x2, int y2, int thickness, uint32_t color) override
        {
            DrawBoxAA(x1, y1, x2, y1 + thickness, color, true);
            DrawBoxAA(x1, y2 - thickness, x2, y2, color, true);
            DrawBoxAA(x1, y1, x1 + thickness, y2, color, true);
            DrawBoxAA(x2 - thickness, y1, x2, y2, color, true);
        }
    };

    std::unique_ptr<IRenderService> CreateRenderService()
    {
        return std::make_unique<DxLibRenderService>();
    }
} // namespace mc
