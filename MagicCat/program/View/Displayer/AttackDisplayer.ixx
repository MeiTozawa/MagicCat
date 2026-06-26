module;

#include <memory>
export module Displayer:Attack;
import DisplayerBase;


namespace mc
{
    class AttackDisplayer : public IDisplayer
    {
        float x, y, scale;
        const int* handle;

    public:
        AttackDisplayer(const float x, const float y, const float scale, const int* handle)
            : x(x), y(y), scale(scale), handle(handle) {}


        void Update(float deltaTime) override {}

        void Draw(float deltaTime) const override
        {
            if (*handle != -1)
            {
                DrawRotaGraphF(x, y, scale, 0.0, *handle, TRUE);
            }
        }
    };

    export std::unique_ptr<IDisplayer> CreateAttackDisplayer(float x, float y, float scale, const int* handle)
    {
        return std::make_unique<AttackDisplayer>(x, y, scale, handle);
    }
}
