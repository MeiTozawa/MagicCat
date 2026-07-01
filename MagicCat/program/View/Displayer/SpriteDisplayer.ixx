module;

#include <RenderUtils.h>

export module Displayer:Sprite;

import EventBus;
import HealthComponent;
import AssetService;
import RenderService;
import DisplayerBase;

namespace mc {
    constexpr int ANIMATION_SPEED = 6;

    class SpriteDisplayer : public Displayer
    {
        IAssetService* assetService;
        IRenderService& renderService;
        float timer = 0;
        int frame_index = 0;
        float extraRate;
        bool isFlip;

        size_t frame;
        Point<int> size;
        int handle;

    public:
        SpriteDisplayer(IAssetService* assetService, IRenderService& renderService,
                        ESprite sprite, float extraRate = 1.f, bool isFlip = false) :
            assetService(assetService), renderService(renderService),
            extraRate(extraRate), isFlip(isFlip)
        {
            handle = assetService->GetSpriteHandle(sprite);
            auto info = assetService->GetSpriteInfo(sprite);
            frame = info.frame;
            size = info.size;
        }

    private:
        void OnUpdate(float deltaTime) override
        {
            timer += deltaTime;
            if (timer >= 1.f / ANIMATION_SPEED)
            {
                frame_index += 1;
                if (frame_index >= frame)
                    frame_index = 0;
                timer = 0;
            }
        }

        void OnDraw(float deltaTime) const override
        {
            renderService.DrawRectRotaGraph(
                x + size.x / 2,
                y + size.y / 2,
                frame_index * size.x,
                0,
                size.x,
                size.y,
                extraRate,
                0.,
                handle,
                true,
                isFlip
            );
        }
    };

    export std::unique_ptr<Displayer> CreateSpriteDisplayer(
        IAssetService* assetService, IRenderService* renderService,
        ESprite sprite, float extraRate, bool isFlip = false
    )
    {
        return std::make_unique<SpriteDisplayer>(assetService, *renderService, sprite, extraRate, isFlip);
    }
} // namespace mc
