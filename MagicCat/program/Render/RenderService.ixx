module;

#include <cstdint>

export module RenderService;

import ServiceLocator;

namespace mc
{
    export class IRenderService
    {
    public:
        virtual ~IRenderService() = default;

        virtual int GetFontSize() = 0;
        virtual void DrawString(int x, int y, const wchar_t* text, uint32_t color) = 0;
        virtual void DrawBoxAA(float x1, float y1, float x2, float y2, uint32_t color, bool fillFlag) = 0;
        virtual int GetDrawStringWidth(const wchar_t* text) = 0;
        virtual void DrawGraph(int x, int y, int handle, bool transFlag) = 0;
        virtual void DrawRectRotaGraph(int x, int y, int srcX, int srcY, int width, int height, double extRate, double angle, int graphHandle, bool transFlag, bool turnFlag) = 0;
        virtual void SetDrawBlendMode(int blendMode, int pal) = 0;
    };

    export Shared<IRenderService> CreateRenderService();

    export inline int GetFontSize()
    {
        return ServiceLocator::Get<IRenderService>()->GetFontSize();
    }

    export inline void DrawString(int x, int y, const wchar_t* text, uint32_t color)
    {
        ServiceLocator::Get<IRenderService>()->DrawString(x, y, text, color);
    }

    export inline void DrawBoxAA(float x1, float y1, float x2, float y2, uint32_t color, bool fillFlag)
    {
        ServiceLocator::Get<IRenderService>()->DrawBoxAA(x1, y1, x2, y2, color, fillFlag);
    }

    export inline int GetDrawStringWidth(const wchar_t* text)
    {
        return ServiceLocator::Get<IRenderService>()->GetDrawStringWidth(text);
    }

    export inline void DrawGraph(int x, int y, int handle, bool transFlag)
    {
        ServiceLocator::Get<IRenderService>()->DrawGraph(x, y, handle, transFlag);
    }

    export inline void DrawRectRotaGraph(int x, int y, int srcX, int srcY, int width, int height, double extRate, double angle, int graphHandle, bool transFlag, bool turnFlag)
    {
        ServiceLocator::Get<IRenderService>()->DrawRectRotaGraph(x, y, srcX, srcY, width, height, extRate, angle, graphHandle, transFlag, turnFlag);
    }

    export inline void SetDrawBlendMode(int blendMode, int pal)
    {
        ServiceLocator::Get<IRenderService>()->SetDrawBlendMode(blendMode, pal);
    }
} // namespace mc
