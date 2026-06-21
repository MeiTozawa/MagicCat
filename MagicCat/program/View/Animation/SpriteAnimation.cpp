module;

module AnimationFactory;

import EventBus;
import CharacterService;
import HealthComponent;
import AssetService;

namespace mc
{

    constexpr int ANIMATION_SPEED = 6;

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
        }

        void Draw(float deltaTime) const override
        {
            DrawRectRotaGraph(
                x + size.x / 2,
                y + size.y / 2,
                frame_index * size.x,
                0,
                size.x,
                size.y,
                extraRate,
                0.,
                handle,
                TRUE,
                isFlip
            );
        }
    };

    std::unique_ptr<AnimationPlayer> CreateSpriteAnimation(
        IAssetService* assetService, ESprite sprite, float extraRate, bool isFlip
    )
    {
        return std::make_unique<SpriteAnimation>(assetService, sprite, extraRate, isFlip);
    }
} // namespace mc
