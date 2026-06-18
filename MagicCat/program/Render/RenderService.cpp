module;

#include <string>
#include <cstdint>
#include "DxLib.h"

module RenderService;
import ServiceLocator;

namespace mc
{
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

        void DrawRectRotaGraph(int x, int y, int srcX, int srcY, int width, int height, double extRate, double angle, int graphHandle, bool transFlag, bool turnFlag) override
        {
            ::DrawRectRotaGraph(x, y, srcX, srcY, width, height, extRate, angle, graphHandle, transFlag ? TRUE : FALSE, turnFlag ? TRUE : FALSE);
        }

        void SetDrawBlendMode(int blendMode, int pal) override
        {
            ::SetDrawBlendMode(blendMode, pal);
        }
    };

    Shared<IRenderService> CreateRenderService()
    {
        return std::make_shared<DxLibRenderService>();
    }
} // namespace mc
