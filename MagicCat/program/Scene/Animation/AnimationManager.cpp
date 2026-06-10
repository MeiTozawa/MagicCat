module;

module AnimationManager;

namespace mc
{
    class AnimationManager : public IAnimationManager
    {
        std::vector<std::unique_ptr<IAnimationPlayer>> animationPlayers = {};

    public:
        void Update(float deltaTime) override
        {
            for (auto& player : animationPlayers)
            {
                player->Update(deltaTime);
            }
        }

        void RegisterAnimation(std::unique_ptr<IAnimationPlayer>&& scene) override
        {
            animationPlayers.push_back(std::move(scene));
        }

        void ClearAnimations() override
        {
            animationPlayers.clear();
        }
    };

    std::unique_ptr<IAnimationManager> CreateAnimationManager()
    {
        return std::make_unique<AnimationManager>();
    }
}
