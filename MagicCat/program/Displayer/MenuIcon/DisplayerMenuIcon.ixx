module;

#include <memory>

export module Displayer:MenuIcon;
export import :Core;
import AssetService;
import RenderService;
import InputService;
import OSService;
import SceneService;

namespace mc {
    export std::unique_ptr<IDisplayer> CreateMenuIconDisplayer(
        ISceneService& sceneService,
        IInputService& inputService,
        IRenderService& renderService,
        IAssetService& assetService,
        IOSService& osService);
}
