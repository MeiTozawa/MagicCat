module;
#include "tweeny.h"

module EffectorFactory;
import RenderService;
namespace mc {
    /// durationMs が 0 の場合はそのステップを追加せず tween をそのまま返す（値渡しで連鎖呼び出しが可能）
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

    class FadeEffector : public Effector
    {
        tweeny::tween<int> alphaTween;
        int currentAlpha = 0;
        int fadeInTime, holdTime, fadeOutTime;
        IRenderService& renderService;

    public:
        FadeEffector(IRenderService& rs, int fadeInTime, int holdTime, int fadeOutTime)
            : fadeInTime(fadeInTime), holdTime(holdTime), fadeOutTime(fadeOutTime),
              renderService(rs)
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
                return;
            if (currentAlpha < 255)
                renderService.SetDrawBlendMode(BlendMode::Alpha, currentAlpha);
        }

        void AfterDraw() const override
        {
            if (currentAlpha <= 0) return;
            renderService.SetDrawBlendMode(BlendMode::NoBlend, 0);
        }

        bool ShouldDraw() const override { return currentAlpha > 0; }
    };

    std::unique_ptr<Effector> CreateFadeEffector(
        IRenderService& renderService, int fadeInTime, int holdTime, int fadeOutTime
    )
    {
        return std::make_unique<FadeEffector>(renderService, fadeInTime, holdTime, fadeOutTime);
    }

    std::unique_ptr<Effector> CreateFadeOutEffector(
        IRenderService& renderService, int durationMs
    )
    {
        return CreateFadeEffector(renderService, durationMs, 0, 0);
    }

    std::unique_ptr<Effector> CreateFadeInEffector(
        IRenderService& renderService, int durationMs
    )
    {
        return CreateFadeEffector(renderService, 0, 0, durationMs);
    }
}
