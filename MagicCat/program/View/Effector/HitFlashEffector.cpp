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

    public:
        HitFlashEffector(std::unique_ptr<IDisplayer>&& displayer, const int flashTime) :
            EffectorPlayer(std::move(displayer)), flashTime(flashTime) {}

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
            SetDrawBright(255, bright, bright);
            displayer->Draw(deltaTime);
            SetDrawBright(255, 255, 255);
        }
    };

    std::unique_ptr<EffectorPlayer> CreateHitFlashEffector(
        std::unique_ptr<IDisplayer>&& displayer, int flashTime
    )
    {
        return std::make_unique<HitFlashEffector>(std::move(displayer), flashTime);
    }
}
