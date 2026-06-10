module;

#include <unordered_map>
#include <memory>

module SceneService;

import GameService;
import ServiceLocator;
import HealthComponent;
import EventBus;
import CharacterService;

namespace mc
{
    class SceneService : public ISceneService
    {
        std::unordered_map<ESceneState, std::unique_ptr<IScene>> scenes;
        ESceneState currentScene = Start;
        bool initialized = false;
        EventHandle characterDiedHandle;
        IAnimationManager* animationManager;

        void EnsureInitialized()
        {
            if (!initialized)
            {
                initialized = true;
                animationManager = ServiceLocator::Get<IAnimationManager>();
                for (auto& reg : SceneRegistry::GetRegistrations())
                {
                    reg();
                }
                scenes[currentScene]->Start();
            }
        }

    public:
        SceneService()
        {
            characterDiedHandle = EventBus::Subscribe<DeathEvent>([this](const DeathEvent& event)
            {
                if (auto characterService = ServiceLocator::Get<ICharacterService>())
                {
                    if (event.Victim == &characterService->GetPlayer())
                    {
                        ChangeSceneTo(Start);
                        characterService->Reset();
                    }
                }
            });
        }

        ~SceneService() override
        {
            EventBus::Unsubscribe(characterDiedHandle);
        }

        void RegisterScene(ESceneState type, std::unique_ptr<IScene>&& scene) override
        {
            scenes[type] = std::move(scene);
        }

        void ChangeSceneTo(ESceneState type) override
        {
            EnsureInitialized();
            if (scenes.contains(type))
            {
                currentScene = type;
                animationManager->ClearAnimations();
                scenes[currentScene]->Start();
            }
        }

        void Update(float deltaTime) override
        {
            EnsureInitialized();
            scenes[currentScene]->Update(deltaTime);
        }

        ESceneState GetCurrentScene() override
        {
            return currentScene;
        }

        void SetCurrentScene(ESceneState e) override
        {
            currentScene = e;
        }
    };

    Shared<ISceneService> CreateSceneService()
    {
        return std::make_shared<SceneService>();
    }
} // namespace mc
