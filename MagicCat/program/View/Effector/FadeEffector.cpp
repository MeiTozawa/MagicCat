module;
#include "tweeny.h"

module EffectorFactory;
namespace mc
{
    class FadeEffector : public EffectorPlayer
    {
    private:
        tweeny::tween<int> alphaTween;
        int currentAlpha = 0;
        int fadeInTime, holdTime, fadeOutTime;

    public:
        FadeEffector(std::unique_ptr<IDisplayer>&& displayer, int fadeInTime, int holdTime, int fadeOutTime)
            : EffectorPlayer(std::move(displayer)),
              fadeInTime(fadeInTime), holdTime(holdTime), fadeOutTime(fadeOutTime) {}

        void Play() override
        {
            alphaTween = tweeny::from(0)
                         .to(255).during(fadeInTime).via(tweeny::easing::sinusoidalOut)
                         .to(255).during(holdTime)
                         .to(0).during(fadeOutTime).via(tweeny::easing::sinusoidalOut);

            isPlaying = true;
        }


        void Update(float deltaTime) override
        {
            displayer->Update(deltaTime);
            if (!isPlaying) return;

            int dtMs = static_cast<int>(deltaTime * 1000.0f);
            alphaTween.step(dtMs);

            currentAlpha = alphaTween.peek();

            if (alphaTween.progress() >= 1.0f)
            {
                isPlaying = false;
            }
        }

        void Draw(float deltaTime) const override
        {
            if (currentAlpha <= 0)
                return;

            if (currentAlpha < 255)
            {
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, currentAlpha);
                displayer->Draw(deltaTime);
                SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            }
            else
            {
                displayer->Draw(deltaTime);
            }
        }
    };
    
    std::unique_ptr<EffectorPlayer> CreateFadeEffector(
        std::unique_ptr<IDisplayer>&& displayer, int fadeInTime, int holdTime, int fadeOutTime
    )
    {
        return std::make_unique<FadeEffector>(std::move(displayer), fadeInTime, holdTime, fadeOutTime);
    }
}
