module;

module AnimationFactory;

import AssetService;
import ServiceLocator;
import EventBus;
import CharacterService;
import HealthComponent;
import FadeAnimation;
import Character;


namespace mc
{
    constexpr float FADE_IN_TIME = 0.1f;
    constexpr float HOLD_TIME = 0.4f;
    constexpr float FADE_OUT_TIME = 0.1f;

    class AttackAnimation : public IAnimationPlayer
    {
        std::unique_ptr<FadeAnimation> fadeAnimation;
        Shared<dxe::Sprite> icon;
        float x, y, scale;

    public:
        AttackAnimation(float x, float y, float scale, EAttackType type)
            : x(x), y(y), scale(scale)
        {
            auto assetService = ServiceLocator::Get<IAssetService>();
            icon = assetService->GetImage(static_cast<EImage>(type));
            if (icon)
            {
                fadeAnimation = std::make_unique<FadeAnimation>(icon, scale, x, y, FADE_IN_TIME, HOLD_TIME, FADE_OUT_TIME);
            }
        }

        void Update(float deltaTime) override
        {
            fadeAnimation->Update(deltaTime);
        }

        bool IsPlaying() override { return fadeAnimation ? fadeAnimation->IsPlaying() : false; }
    };

    std::unique_ptr<IAnimationPlayer> CreateAttackAnimation(float x, float y, float scale, EAttackType type)
    {
        return std::make_unique<AttackAnimation>(x, y, scale, type);
    }
} // namespace mc
