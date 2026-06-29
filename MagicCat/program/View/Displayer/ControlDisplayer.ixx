module;

#include <memory>
#include <DxLib.h>


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
    }

    class ControlDisplayer : public Displayer
    {
        IAssetService& assetService;
        IRenderService& renderService;
        uint32_t color;

    public:
        ControlDisplayer(IAssetService& asset, IRenderService& render, uint32_t c = 0xFFFFFF)
            : assetService(asset), renderService(render), color(c) {}

        void OnDraw(float deltaTime) const override
        {
            int icon = assetService.GetImageHandle(EImage::KB_Q);
            DrawRotaGraphF(KB_Q_X, Y, 1.0, 0.0, icon, TRUE);
            renderService.DrawString(KB_Q_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"カードを引く", color);

            icon = assetService.GetImageHandle(EImage::KB_R);
            DrawRotaGraphF(KB_R_X, Y, 1.0, 0.0, icon, TRUE);
            renderService.DrawString(KB_R_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"ルールを見る", color);

            icon = assetService.GetImageHandle(EImage::KB_UP);
            DrawRotaGraphF(KB_UP_X, Y, 1.0, 0.0, icon, TRUE);

            icon = assetService.GetImageHandle(EImage::KB_DOWN);
            DrawRotaGraphF(KB_DOWN_X, Y, 1.0, 0.0, icon, TRUE);

            icon = assetService.GetImageHandle(EImage::KB_SPACE);
            DrawRotaGraphF(KB_SPACE_X, Y, 1.0, 0.0, icon, TRUE);
            renderService.DrawString(KB_SPACE_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"アクション", color);
        }
    };

    export std::unique_ptr<Displayer> CreateControlDisplayer(IAssetService& assetService, IRenderService& renderService,
                                                             uint32_t color = 0xFFFFFF)
    {
        return std::make_unique<ControlDisplayer>(assetService, renderService, color);
    }
} // namespace mc
