module;

#include <memory>

export module Displayer:Sprite;
export import :Core;
import AssetService;
import RenderService;

namespace mc {
    export std::unique_ptr<IDisplayer> CreateSpriteDisplayer(
        IAssetService* assetService,
        IRenderService* renderService,
        ESprite sprite,
        float extraRate,
        bool isFlip = false);
}
