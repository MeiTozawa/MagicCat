module;

export module FadeAnimation;
import AnimationFactory;
namespace mc
{
    export enum class FadeState
    {
        Inactive,
        FadingIn,
        Holding,
        FadingOut
    };

    export class FadeAnimation : public IAnimationPlayer
    {
    private:
        Shared<dxe::Sprite> sprite;

        FadeState state = FadeState::Inactive;
        float alpha; // !!ここでのalphaの範囲は0から1であり、0から255ではない!!
        float timer;
        float scale;
        float x, y;

        float fadeInTime;
        float holdTime;
        float fadeOutTime;

    public:
        FadeAnimation(const Shared<dxe::Sprite>& sprite, float scale, float x, float y,
                      float fadeInTime, float holdTime, float fadeOutTime) :
            sprite(sprite), scale(scale), x(x), y(y),
            fadeInTime(fadeInTime),
            holdTime(holdTime),
            fadeOutTime(fadeOutTime)
        {
            alpha = 0.0f;
            timer = 0.0f;
            state = FadeState::FadingIn;
            if (sprite) {
                sprite->setBlendMode(dxe::Sprite::eBlendMode::Alpha);
            }
        }

        void Update(float deltaTime) override
        {
            switch (state)
            {
            case FadeState::Inactive:
                return;

            case FadeState::FadingIn:
                alpha += (1.0f / fadeInTime) * deltaTime;
                if (alpha >= 1.0f)
                {
                    alpha = 1.0f;
                    timer = 0.0f;
                    state = FadeState::Holding;
                }
                break;

            case FadeState::Holding:
                timer += deltaTime;
                if (timer >= holdTime)
                {
                    state = FadeState::FadingOut;
                }
                break;

            case FadeState::FadingOut:
                alpha -= (1.0f / fadeOutTime) * deltaTime;
                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    state = FadeState::Inactive;
                }
                break;
            }

            sprite->setScaleXY({scale, scale});
            sprite->setPosition({x, y});
            sprite->setAlpha(alpha);
            sprite->draw();
        }

        bool IsPlaying() override { return state != FadeState::Inactive; }
    };
}
