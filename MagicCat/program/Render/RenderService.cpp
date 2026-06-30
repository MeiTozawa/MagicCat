module;

#include <cstdint>
#include <dxe.h>

module RenderService;
namespace mc {
    class DxLibRenderService : public IRenderService
    {
    public:
        // ---- フォントサイズ ----
        int GetFontSize() override
        {
            return ::GetFontSize();
        }

        // ---- 描画 ----
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

        void DrawRotaGraphF(float x, float y, double extRate, double angle,
                            int graphHandle, bool transFlag) override
        {
            ::DrawRotaGraphF(x, y, extRate, angle, graphHandle, transFlag ? TRUE : FALSE);
        }

        void DrawRectRotaGraph(int x, int y, int srcX, int srcY, int width, int height, double extRate, double angle,
                               int graphHandle, bool transFlag, bool turnFlag) override
        {
            ::DrawRectRotaGraph(x, y, srcX, srcY, width, height, extRate, angle, graphHandle,
                                transFlag ? TRUE : FALSE, turnFlag ? TRUE : FALSE);
        }

        void SetDrawBlendMode(BlendMode mode, int pal) override
        {
            int dxMode = DX_BLENDMODE_NOBLEND;
            switch (mode)
            {
            case BlendMode::Alpha:   dxMode = DX_BLENDMODE_ALPHA;   break;
            case BlendMode::NoBlend: dxMode = DX_BLENDMODE_NOBLEND; break;
            }
            ::SetDrawBlendMode(dxMode, pal);
        }

        void SetDrawBright(int r, int g, int b) override
        {
            ::SetDrawBright(r, g, b);
        }

        void DrawRoundRectFrame(int x1, int y1, int x2, int y2,
                                int cornerRadius, int thickness, uint32_t color) override
        {
            for (int i = 0; i < thickness; ++i)
                ::DrawRoundRect(x1 + i, y1 + i, x2 - i, y2 - i,
                                cornerRadius, cornerRadius, color, FALSE);
        }

        void DrawFilledTriangle(int x1, int y1, int x2, int y2,
                                int x3, int y3, uint32_t color) override
        {
            ::DrawTriangle(x1, y1, x2, y2, x3, y3, color, TRUE);
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

        // ---- ウィンドウサイズ ----
        int GetWindowWidth() const override
        {
            return static_cast<int>(dxe::GetWindowWidthF());
        }

        int GetWindowHeight() const override
        {
            return static_cast<int>(dxe::GetWindowHeightF());
        }

        // ---- 初期化・フォント設定 ----
        void SetFontTypeNormal() override
        {
            ::ChangeFontType(DX_FONTTYPE_NORMAL);
        }

        void ChangeFont(const wchar_t* fontName) override
        {
            ::ChangeFont(fontName);
        }

        void SetFontSize(int size) override
        {
            ::SetFontSize(size);
        }

        void SetFontThickness(int thickness) override
        {
            ::SetFontThickness(thickness);
        }

        void SetBackgroundColor(int r, int g, int b) override
        {
            ::SetBackgroundColor(r, g, b);
        }
    };

    std::unique_ptr<IRenderService> CreateRenderService()
    {
        return std::make_unique<DxLibRenderService>();
    }
} // namespace mc
