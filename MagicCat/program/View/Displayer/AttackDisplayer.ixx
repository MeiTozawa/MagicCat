module;

#include <memory>
export module Displayer:Attack;
import DisplayerBase;


namespace mc
{
    export class AttackDisplayer : public IDisplayer
    {
        float x, y, scale;
        int handle = -1;

    public:
        AttackDisplayer(const float x, const float y, const float scale)
            : x(x), y(y), scale(scale) {}

        void SetImage(int imageHandle)
        {
            handle = imageHandle;
        }

        void Update(float deltaTime) override {}

        void Draw(float deltaTime) const override
        {
            if (handle != -1)
            {
                DrawRotaGraphF(x, y, scale, 0.0, handle, TRUE);
            }
        }
    };

    export std::unique_ptr<AttackDisplayer> CreateAttackDisplayer(float x, float y, float scale)
    {
        return std::make_unique<AttackDisplayer>(x, y, scale);
    }
}
