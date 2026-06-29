module;

#include "tweeny.h"

module EffectorFactory;
import RenderService;

namespace mc {
    class HitFlashEffector : public Effector
    {
        const int flashTime = 0;
        int bright = 255;
        tweeny::tween<int> colorTween = {};
        uint32_t flashColor = 0xFF0000;
        IRenderService& renderService;

    public:
        HitFlashEffector(IRenderService& rs, const int flashTime, uint32_t color) :
            flashTime(flashTime), flashColor(color), renderService(rs)
        {
            colorTween = tweeny::from(255)
                         .to(0).during(50)
                         .to(255).during(flashTime).via(tweeny::easing::quadraticOut);
        }

        bool Update(float deltaTime) override
        {
            int deltaMs = static_cast<int>(deltaTime * 1000.0f);
            colorTween.step(deltaMs);

            bright = colorTween.peek();

            if (colorTween.progress() >= 1.0f)
            {
                bright = 255;
                return false;
            }
            return true;
        }

        void BeforeDraw() const override
        {
            int flashR = (flashColor >> 16) & 0xFF;
            int flashG = (flashColor >> 8) & 0xFF;
            int flashB = flashColor & 0xFF;

            int curR = flashR + (255 - flashR) * bright / 255;
            int curG = flashG + (255 - flashG) * bright / 255;
            int curB = flashB + (255 - flashB) * bright / 255;

            renderService.SetDrawBright(curR, curG, curB);
        }

        void AfterDraw() const override
        {
            renderService.SetDrawBright(255, 255, 255);
        }
    };

    std::unique_ptr<Effector> CreateHitFlashEffector(
        IRenderService& renderService, uint32_t color, int flashTime
    )
    {
        return std::make_unique<HitFlashEffector>(renderService, flashTime, color);
    }
}
