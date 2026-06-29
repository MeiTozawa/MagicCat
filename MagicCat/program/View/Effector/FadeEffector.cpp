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

    class FadeEffector : public Effector
    {
    private:
        tweeny::tween<int> alphaTween;
        int currentAlpha = 0;
        int fadeInTime, holdTime, fadeOutTime;

    public:
        FadeEffector(int fadeInTime, int holdTime, int fadeOutTime)
            : fadeInTime(fadeInTime), holdTime(holdTime), fadeOutTime(fadeOutTime)
        {
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
        }

        bool Update(float deltaTime) override
        {
            if (fadeInTime == 0 && holdTime == 0 && fadeOutTime == 0)
            {
                return false;
            }

            int dtMs = static_cast<int>(deltaTime * 1000.0f);
            alphaTween.step(dtMs);
            currentAlpha = alphaTween.peek();

            if (alphaTween.progress() >= 1.0f)
            {
                currentAlpha = alphaTween.peek();
                return false;
            }
            return true;
        }

        void BeforeDraw() const override
        {
            if (currentAlpha <= 0)
                return;  // 完全透明 — 描画不要
            if (currentAlpha < 255)
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, currentAlpha);
        }

        void AfterDraw() const override
        {
            if (currentAlpha <= 0) return;
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

        bool ShouldDraw() const override { return currentAlpha > 0; }
    };

    std::unique_ptr<Effector> CreateFadeEffector(
        int fadeInTime, int holdTime, int fadeOutTime
    )
    {
        return std::make_unique<FadeEffector>(fadeInTime, holdTime, fadeOutTime);
    }

    std::unique_ptr<Effector> CreateFadeOutEffector(
        int durationMs
    )
    {
        return CreateFadeEffector(durationMs, 0, 0);
    }

    std::unique_ptr<Effector> CreateFadeInEffector(
        int durationMs
    )
    {
        return CreateFadeEffector(0, 0, durationMs);
    }
}
