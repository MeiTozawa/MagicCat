module;

#include <memory>
#include <DrawStringUtils.h>

module SceneService;

import InputService;
import AssetService;
import ServiceLocator;

namespace mc
{
    namespace {
        constexpr int OVERLAY_ALPHA = 200;

        constexpr int BOX_MARGIN_X = 200;
        constexpr int BOX_MARGIN_Y = 150;
        
        constexpr int TEXT_START_OFFSET_X = 50;
        constexpr int TEXT_START_OFFSET_Y = 50;
        
        constexpr int CONTENT_START_OFFSET_Y = 140;
        constexpr int LINE_SPACING = 70;
        
        constexpr int INDENT_LEVEL_1 = 50;
        constexpr int INDENT_LEVEL_2 = 80;
        constexpr int INDENT_LEVEL_3 = 100;
        
        constexpr int SECTION_SPACING = 300;

        constexpr int ICON_OFFSET_X = 320;
        constexpr int ICON_OFFSET_Y = 80;
        constexpr int ICON_TEXT_OFFSET_X = 250;
        constexpr int ICON_TEXT_OFFSET_Y = 72;

        constexpr uint32_t COLOR_BOX_BG = 0x1E1E28;
        constexpr uint32_t COLOR_BOX_BORDER = 0xC8C8C8;
    }

    class RulesScene : public IScene
    {
        IInputService* inputService = nullptr;
        ISceneService* sceneService = nullptr;
        IAssetService* assetService = nullptr;

    public:
        RulesScene() {}

        void Start() override
        {
            inputService = ServiceLocator::Get<IInputService>();
            sceneService = ServiceLocator::Get<ISceneService>();
            assetService = ServiceLocator::Get<IAssetService>();
        }

        void Update(float deltaTime) override
        {
            if (inputService->IsPressed(InputAction::IgShowRules))
            {
                sceneService->PopScene();
                return;
            }

            int kbrHandle = assetService->GetImageHandle(EImage::KB_R);

            SetDrawBlendMode(DX_BLENDMODE_ALPHA, OVERLAY_ALPHA);
            DrawBox(0, 0, GetWindowX(), GetWindowY(), COLOR_BLACK, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

            constexpr int boxX1 = BOX_MARGIN_X;
            constexpr int boxY1 = BOX_MARGIN_Y;
            const int boxX2 = GetWindowX() - BOX_MARGIN_X;
            const int boxY2 = GetWindowY() - BOX_MARGIN_Y;

            DrawBox(boxX1, boxY1, boxX2, boxY2, COLOR_BOX_BG, TRUE);
            DrawBox(boxX1, boxY1, boxX2, boxY2, COLOR_BOX_BORDER, FALSE);

            DrawString(boxX1 + TEXT_START_OFFSET_X, boxY1 + TEXT_START_OFFSET_Y, L"【ルール説明】", COLOR_WHITE);

            int textY = boxY1 + CONTENT_START_OFFSET_Y;
            DrawString(boxX1 + INDENT_LEVEL_1, textY, L"1. カードを引くと即座に効果が発動します。", COLOR_TEXT_NORMAL);
            DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING, L"・魔法カード：MPが回復しする。", COLOR_TEXT_BLUE);
            DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING * 2, L"・じゃんけんカード：敵の該当する攻撃のウェイトが上昇する。", COLOR_TEXT_RED);

            DrawString(boxX1 + INDENT_LEVEL_1, textY + SECTION_SPACING, L"2. プレイヤーは攻撃を選択するか、魔法を使用できます。", COLOR_TEXT_NORMAL);
            DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING, L"・魔法を使用するとMPを消費し、", COLOR_TEXT_GREEN);
            DrawString(boxX1 + INDENT_LEVEL_3, textY + SECTION_SPACING + LINE_SPACING * 2, L"敵の「攻撃力」と「基礎ウェイト」が見えるようになります。", COLOR_TEXT_GREEN);

            DrawGraph(boxX2 - ICON_OFFSET_X, boxY2 - ICON_OFFSET_Y, kbrHandle, TRUE);
            DrawString(boxX2 - ICON_TEXT_OFFSET_X, boxY2 - ICON_TEXT_OFFSET_Y, L"押して戻る", COLOR_TEXT_NORMAL);
        }
    };

    std::unique_ptr<IScene> CreateRulesScene()
    {
        return std::make_unique<RulesScene>();
    }
}
