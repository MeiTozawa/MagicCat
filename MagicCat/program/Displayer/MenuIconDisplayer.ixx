module;

#include <memory>
#include <RenderUtils.h>

export module Displayer:MenuIcon;

import DisplayerBase;
import SceneService;
import InputService;
import RenderService;
import AssetService;
import OSService;

namespace mc {

    /// @brief メニューアイコンの描画・マウス判定・シーン操作を担うディスプレイヤー。
    /// SceneService から分離された HandleMenuButton ロジックをカプセル化する。
    export class MenuIconDisplayer : public Displayer
    {
    public:
        MenuIconDisplayer(ISceneService& sceneService, IInputService& inputService,
                          IRenderService& renderService, IAssetService& assetService,
                          IOSService& osService)
            : sceneService(sceneService), inputService(inputService),
              renderService(renderService), assetService(assetService),
              osService(osService)
        {}

    private:
        void OnUpdate(float deltaTime) override
        {
            if (sceneService.GetCurrentScene() == ESceneState::Cutscene)
                return;

            const bool isMenuOpen = (sceneService.GetCurrentScene() == ESceneState::Menu);

            if (inputService.GetActiveDevice() == InputDevice::Mouse
                && inputService.IsMouseOver(MENU_ICON_RECT))
                osService.SetCursorPointer();

            const auto click = inputService.OnMouseClick(InputAction::MouseClick);
            const bool iconClicked = click.x != -1 && click.y != -1
                                     && click.In(MENU_ICON_RECT.Expand(5));

            bool menuJustClosed = false;
            if (!isMenuOpen && wasMenuOpenLastFrame)
            {
                menuJustClosed = true;
            }
            wasMenuOpenLastFrame = isMenuOpen;

            if (isMenuOpen)
            {
                if (iconClicked)
                {
                    inputService.PopContext();
                    sceneService.PopScene();
                }
            }
            else
            {
                if (menuJustClosed)
                {
                    return;
                }

                if (inputService.IsPressed(InputAction::ToggleMenu) || iconClicked)
                {
                    sceneService.PushScene(ESceneState::Menu);
                }
            }
        }

        void OnDraw(float deltaTime) const override
        {
            if (sceneService.GetCurrentScene() == ESceneState::Cutscene)
                return;

            const int iconHandle = assetService.GetImageHandle(EImage::BUTTON_MENU);
            if (iconHandle >= 0)
                renderService.DrawRotaGraphF(
                    static_cast<float>(MENU_ICON_X),
                    static_cast<float>(MENU_ICON_Y),
                    0.8f, 0.0f, iconHandle, true);
        }

        ISceneService&  sceneService;
        IInputService&  inputService;
        IRenderService& renderService;
        IAssetService&  assetService;
        IOSService&     osService;

        bool wasMenuOpenLastFrame = false;
    };

    /// @brief MenuIconDisplayer を生成するファクトリー関数
    export std::unique_ptr<MenuIconDisplayer> CreateMenuIconDisplayer(
        ISceneService& sceneService, IInputService& inputService,
        IRenderService& renderService, IAssetService& assetService,
        IOSService& osService)
    {
        return std::make_unique<MenuIconDisplayer>(
            sceneService, inputService, renderService, assetService, osService);
    }

} // namespace mc
