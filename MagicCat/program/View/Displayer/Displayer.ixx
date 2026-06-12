module;

export module Displayer;

#include <memory>

namespace mc
{
    export class IDisplayer
    {
    public:
        virtual ~IDisplayer() = default;
        virtual void Update(float deltaTime) = 0;
        virtual void Draw(float deltaTime) const = 0;
    };

    export std::unique_ptr<IDisplayer> CreateAttackDisplayer(float x, float y, float scale, const int* handle);
    export std::unique_ptr<IDisplayer> CreateControlDisplayer(uint32_t color = 0xFFFFFF);
    export std::unique_ptr<IDisplayer> CreateCardDisplayer();
    export std::unique_ptr<IDisplayer> CreateCharacterDisplayer();
}
