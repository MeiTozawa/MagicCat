module;

export module AnimationFactory;
import Character;

namespace mc
{
    export class IAnimationPlayer
    {
    public:
        virtual ~IAnimationPlayer() = default;
        virtual void Update(float deltaTime) = 0;
        virtual bool IsPlaying() = 0;
    };
    
    export std::unique_ptr<IAnimationPlayer> CreateSpriteAnimation(int handle, int x, int y, float extraRate = 1.f, bool isFlip = false);
    export std::unique_ptr<IAnimationPlayer> CreateAttackAnimation(float x, float y , float scale, EAttackType type);
    // export std::unique_ptr<IAnimationPlayer> CreateTextAnimation();
}
