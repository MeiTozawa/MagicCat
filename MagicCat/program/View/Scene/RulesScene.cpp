module;

#include <memory>
#include <RenderUtils.h>

module SceneService;

import InputService;
import RenderService;
import AssetService;

namespace mc
{
    namespace
    {
        constexpr int OVERLAY_ALPHA = 200;

        constexpr int BOX_MARGIN_X = 200;
        constexpr int BOX_MARGIN_Y = 150;

        constexpr int TEXT_START_OFFSET_X = 50;
        constexpr int TEXT_START_OFFSET_Y = 50;

        constexpr int CONTENT_START_OFFSET_Y = 140;
        constexpr int LINE_SPACING = 70;

        constexpr int INDENT_LEVEL_1 = 50;
        constexpr int INDENT_LEVEL_2 = 80;

        constexpr int SECTION_SPACING = 250;

        constexpr int ICON_OFFSET_X = 320;
        constexpr int ICON_OFFSET_Y = 80;
        constexpr int ICON_TEXT_OFFSET_X = 250;
        constexpr int ICON_TEXT_OFFSET_Y = 72;

        constexpr uint32_t COLOR_BOX_BG = 0x1E1E28;
    }

    class RulesScene : public IScene
    {
        IInputService* inputService;
        ISceneService* sceneService;
        IAssetService* assetService;
        IRenderService* renderService;

    public:
        RulesScene(IInputService* input, ISceneService* scene, IAssetService* asset, IRenderService* render)
            : inputService(input), sceneService(scene), assetService(asset), renderService(render) {}

        void Start() override {}


        void Update(float deltaTime) override
        {
            if (inputService->IsPressed(InputAction::IgShowRules))
            {
                sceneService->PopScene();
                return;
            }

            SetDrawBlendMode(DX_BLENDMODE_ALPHA, OVERLAY_ALPHA);
            DrawBox(0, 0, GetWindowWidth(), GetWindowHeight(), COLOR_BLACK, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

            constexpr int boxX1 = BOX_MARGIN_X;
            constexpr int boxY1 = BOX_MARGIN_Y;

            const int boxX2 = GetWindowWidth() - BOX_MARGIN_X;
            const int boxY2 = GetWindowHeight() - BOX_MARGIN_Y;

            DrawBox(boxX1, boxY1, boxX2, boxY2, COLOR_BOX_BG, TRUE);
            DrawHollowBox(renderService, boxX1, boxY1, boxX2, boxY2, 3, COLOR_WHITE);

            renderService->DrawString(boxX1 + TEXT_START_OFFSET_X, boxY1 + TEXT_START_OFFSET_Y,
                                      L"【ルール説明】", COLOR_WHITE);

            int textY = boxY1 + CONTENT_START_OFFSET_Y;
            renderService->DrawString(boxX1 + INDENT_LEVEL_1, textY,
                                      L"仕組み: カードを引くと即座に効果が発動します", COLOR_TEXT_NORMAL);
            renderService->DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING,
                                      L"魔法カード：MPが回復しする。", COLOR_TEXT_BLUE);
            renderService->DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING * 2,
                                      L"じゃんけんカード：敵の該当する攻撃のウェイトが上昇する。", COLOR_TEXT_RED);

            renderService->DrawString(boxX1 + INDENT_LEVEL_1, textY + SECTION_SPACING,
                                      L"アクション: 【カードを引く】【攻撃する】【魔法を使用する】", COLOR_TEXT_NORMAL);
            renderService->DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING,
                                      L"魔法を使用するとMPが必要。効果は何か試してごらん。", COLOR_TEXT_GREEN);
            renderService->DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING * 2,
                                      L"攻撃した後、ターンが終了する。", COLOR_TEXT_RED);
            renderService->DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING * 3,
                                      L"ターンが終了すると、敵のウェイトと手札はリセットされる。", COLOR_TEXT_RED);

            int kbrHandle = assetService->GetImageHandle(EImage::KB_R);
            DrawGraph(boxX2 - ICON_OFFSET_X, boxY2 - ICON_OFFSET_Y, kbrHandle, TRUE);
            renderService->DrawString(boxX2 - ICON_TEXT_OFFSET_X, boxY2 - ICON_TEXT_OFFSET_Y, L"押して戻る",
                                      COLOR_TEXT_NORMAL);
        }
    };

    std::unique_ptr<IScene> CreateRulesScene(IInputService* inputService, ISceneService* sceneService,
                                             IAssetService* assetService, IRenderService* renderService)
    {
        return std::make_unique<RulesScene>(inputService, sceneService, assetService, renderService);
    }
}
