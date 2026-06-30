module;

#include <memory>
export module Displayer:Attack;
import DisplayerBase;
import RenderService;

namespace mc {
    export class AttackDisplayer : public Displayer
    {
        float x, y, scale;
        int handle = -1;
        IRenderService& renderService;

    public:
        AttackDisplayer(IRenderService& rs, const float x, const float y, const float scale)
            : x(x), y(y), scale(scale), renderService(rs) {}

        void SetImage(int imageHandle)
        {
            handle = imageHandle;
        }

    private:
        void OnDraw(float deltaTime) const override
        {
            if (handle != -1)
            {
                renderService.DrawRotaGraphF(x, y, scale, 0.0, handle, true);
            }
        }
    };

    export std::unique_ptr<AttackDisplayer> CreateAttackDisplayer(IRenderService& renderService,
                                                                   float x, float y, float scale)
    {
        return std::make_unique<AttackDisplayer>(renderService, x, y, scale);
    }
}
