module;

#include <cstdint>

export module RenderService;

namespace mc {
    /// @brief SetDrawBlendMode に渡すブレンドモード（DxLib の DX_BLENDMODE_* に対応）
    export enum class BlendMode
    {
        NoBlend,  ///< DX_BLENDMODE_NOBLEND
        Alpha,    ///< DX_BLENDMODE_ALPHA
    };

    export class IRenderService
    {
    public:
        virtual ~IRenderService() = default;

        // ---- 描画 ----
        virtual int GetFontSize() = 0;
        virtual void DrawString(int x, int y, const wchar_t* text, uint32_t color) = 0;
        virtual void DrawBoxAA(float x1, float y1, float x2, float y2, uint32_t color, bool fillFlag) = 0;
        virtual int GetDrawStringWidth(const wchar_t* text) = 0;
        virtual void DrawGraph(int x, int y, int handle, bool transFlag) = 0;
        virtual void DrawRotaGraphF(float x, float y, double extRate, double angle,
                                    int graphHandle, bool transFlag) = 0;
        virtual void DrawRectRotaGraph(int x, int y, int srcX, int srcY, int width, int height, double extRate,
                                       double angle, int graphHandle, bool transFlag, bool turnFlag) = 0;
        virtual void SetDrawBlendMode(BlendMode mode, int pal) = 0;
        virtual void SetDrawBright(int r, int g, int b) = 0;
        virtual void DrawRoundRectFrame(int x1, int y1, int x2, int y2,
                                        int cornerRadius, int thickness, uint32_t color) = 0;

        /// @brief 塗り潰し三角形を描画する
        virtual void DrawFilledTriangle(int x1, int y1, int x2, int y2,
                                        int x3, int y3, uint32_t color) = 0;

        /// @brief 文字列を中央揃えで描画する（x, y は中心座標）
        virtual void DrawCenterString(int x, int y, const wchar_t* text, uint32_t color) = 0;
        /// @brief 文字列を左揃えで描画する（x は左端、y は中心Y座標）
        virtual void DrawLeftString(int x, int y, const wchar_t* text, uint32_t color) = 0;
        /// @brief 文字列を右揃えで描画する（x は右端、y は中心Y座標）
        virtual void DrawRightString(int x, int y, const wchar_t* text, uint32_t color) = 0;
        /// @brief 中空の矩形（枠線）を太さを指定して描画する
        virtual void DrawHollowBox(int x1, int y1, int x2, int y2, int thickness, uint32_t color) = 0;

        // ---- ウィンドウ ----
        virtual int GetWindowWidth() const = 0;
        virtual int GetWindowHeight() const = 0;

        // ---- 初期化・フォント設定 ----
        /// @brief フォントタイプを通常モードに設定する（DX_FONTTYPE_NORMAL）
        virtual void SetFontTypeNormal() = 0;
        virtual void ChangeFont(const wchar_t* fontName) = 0;
        virtual void SetFontSize(int size) = 0;
        virtual void SetFontThickness(int thickness) = 0;
        virtual void SetBackgroundColor(int r, int g, int b) = 0;
    };

    export std::unique_ptr<IRenderService> CreateRenderService();
} // namespace mc
