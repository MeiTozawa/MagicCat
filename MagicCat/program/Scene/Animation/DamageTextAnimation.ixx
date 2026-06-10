module;

module AnimationManager;

import AssetService;
import ServiceLocator;
import EventBus;
import CharacterService;
import HealthComponent;

namespace mc
{
    class DamageTextAnimation : public IAnimationPlayer {};

    std::unique_ptr<IAnimationPlayer> CreateDamageTextAnimation()
    {
        return std::make_unique<DamageTextAnimation>();
    }
} // namespace mc
