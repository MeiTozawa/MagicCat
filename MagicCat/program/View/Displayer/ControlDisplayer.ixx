module;

#include <memory>
#include <DxLib.h>
#include <RenderUtils.h>

export module Displayer:Control;
import DisplayerBase;

import AssetService;
import RenderService;
import InputService;


namespace mc {
    namespace {
        // アイコン描画 X 座標
        constexpr int ICON_DRAW_X  = 100;   // カードを引く
        constexpr int ICON_NAV_X   = 500;   // 上下移動（キーボード: 上キー / パッド: D-pad）
        constexpr int ICON_NAV2_X  = 560;   // 下キー（キーボードのみ）
        constexpr int ICON_CONF_X  = 620;   // 確定（Space / A）
        constexpr int ICON_RULES_X = 920;   // ルールを見る（Esc / Menu）
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
        IInputService& inputService;
        uint32_t color;

    public:
        ControlDisplayer(IAssetService& asset, IRenderService& render,
                         IInputService& input, uint32_t c = 0xFFFFFF)
            : assetService(asset), renderService(render), inputService(input), color(c) {}

    private:
        void OnDraw(float) const override
        {
            const bool isGamepad = (inputService.GetActiveDevice() == InputDevice::Gamepad);

            // マウスホバー検出（キーボードモードのみ意味がある）
            auto mousePos = inputService.GetMousePosition();
            int mx = mousePos.x;
            int my = mousePos.y;

            bool hoverDraw  = !isGamepad &&
                              mx >= ICON_DRAW_X - ICON_HALF_W && mx < ICON_DRAW_X + ICON_HALF_W &&
                              my >= Y - ICON_HALF_H            && my < Y + ICON_HALF_H;
            bool hoverRules = !isGamepad &&
                              mx >= ICON_RULES_X - ICON_HALF_W && mx < ICON_RULES_X + ICON_HALF_W &&
                              my >= Y - ICON_HALF_H              && my < Y + ICON_HALF_H;

            // ---- カードを引く ----
            {
                EImage drawIcon = isGamepad ? EImage::XBOX_X : EImage::KB_Q;
                int icon = assetService.GetImageHandle(drawIcon);
                renderService.DrawRotaGraphF(ICON_DRAW_X, Y, 0.5, 0.0, icon, true);
                uint32_t c = hoverDraw ? COLOR_HOVER : color;
                renderService.DrawString(ICON_DRAW_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"カードを引く", c);
                if (hoverDraw)
                    renderService.DrawHollowBox(ICON_DRAW_X - ICON_HALF_W, Y - ICON_HALF_H,
                                                ICON_DRAW_X + ICON_HALF_W, Y + ICON_HALF_H, 2, COLOR_HOVER);
            }

            // ---- ルールを見る ----
            {
                EImage rulesIcon = isGamepad ? EImage::BUTTON_MENU : EImage::KB_ESCAPE;
                int icon = assetService.GetImageHandle(rulesIcon);
                renderService.DrawRotaGraphF(ICON_RULES_X, Y, 0.5, 0.0, icon, true);
                uint32_t c = hoverRules ? COLOR_HOVER : color;
                renderService.DrawString(ICON_RULES_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"ルールを見る", c);
                if (hoverRules)
                    renderService.DrawHollowBox(ICON_RULES_X - ICON_HALF_W, Y - ICON_HALF_H,
                                                ICON_RULES_X + ICON_HALF_W, Y + ICON_HALF_H, 2, COLOR_HOVER);
            }

            // ---- 上下移動 ----
            if (isGamepad)
            {
                // D-pad 横アイコン1枚で上下を表現
                int icon = assetService.GetImageHandle(EImage::XBOX_DPAD_HORIZONTAL);
                renderService.DrawRotaGraphF(ICON_NAV_X, Y, 0.5, 0.0, icon, true);
            }
            else
            {
                int icon = assetService.GetImageHandle(EImage::KB_UP);
                renderService.DrawRotaGraphF(ICON_NAV_X, Y, 0.5, 0.0, icon, true);

                icon = assetService.GetImageHandle(EImage::KB_DOWN);
                renderService.DrawRotaGraphF(ICON_NAV2_X, Y, 0.5, 0.0, icon, true);
            }

            // ---- 確定 ----
            {
                EImage confIcon = isGamepad ? EImage::XBOX_A : EImage::KB_SPACE;
                int icon = assetService.GetImageHandle(confIcon);
                renderService.DrawRotaGraphF(ICON_CONF_X, Y, 0.5, 0.0, icon, true);
                renderService.DrawString(ICON_CONF_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"選択する", color);
            }
        }
    };

    export std::unique_ptr<Displayer> CreateControlDisplayer(IAssetService& assetService,
                                                             IRenderService& renderService,
                                                             IInputService& inputService,
                                                             uint32_t color = 0xFFFFFF)
    {
        return std::make_unique<ControlDisplayer>(assetService, renderService, inputService, color);
    }
} // namespace mc
