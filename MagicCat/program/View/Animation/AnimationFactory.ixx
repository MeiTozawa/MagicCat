module;

export module AnimationFactory;
import Character;
import DisplayerBase;
import AssetService;

namespace mc
{
    export class AnimationPlayer : virtual public IDisplayer
    {
        bool isVisible = true;

    protected:
        int x = 0;
        int y = 0;

    public:
        ~AnimationPlayer() override = default;
        void Update(float deltaTime) override = 0;
        void Draw(float deltaTime) const override = 0;

        /// @brief 表示するスプライトを差し替える（アニメーションを再生成せず内部データのみ更新する）
        virtual void SetSprite(ESprite sprite) {}

        virtual void SetPosition(int newX, int newY) final
        {
            x = newX;
            y = newY;
        }

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

    export std::unique_ptr<AnimationPlayer> CreateSpriteAnimation(
        IAssetService* assetService, ESprite sprite, float extraRate = 1.f, bool isFlip = false
    );
}
