module;

module AnimationFactory;

import EventBus;
import HealthComponent;
import AssetService;

namespace mc
{

    constexpr int ANIMATION_SPEED = 6;
    constexpr float CROSSFADE_TIME = 0.4f;

    class SpriteAnimation : public AnimationPlayer
    {
        IAssetService* assetService;
        float timer = 0;
        int frame_index = 0;
        float extraRate;
        bool isFlip;

        size_t frame;
        tnl::Vector2i size;
        int handle;

        bool isTransitioning = false;
        float transitionTimer = 0;
        int prevHandle = -1;
        size_t prevFrame = 0;
        tnl::Vector2i prevSize{};
        int prevFrameIndex = 0;

        void DrawSprite(int spriteHandle, int frameIdx, const tnl::Vector2i& spriteSize, int alpha) const
        {
            if (alpha <= 0)
                return;

            const bool useBlend = alpha < 255;
            if (useBlend)
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

            DrawRectRotaGraph(
                x + spriteSize.x / 2,
                y + spriteSize.y / 2,
                frameIdx * spriteSize.x,
                0,
                spriteSize.x,
                spriteSize.y,
                extraRate,
                0.,
                spriteHandle,
                TRUE,
                isFlip
            );

            if (useBlend)
                SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

    public:
        SpriteAnimation(IAssetService* assetService, ESprite sprite, float extraRate = 1.f, bool isFlip = false) :
            assetService(assetService), extraRate(extraRate), isFlip(isFlip)
        {
            handle = assetService->GetSpriteHandle(sprite);
            auto info = assetService->GetSpriteInfo(sprite);
            frame = info.frame;
            size = info.size;
        }

        void Update(float deltaTime) override
        {
            timer += deltaTime;
            if (timer >= 1.f / ANIMATION_SPEED)
            {
                frame_index += 1;
                if (frame_index >= frame)
                    frame_index = 0;
                timer = 0;
            }

            if (isTransitioning)
            {
                transitionTimer += deltaTime;
                if (transitionTimer >= CROSSFADE_TIME)
                {
                    isTransitioning = false;
                    transitionTimer = 0;
                    prevHandle = -1;
                }
            }
        }

        void SetSprite(ESprite sprite) override
        {
            int newHandle = assetService->GetSpriteHandle(sprite);

            if (newHandle != handle)
            {
                prevHandle = handle;
                prevFrame = frame;
                prevSize = size;
                prevFrameIndex = frame_index;
                isTransitioning = true;
                transitionTimer = 0;
            }

            handle = newHandle;
            auto info = assetService->GetSpriteInfo(sprite);
            frame = info.frame;
            size = info.size;
            frame_index = 0;
            timer = 0;
        }

        void Draw(float deltaTime) const override
        {
            if (isTransitioning)
            {
                float progress = transitionTimer / CROSSFADE_TIME;
                if (progress < 0.f) progress = 0.f;
                if (progress > 1.f) progress = 1.f;

                int newAlpha = static_cast<int>(255 * progress);
                int prevAlpha = 255 - newAlpha;

                DrawSprite(prevHandle, prevFrameIndex, prevSize, prevAlpha);
                DrawSprite(handle, frame_index, size, newAlpha);
            }
            else
            {
                DrawSprite(handle, frame_index, size, 255);
            }
        }
    };

    std::unique_ptr<AnimationPlayer> CreateSpriteAnimation(
        IAssetService* assetService, ESprite sprite, float extraRate, bool isFlip
    )
    {
        return std::make_unique<SpriteAnimation>(assetService, sprite, extraRate, isFlip);
    }
} // namespace mc
