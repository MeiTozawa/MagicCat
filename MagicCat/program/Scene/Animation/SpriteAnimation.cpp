module;

module AnimationFactory;

import ServiceLocator;
import EventBus;
import CharacterService;
import HealthComponent;

namespace mc
{
    constexpr int SPRITE_SIZE = 256;


    constexpr int ANIMATION_SPEED = 10;
    constexpr int FRAME_COUNT = 4;
    constexpr int FRAME_SIZE = 32;


    class SpriteAnimation : public IAnimationPlayer
    {
        float timer = 0;
        int frame_index = 0;

        int handle;
        int x;
        int y;
        float extraRate;
        bool isFlip;

    public:
        SpriteAnimation(int handle, int x, int y, float extraRate = 1.f, bool isFlip = false) :
            handle(handle), x(x), y(y), extraRate(extraRate), isFlip(isFlip){}
        
        void Update(float deltaTime) override
        {
            timer += deltaTime;
            if (timer >= 1.f / ANIMATION_SPEED)
            {
                frame_index += 1;
                if (frame_index >= FRAME_COUNT)
                    frame_index = 0;
                timer = 0;
            }

            DrawRectRotaGraph(
                x + FRAME_SIZE / 2,
                y + FRAME_SIZE / 2,
                frame_index * FRAME_SIZE,
                0,
                FRAME_SIZE,
                FRAME_SIZE,
                extraRate,
                0.,
                handle,
                TRUE,
                isFlip
            );
        }
    };

    std::unique_ptr<IAnimationPlayer> CreateSpriteAnimation(int handle, int x, int y, float extraRate, bool isFlip)
    {
        return std::make_unique<SpriteAnimation>(handle, x, y, extraRate , isFlip);
    }
} // namespace mc
