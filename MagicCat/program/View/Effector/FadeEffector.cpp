module;
#include "tweeny.h"

module EffectorFactory;
namespace mc {
    
    // -------------------------------------------------------------------------
    // tween 構築ヘルパー（値渡し・値返しで真の連鎖呼び出しが可能）
    // durationMs が 0 の場合はそのステップを追加せずそのまま返す
    // -------------------------------------------------------------------------
    static tweeny::tween<int> FadeIn(tweeny::tween<int> tween, int durationMs)
    {
        if (durationMs > 0)
            return tween.to(255).during(durationMs).via(tweeny::easing::sinusoidalOut);
        return tween;
    }

    static tweeny::tween<int> Hold(tweeny::tween<int> tween, int durationMs)
    {
        if (durationMs > 0)
            return tween.to(255).during(durationMs);
        return tween;
    }

    static tweeny::tween<int> FadeOut(tweeny::tween<int> tween, int durationMs)
    {
        if (durationMs > 0)
            return tween.to(0).during(durationMs).via(tweeny::easing::sinusoidalOut);
        return tween;
    }

    // -------------------------------------------------------------------------

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
            if (fadeInTime == 0 && holdTime == 0 && fadeOutTime == 0)
            {
                isPlaying = false;
                return;
            }

            const int startAlpha = (fadeInTime > 0) ? 0 : 255;
            currentAlpha = startAlpha;

            alphaTween =
                FadeOut(
                    Hold(
                        FadeIn(
                            tweeny::from(startAlpha),
                            fadeInTime),
                        holdTime),
                    fadeOutTime);

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
                currentAlpha = alphaTween.peek();
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

    std::unique_ptr<EffectorPlayer> CreateFadeOutEffector(
        std::unique_ptr<IDisplayer>&& displayer, int durationMs
    )
    {
        return CreateFadeEffector(std::move(displayer), durationMs, 0, 0);
    }

    std::unique_ptr<EffectorPlayer> CreateFadeInEffector(
        std::unique_ptr<IDisplayer>&& displayer, int durationMs
    )
    {
        return CreateFadeEffector(std::move(displayer), 0, 0, durationMs);
    }
}
