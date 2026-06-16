module;

#include "tweeny.h"

module EffectorFactory;
import AnimationFactory;

namespace mc
{
    class HitFlashEffector : public EffectorPlayer
    {
        bool isPlaying = false;
        const int flashTime = 0;
        int bright = 255;
        tweeny::tween<int> colorTween = {};
        uint32_t flashColor = 0xFF0000;

    public:
        HitFlashEffector(std::unique_ptr<IDisplayer>&& displayer, const int flashTime, uint32_t color) :
            EffectorPlayer(std::move(displayer)), flashTime(flashTime), flashColor(color) {}

        void Play() override
        {
            colorTween = tweeny::from(255)
                         .to(0).during(50)
                         .to(255).during(flashTime).via(tweeny::easing::quadraticOut);

            isPlaying = true;
        }

        void Update(float deltaTime) override
        {
            displayer->Update(deltaTime);
            if (!isPlaying) return;

            int deltaMs = static_cast<int>(deltaTime * 1000.0f);
            colorTween.step(deltaMs);

            bright = colorTween.peek();

            if (colorTween.progress() >= 1.0f)
            {
                bright = 255;
                isPlaying = false;
            }
        }

        void Draw(float deltaTime) const override
        {
            int flashR = (flashColor >> 16) & 0xFF;
            int flashG = (flashColor >> 8) & 0xFF;
            int flashB = flashColor & 0xFF;

            int curR = flashR + (255 - flashR) * bright / 255;
            int curG = flashG + (255 - flashG) * bright / 255;
            int curB = flashB + (255 - flashB) * bright / 255;

            SetDrawBright(curR, curG, curB);
            displayer->Draw(deltaTime);
            SetDrawBright(255, 255, 255);
        }
    };

    std::unique_ptr<EffectorPlayer> CreateHitFlashEffector(
        std::unique_ptr<IDisplayer>&& displayer, uint32_t color, int flashTime
    )
    {
        return std::make_unique<HitFlashEffector>(std::move(displayer), flashTime, color);
    }
}
