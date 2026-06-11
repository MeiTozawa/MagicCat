module;

export module AnimationFactory;
import Character;

namespace mc
{
    export class AnimationPlayer
    {
    private:
        bool isVisible = true;
        virtual void LogicUpdate(float deltaTime) = 0;
        virtual void Draw(float deltaTime) = 0;

    public:
        virtual ~AnimationPlayer() = default;
        virtual bool IsPlaying() { return true; };
        
        virtual void Update(float deltaTime) final
        {
            LogicUpdate(deltaTime);
            if (isVisible)
                Draw(deltaTime);
        };

        virtual void ToggleVisibility() final { isVisible = !isVisible; };
        virtual bool IsVisible() const final { return isVisible; };
    };

    export enum class FadeState
    {
        Inactive,
        FadingIn,
        Holding,
        FadingOut
    };

    export std::unique_ptr<AnimationPlayer> CreateFadeAnimation(
        const Shared<dxe::Sprite>& sprite, float scale, float x, float y,
        float fadeInTime, float holdTime, float fadeOutTime
    );
    export std::unique_ptr<AnimationPlayer> CreateSpriteAnimation(
        int handle, int x, int y, float extraRate = 1.f, bool isFlip = false
    );
    export std::unique_ptr<AnimationPlayer> CreateAttackAnimation(
        float x, float y, float scale, EAttackType type
    );
    // export std::unique_ptr<IAnimationPlayer> CreateTextAnimation();
}
