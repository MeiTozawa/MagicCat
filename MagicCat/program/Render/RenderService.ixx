module;

#include <cstdint>

export module RenderService;

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

    export std::unique_ptr<IRenderService> CreateRenderService();
} // namespace mc
