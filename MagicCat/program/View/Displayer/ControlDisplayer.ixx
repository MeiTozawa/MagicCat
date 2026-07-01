module;

#include <memory>

export module Displayer:Control;
import DisplayerBase;

import AssetService;
import RenderService;


namespace mc {
    namespace {
        constexpr int KB_Q_X = 100;
        constexpr int KB_UP_X = 500;
        constexpr int KB_DOWN_X = 560;
        constexpr int KB_SPACE_X = 620;
        constexpr int KB_R_X = 920;
        constexpr int Y = 100;

        constexpr int TEXT_OFFSET_X = 40;
        constexpr int TEXT_OFFSET_Y = -25;

        constexpr int ICON_HALF_W = 40;
        constexpr int ICON_HALF_H = 28;
        constexpr uint32_t COLOR_HOVER = 0xFFFF00; // COLOR_YELLOW
    }

    class ControlDisplayer : public Displayer
    {
        IAssetService& assetService;
        IRenderService& renderService;
        uint32_t color;

    public:
        ControlDisplayer(IAssetService& asset, IRenderService& render, uint32_t c = 0xFFFFFF)
            : assetService(asset), renderService(render), color(c) {}

    private:
        void OnDraw(float deltaTime) const override
        {
            // マウス座標を毎フレーム直接取得（Pressed 判定不要）
            int mx = 0, my = 0;
            GetMousePoint(&mx, &my);

            bool hoverQ = (mx >= KB_Q_X - ICON_HALF_W && mx < KB_Q_X + ICON_HALF_W &&
                my >= Y - ICON_HALF_H && my < Y + ICON_HALF_H);
            bool hoverR = (mx >= KB_R_X - ICON_HALF_W && mx < KB_R_X + ICON_HALF_W &&
                my >= Y - ICON_HALF_H && my < Y + ICON_HALF_H);

            // KB_Q アイコン描画
            int icon = assetService.GetImageHandle(EImage::KB_Q);
            renderService.DrawRotaGraphF(KB_Q_X, Y, 1.0, 0.0, icon, true);
            uint32_t qColor = hoverQ ? COLOR_HOVER : color;
            renderService.DrawString(KB_Q_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"カードを引く", qColor);
            if (hoverQ)
                renderService.DrawHollowBox(KB_Q_X - ICON_HALF_W, Y - ICON_HALF_H,
                                            KB_Q_X + ICON_HALF_W, Y + ICON_HALF_H, 2, COLOR_HOVER);

            // KB_R アイコン描画
            icon = assetService.GetImageHandle(EImage::KB_R);
            renderService.DrawRotaGraphF(KB_R_X, Y, 1.0, 0.0, icon, true);
            uint32_t rColor = hoverR ? COLOR_HOVER : color;
            renderService.DrawString(KB_R_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"ルールを見る", rColor);
            if (hoverR)
                renderService.DrawHollowBox(KB_R_X - ICON_HALF_W, Y - ICON_HALF_H,
                                            KB_R_X + ICON_HALF_W, Y + ICON_HALF_H, 2, COLOR_HOVER);

            // 既存の KB_UP / KB_DOWN / KB_SPACE 描画（変更なし）
            icon = assetService.GetImageHandle(EImage::KB_UP);
            renderService.DrawRotaGraphF(KB_UP_X, Y, 1.0, 0.0, icon, true);

            icon = assetService.GetImageHandle(EImage::KB_DOWN);
            renderService.DrawRotaGraphF(KB_DOWN_X, Y, 1.0, 0.0, icon, true);

            icon = assetService.GetImageHandle(EImage::KB_SPACE);
            renderService.DrawRotaGraphF(KB_SPACE_X, Y, 1.0, 0.0, icon, true);
            renderService.DrawString(KB_SPACE_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"選択する", color);
        }
    };

    export std::unique_ptr<Displayer> CreateControlDisplayer(IAssetService& assetService, IRenderService& renderService,
                                                             uint32_t color = 0xFFFFFF)
    {
        return std::make_unique<ControlDisplayer>(assetService, renderService, color);
    }
} // namespace mc
