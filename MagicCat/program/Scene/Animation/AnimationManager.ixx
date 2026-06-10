module;

export module AnimationManager;

namespace mc
{
    export struct Animation
    {
        int handle;
        int x;
        int y;
        float extraRate = 1.f;
        bool isFlip = false;
    };

    export class IAnimationPlayer
    {
    public:
        virtual ~IAnimationPlayer() = default;
        virtual void Update(float deltaTime) = 0;
    };

    export class IAnimationManager
    {
    public:
        virtual ~IAnimationManager() = default;

        virtual void Update(float deltaTime) = 0;

        virtual void RegisterAnimation(std::unique_ptr<IAnimationPlayer>&& scene) = 0;

        virtual void ClearAnimations() = 0;
    };

    export std::unique_ptr<IAnimationManager> CreateAnimationManager();
    export std::unique_ptr<IAnimationPlayer> CreateSpriteAnimation();
    export std::unique_ptr<IAnimationPlayer> CreateDamageTextAnimation();
}
