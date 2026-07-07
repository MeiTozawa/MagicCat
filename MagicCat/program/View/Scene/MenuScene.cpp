module;

#include <algorithm>
#include <app_build_setting.h>
#include <memory>
#include <RenderUtils.h>

module SceneService;

import InputService;
import RenderService;
import AssetService;

namespace mc {
    namespace {
        constexpr int OVERLAY_ALPHA = 200;

        constexpr int BOX_MARGIN_X = 100;
        constexpr int BOX_MARGIN_Y_UP = 75;
        constexpr int BOX_MARGIN_Y_DOWN = 250;

        constexpr Rect<int> BOX_RECT = {
            BOX_MARGIN_X, BOX_MARGIN_Y_UP,
            WINDOW_WIDTH - BOX_MARGIN_X, WINDOW_HEIGHT - BOX_MARGIN_Y_DOWN
        };

        constexpr int TEXT_START_OFFSET_X = 50;
        constexpr int TEXT_START_OFFSET_Y = 50;

        constexpr int CONTENT_START_OFFSET_Y = 140;
        constexpr int LINE_SPACING = 60;

        constexpr int INDENT_LEVEL_1 = 50;
        constexpr int INDENT_LEVEL_2 = 80;

        constexpr int SECTION_SPACING = 280;

        constexpr int NEXT_PAGE_ICON_X = 780;
        constexpr int NEXT_PAGE_Y = WINDOW_HEIGHT - BOX_MARGIN_Y_DOWN - 50;

        constexpr uint32_t COLOR_BOX_BG = 0x1E1E28;

        constexpr int BUTTON_HEIGHT = 100;
        constexpr int BUTTON_WIDTH = 300;
        constexpr int BUTTON_OFFSET_X = 50;
        constexpr int BUTTON_COUNT = 5;

        constexpr int BUTTON_Y1 = 880;
        constexpr int BUTTON_Y2 = BUTTON_Y1 + BUTTON_HEIGHT;
        constexpr int BUTTON0_X = (WINDOW_WIDTH - BUTTON_WIDTH * BUTTON_COUNT - BUTTON_OFFSET_X * (BUTTON_COUNT - 1)) /
            2;

        constexpr int BUTTON1_X = BUTTON0_X + 1 * BUTTON_WIDTH + 1 * BUTTON_OFFSET_X;

        constexpr int BUTTON2_X = BUTTON0_X + 2 * BUTTON_WIDTH + 2 * BUTTON_OFFSET_X;

        constexpr int BUTTON3_X = BUTTON0_X + 3 * BUTTON_WIDTH + 3 * BUTTON_OFFSET_X;

        constexpr int BUTTON4_X = BUTTON0_X + 4 * BUTTON_WIDTH + 4 * BUTTON_OFFSET_X;

        constexpr Rect<int> BTN0_RECT = {BUTTON0_X, BUTTON_Y1, BUTTON0_X + BUTTON_WIDTH, BUTTON_Y2};
        constexpr Rect<int> BTN1_RECT = {BUTTON1_X, BUTTON_Y1, BUTTON1_X + BUTTON_WIDTH, BUTTON_Y2};
        constexpr Rect<int> BTN2_RECT = {BUTTON2_X, BUTTON_Y1, BUTTON2_X + BUTTON_WIDTH, BUTTON_Y2};
        constexpr Rect<int> BTN3_RECT = {BUTTON3_X, BUTTON_Y1, BUTTON3_X + BUTTON_WIDTH, BUTTON_Y2};
        constexpr Rect<int> BTN4_RECT = {BUTTON4_X, BUTTON_Y1, BUTTON4_X + BUTTON_WIDTH, BUTTON_Y2};
    }

    class MenuScene : public IScene
    {
    public:
        MenuScene(IInputService& input, ISceneService& scene, IAssetService& asset, IRenderService& render)
            : inputService(input), sceneService(scene), assetService(asset), renderService(render) {}

        void Start() override
        {
            inputService.PushContext(InputContext::Menu);
        }

        void Update(float deltaTime) override
        {
            HandleInput();

            currentPage = std::clamp(currentPage, 0, 1);

            DrawBox();
            DrawContent();
            DrawButtons();
            DrawNavigationHints();
        }

    private:
        void HandleInput()
        {
            if (HandleOtherInput()) return;
            HandleMouseInput();
        }

        /// @brief Handle Keyboard Input and Gamepad Input
        bool HandleOtherInput()
        {
            if (inputService.IsPressed(InputAction::ToggleMenu))
            {
                inputService.PopContext();
                sceneService.PopScene();
                return true;
            }

            if (inputService.IsPressed(InputAction::Confirm)) NextPage();
            return false;
        }

        void HandleMouseInput()
        {
            auto menuClick = inputService.OnMouseClick(InputAction::MouseClick);
            if (menuClick.x == -1 || menuClick.y == -1) return;

            if (menuClick.In(BOX_RECT))
            {
                NextPage();
                return;
            }
            else if (menuClick.In(BTN0_RECT))
            {
                inputService.PopContext();
                sceneService.PopScene();
                return;
            }
            else if (menuClick.In(BTN1_RECT))
            {
                // 音量設定
                return;
            }
            else if (menuClick.In(BTN2_RECT))
            {
                // セーブ
                return;
            }
            else if (menuClick.In(BTN3_RECT))
            {
                // ロード
                return;
            }
            else if (menuClick.In(BTN4_RECT))
            {
                inputService.PopContext();
                sceneService.PopScene();
                return;
            }
        }

        void DrawBox() const
        {
            constexpr int boxX1 = BOX_MARGIN_X;
            constexpr int boxY1 = BOX_MARGIN_Y_UP;
            constexpr int boxX2 = WINDOW_WIDTH - BOX_MARGIN_X;
            constexpr int boxY2 = WINDOW_HEIGHT - BOX_MARGIN_Y_DOWN;

            renderService.DrawBoxAA(static_cast<float>(boxX1), static_cast<float>(boxY1),
                                    static_cast<float>(boxX2), static_cast<float>(boxY2), COLOR_BOX_BG, true);
            renderService.DrawHollowBox(boxX1, boxY1, boxX2, boxY2, 3, COLOR_WHITE);
            renderService.DrawString(boxX1 + TEXT_START_OFFSET_X, boxY1 + TEXT_START_OFFSET_Y,
                                     L"【メニュー】", COLOR_WHITE);
        }

        void DrawButtons() const
        {
            renderService.DrawButton(BTN0_RECT, L"戻る");
            renderService.DrawButton(BTN1_RECT, L"音量設定");
            renderService.DrawButton(BTN2_RECT, L"セーブ");
            renderService.DrawButton(BTN3_RECT, L"ロード");
            renderService.DrawButton(BTN4_RECT, L"終了");
        }

        void DrawContent() const
        {
            constexpr int boxX1 = BOX_MARGIN_X;
            constexpr int boxY1 = BOX_MARGIN_Y_UP;
            int textY = boxY1 + CONTENT_START_OFFSET_Y;

            if (currentPage == 0)
                DrawPage0(boxX1, textY);
            else if (currentPage == 1)
                DrawPage1(boxX1, textY);
        }

        void DrawPage0(int boxX1, int textY) const
        {
            renderService.DrawString(boxX1 + INDENT_LEVEL_1, textY,
                                     L"プレイヤーの魔法スキル：", COLOR_TEXT_NORMAL);
            renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING,
                                     L"【透視】1ゲームに1回のみ。敵の出す手を完全に可視化する。", COLOR_TEXT_BLUE);
            renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING * 2,
                                     L"【⚔UP】制限なし。そのターンの間、自分の攻撃力が+2される。", COLOR_TEXT_BLUE);
            renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + LINE_SPACING * 3,
                                     L"【回復】1ゲームに3回まで。自分のHPを2回復する。", COLOR_TEXT_BLUE);

            renderService.DrawString(boxX1 + INDENT_LEVEL_1, textY + SECTION_SPACING,
                                     L"表示される記号の意味：", COLOR_TEXT_NORMAL);
            renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING,
                                     L"⚖：敵がその手を出す確率の高さ。大きいほど出しやすい。", COLOR_TEXT_RED);
            renderService.DrawString(boxX1 + INDENT_LEVEL_2, textY + SECTION_SPACING + LINE_SPACING * 2,
                                     L"⚔：その手で勝った時に相手に与えるダメージ量。", COLOR_TEXT_RED);
        }

        void DrawPage1(int boxX1, int textY) const
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

        void DrawNavigationHints() const
        {
            int iconHandle;
            switch (inputService.GetActiveDevice())
            {
            case InputDevice::Keyboard:
                iconHandle = assetService.GetImageHandle(EImage::KB_SPACE);
                break;
            case InputDevice::Mouse:
                iconHandle = assetService.GetImageHandle(EImage::MOUSE_LEFT);
                break;
            case InputDevice::Gamepad:
                iconHandle = assetService.GetImageHandle(EImage::XBOX_A);
                break;
            default:
                assert(false && "未知のデバイス");
                iconHandle = assetService.GetImageHandle(EImage::MOUSE_LEFT);
            }
            renderService.DrawRotaGraphF(NEXT_PAGE_ICON_X, NEXT_PAGE_Y, 0.5, 0.0, iconHandle, true);
            renderService.DrawCenterString(WINDOW_WIDTH / 2, NEXT_PAGE_Y,
                                           L"でページ切替", COLOR_TEXT_NORMAL);
        }

        void NextPage()
        {
            currentPage++;
            if (currentPage < 0 || currentPage > 1)
                currentPage = 0;
        }

        IInputService& inputService;
        ISceneService& sceneService;
        IAssetService& assetService;
        IRenderService& renderService;

        int currentPage = 0;
    };

    std::unique_ptr<IScene> CreateMenuScene(IInputService& inputService, ISceneService& sceneService,
                                            IAssetService& assetService, IRenderService& renderService)
    {
        return std::make_unique<MenuScene>(inputService, sceneService, assetService, renderService);
    }
}
