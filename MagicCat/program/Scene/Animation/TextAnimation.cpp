module;

module AnimationFactory;
import AssetService;
import ServiceLocator;

namespace mc
{
    class TextAnimation : public AnimationPlayer
    {
        std::wstring text;
        int fontHandle;

        FadeState state = FadeState::Inactive;
        float alpha = 0.f; // !!ここでのalphaの範囲は0から1であり、0から255ではない!!
        float timer = 0.f;

        float scale, x, y;
        float fadeInTime, holdTime, fadeOutTime;
        uint32_t color;

        TextAnimation(std::wstring&& text, float scale, float x, float y,
                      float fadeInTime, float holdTime, float fadeOutTime,
                      int fontHandle, uint32_t color = 0xFFFFFF) :
            text(text), fontHandle(fontHandle), scale(scale), x(x),
            y(y), fadeInTime(fadeInTime),
            holdTime(holdTime), fadeOutTime(fadeOutTime), color(color) {}

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
            if (!text.empty())
            {
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
                DrawString(x, y, text.c_str(), color);
                SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
            }
        }
    };
} // namespace mc
