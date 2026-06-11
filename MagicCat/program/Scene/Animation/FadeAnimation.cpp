module;

module AnimationFactory;
import AssetService;
import ServiceLocator;

namespace mc
{
    class FadeAnimation : public AnimationPlayer
    {
    private:
        Shared<dxe::Sprite> sprite = nullptr;
        FadeState state = FadeState::Inactive;
        float alpha = 0.f; // !!ここでのalphaの範囲は0から1であり、0から255ではない!!
        float timer = 0.f;
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
            if (sprite)
            {
                sprite->setBlendMode(dxe::Sprite::eBlendMode::Alpha);
            }
        }

        bool IsPlaying() override { return state != FadeState::Inactive; }

    private:
        void LogicUpdate(float deltaTime) override
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
        }

        void Draw(float deltaTime) override
        {
            if (sprite != nullptr)
            {
                sprite->setScaleXY({scale, scale});
                sprite->setPosition({x, y});
                sprite->setAlpha(alpha);
                sprite->draw();
            }
        }
    };

    std::unique_ptr<AnimationPlayer> CreateFadeAnimation(
        const Shared<dxe::Sprite>& sprite, float scale, float x, float y,
        float fadeInTime, float holdTime, float fadeOutTime)
    {
        return std::make_unique<FadeAnimation>(sprite, scale, x, y, fadeInTime, holdTime, fadeOutTime);
    }
}
