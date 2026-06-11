module;

module AnimationFactory;

import AssetService;
import ServiceLocator;
import EventBus;
import CharacterService;
import HealthComponent;
import Character;


namespace mc
{
    constexpr float FADE_IN_TIME = 0.1f;
    constexpr float HOLD_TIME = 0.4f;
    constexpr float FADE_OUT_TIME = 0.1f;

    std::unique_ptr<AnimationPlayer> CreateAttackAnimation(float x, float y, float scale, EAttackType type)
    {
        auto assetService = ServiceLocator::Get<IAssetService>();
        if (auto icon = assetService->GetImage(static_cast<EImage>(type)))
        {
            return CreateFadeAnimation(icon, scale, x, y, FADE_IN_TIME, HOLD_TIME, FADE_OUT_TIME);
        }
        return nullptr;
    }
} // namespace mc
