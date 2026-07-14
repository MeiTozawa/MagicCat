module;

#include <cassert>
#include <memory>
#include <RenderUtils.h>

module Displayer;

import InputService;
import RenderService;
import AssetService;

namespace mc {
    class RulesPanel : public MenuPanel
    {
    public:
        RulesPanel(IInputService& input, IRenderService& render, IAssetService& asset)
            : MenuPanel()
              , inputService(input)
              , renderService(render)
              , assetService(asset) {}

    protected:
        void OnUpdate(float /*deltaTime*/) override
        {
            if (inputService.IsPressed(InputAction::Confirm))
            {
                NextPage();
                return;
            }
            auto click = inputService.OnMouseClick(InputAction::MouseClick);
            if (click.In(MENU_BOX_RECT))
                NextPage();
        }

        void OnDraw(float /*deltaTime*/) const override
        {
            switch (currentPage)
            {
            case 0: DrawPage0();
                break;
            case 1: DrawPage1();
                break;
            case 2: DrawPage2();
                break;
            default: break;
            }
            DrawNavigationHints();
        }

    private:
        void NextPage()
        {
            currentPage = (currentPage + 1) % PAGE_COUNT;
        }

        void DrawNavigationHints() const
        {
            int icon;
            switch (inputService.GetActiveDevice())
            {
            case InputDevice::Keyboard: icon = assetService.GetImageHandle(EImage::KB_SPACE);
                break;
            case InputDevice::Mouse: icon = assetService.GetImageHandle(EImage::MOUSE_LEFT);
                break;
            case InputDevice::Gamepad: icon = assetService.GetImageHandle(EImage::XBOX_A);
                break;
            default: assert(false && "未知のデバイス");
                icon = assetService.GetImageHandle(EImage::MOUSE_LEFT);
            }
            renderService.DrawRotaGraphF(NEXT_PAGE_ICON_X, FOOTER_TEXT_Y, 0.5, 0.0, icon, true);
            renderService.DrawCenterString(WINDOW_WIDTH / 2, FOOTER_TEXT_Y, L"でページ切替", COLOR_TEXT_NORMAL);

            renderService.DrawCenterString(
                GetX() + PAGE_COUNT_OFFSET_X, FOOTER_TEXT_Y,
                std::format(L"{}/{} ページ", currentPage + 1, PAGE_COUNT).c_str(), COLOR_TEXT_NORMAL);
        }

        void DrawPage0() const
        {
            renderService.DrawString(GetX() + INDENT_LEVEL_1, GetY(),
                                     L"アクション: 【カードを引く】【魔法を使用する】【攻撃する】", COLOR_TEXT_NORMAL);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING,
                                     L"攻撃を行う前に、カードを引いたり、魔法を使ったりすることができます。", COLOR_TEXT_GREEN);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 2,
                                     L"攻撃した後、ターンが終了します。", COLOR_TEXT_RED);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 3,
                                     L"ターンが終了すると、敵の⚖の変更値はリセットされます。", COLOR_TEXT_RED);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 4,
                                     L"ターンが終了すると、手札は捨て札に捨てられます。", COLOR_TEXT_RED);

            renderService.DrawString(GetX() + INDENT_LEVEL_1, GetY() + SECTION_SPACING,
                                     L"表示される記号の意味：", COLOR_TEXT_NORMAL);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + SECTION_SPACING + LINE_SPACING,
                                     L"⚖：敵がその手を出す確率の高さ。大きいほど出しやすい。", COLOR_TEXT_RED);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + SECTION_SPACING + LINE_SPACING * 2,
                                     L"⚔：その手で勝った時に相手に与えるダメージ量。", COLOR_TEXT_RED);
        }

        void DrawPage1() const
        {
            renderService.DrawString(GetX() + INDENT_LEVEL_1, GetY(),
                                     L"カードの説明", COLOR_TEXT_NORMAL);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING,
                                     L"魔法カード：MPが回復します。", COLOR_TEXT_BLUE);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 2,
                                     L"じゃんけんカード：敵の該当する攻撃の⚖が上昇します。", COLOR_TEXT_GREEN);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 3,
                                     L"カードは即座に発動されます。", COLOR_TEXT_RED);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 5,
                                     L"手札は4枚までです。", COLOR_TEXT_GREEN);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 6,
                                     L"山札がなくなった場合、自動的に捨て札をシャッフルして、", COLOR_TEXT_RED);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 7,
                                     L"それをドロー山札として再構成します。", COLOR_TEXT_RED);
        }

        void DrawPage2() const
        {
            renderService.DrawString(GetX() + INDENT_LEVEL_1, GetY(),
                                     L"プレイヤーの魔法スキル：", COLOR_TEXT_NORMAL);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING,
                                     L"【透視】1バトルに1回のみ。敵の出す手を完全に可視化する。", COLOR_TEXT_BLUE);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 2,
                                     L"【⚔UP】制限なし。そのターンの間、自分の攻撃力が+2される。", COLOR_TEXT_BLUE);
            renderService.DrawString(GetX() + INDENT_LEVEL_2, GetY() + LINE_SPACING * 3,
                                     L"【回復】1バトルに3回まで。自分のHPを2回復する。", COLOR_TEXT_BLUE);
        }

        IInputService& inputService;
        IRenderService& renderService;
        IAssetService& assetService;
        int currentPage = 0;

        static constexpr int LINE_SPACING = 60;
        static constexpr int INDENT_LEVEL_1 = 50;
        static constexpr int INDENT_LEVEL_2 = 80;
        static constexpr int SECTION_SPACING = 280;
        static constexpr int PAGE_COUNT = 3;
        static constexpr int PAGE_COUNT_OFFSET_X = 150;
        static constexpr int NEXT_PAGE_ICON_X = 780;
        static constexpr int FOOTER_TEXT_Y = WINDOW_HEIGHT - MENU_BOX_MARGIN_Y_DOWN - 50;
    };

    std::unique_ptr<MenuPanel> CreateRulesPanel(IInputService& input,
                                                IRenderService& render,
                                                IAssetService& asset)
    {
        return std::make_unique<RulesPanel>(input, render, asset);
    }
}
