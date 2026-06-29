module;

#include <memory>
#include <RenderUtils.h>

module SceneService;

import InputService;
import RenderService;
import AssetService;

namespace mc {
    namespace {
        constexpr int OVERLAY_ALPHA = 200;

        constexpr int BOX_MARGIN_X = 200;
        constexpr int BOX_MARGIN_Y = 150;

        constexpr int TEXT_START_OFFSET_X = 50;
        constexpr int TEXT_START_OFFSET_Y = 50;

        constexpr int CONTENT_START_OFFSET_Y = 140;
        constexpr int LINE_SPACING = 60;

        constexpr int INDENT_LEVEL_1 = 50;
        constexpr int INDENT_LEVEL_2 = 80;

        constexpr int SECTION_SPACING = 280;

        constexpr int ICON_OFFSET_X = 320;
        constexpr int ICON_OFFSET_Y = 80;
        constexpr int ICON_TEXT_OFFSET_X = 250;
        constexpr int ICON_TEXT_OFFSET_Y = 72;

        constexpr uint32_t COLOR_BOX_BG = 0x1E1E28;
    }

    class RulesScene : public IScene
    {
        IInputService& inputService;
        ISceneService& sceneService;
        IAssetService& assetService;
        IRenderService& renderService;

        int currentPage = 0;

    public:
        RulesScene(IInputService& input, ISceneService& scene, IAssetService& asset, IRenderService& render)
            : inputService(input), sceneService(scene), assetService(asset), renderService(render) {}

        void Start() override {}


        void Update(float deltaTime) override
        {
            if (inputService.IsPressed(InputAction::IgShowRules))
            {
                sceneService.PopScene();
                return;
            }

            if (inputService.IsPressed(InputAction::IgLeft) && currentPage > 0)
            {
                currentPage--;
            }
            if (inputService.IsPressed(InputAction::IgRight) && currentPage < 1)
            {
                currentPage++;
            }

            renderService.SetDrawBlendMode(BlendMode::Alpha, OVERLAY_ALPHA);
            renderService.DrawBoxAA(0.f, 0.f, static_cast<float>(renderService.GetWindowWidth()), static_cast<float>(renderService.GetWindowHeight()), COLOR_BLACK, true);
            renderService.SetDrawBlendMode(BlendMode::NoBlend, 0);

            constexpr int boxX1 = BOX_MARGIN_X;
            constexpr int boxY1 = BOX_MARGIN_Y;

            const int boxX2 = renderService.GetWindowWidth() - BOX_MARGIN_X;
            const int boxY2 = renderService.GetWindowHeight() - BOX_MARGIN_Y;

            renderService.DrawBoxAA(static_cast<float>(boxX1), static_cast<float>(boxY1),
                                    static_cast<float>(boxX2), static_cast<float>(boxY2), COLOR_BOX_BG, true);
            renderService.DrawHollowBox(boxX1, boxY1, boxX2, boxY2, 3, COLOR_WHITE);

            renderService.DrawString(boxX1 + TEXT_START_OFFSET_X, boxY1 + TEXT_START_OFFSET_Y,
                                     L"【ルール説明】", COLOR_WHITE);

            int textY = boxY1 + CONTENT_START_OFFSET_Y;
            if (currentPage == 0)
            {
                renderService.DrawString(boxX1 + INDENT_LEVEL_1, textY,
                                         L"プレイヤーの魔法スキル：", COLOR_TEXT_NORMAL);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING,
                                         L"【透視】1ゲームに1回のみ。敵の出す手を完全に可視化する。", COLOR_TEXT_BLUE);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING * 2,
                                         L"【⚔UP】制限なし。そのターンの間、自分の攻撃力が+2される。", COLOR_TEXT_BLUE);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING * 3,
                                         L"【回復】1ゲームに3回まで。自分のHPを1回復する。", COLOR_TEXT_BLUE);

                renderService.DrawString(boxX1 + INDENT_LEVEL_1, textY + SECTION_SPACING,
                                         L"表示される記号の意味：", COLOR_TEXT_NORMAL);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING,
                                         L"⚖：敵がその手を出す確率の高さ。大きいほど出しやすい。", COLOR_TEXT_RED);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING * 2,
                                         L"⚔：その手で勝った時に相手に与えるダメージ量。", COLOR_TEXT_RED);
            }
            else if (currentPage == 1)
            {
                renderService.DrawString(boxX1 + INDENT_LEVEL_1, textY,
                                         L"仕組み: カードを引くと即座に効果が発動します", COLOR_TEXT_NORMAL);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING,
                                         L"魔法カード：MPが回復します。", COLOR_TEXT_BLUE);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING * 2,
                                         L"じゃんけんカード：敵の該当する攻撃のウェイトが上昇します。", COLOR_TEXT_RED);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING * 3,
                                         L"手札は4枚までです。", COLOR_TEXT_GREEN);

                renderService.DrawString(boxX1 + INDENT_LEVEL_1, textY + SECTION_SPACING,
                                         L"アクション: 【カードを引く】【魔法を使用する】【攻撃する】", COLOR_TEXT_NORMAL);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING,
                                         L"魔法を使用するとMPが必要。効果は何か試してみてください。", COLOR_TEXT_GREEN);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING * 2,
                                         L"攻撃した後、ターンが終了します。", COLOR_TEXT_RED);
                renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING * 3,
                                         L"ターンが終了すると、敵のウェイトと手札はリセットされます。", COLOR_TEXT_RED);
            }


            int kbrHandle = assetService.GetImageHandle(EImage::KB_R);
            renderService.DrawGraph(boxX2 - ICON_OFFSET_X, boxY2 - ICON_OFFSET_Y, kbrHandle, true);
            renderService.DrawString(boxX2 - ICON_TEXT_OFFSET_X, boxY2 - ICON_TEXT_OFFSET_Y,
                                     L"押して戻る", COLOR_TEXT_NORMAL);
            renderService.DrawCenterString(renderService.GetWindowWidth() / 2, boxY2 - ICON_TEXT_OFFSET_Y,
                                           L"(◀/▶ でページ切替)", COLOR_TEXT_NORMAL);
        }
    };

    std::unique_ptr<IScene> CreateRulesScene(IInputService& inputService, ISceneService& sceneService,
                                             IAssetService& assetService, IRenderService& renderService)
    {
        return std::make_unique<RulesScene>(inputService, sceneService, assetService, renderService);
    }
}
