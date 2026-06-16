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
        std::vector<ESceneState> sceneStack = {};
        bool initialized = false;
        EventHandle characterDiedHandle;

        void EnsureInitialized()
        {
            if (!initialized)
            {
                initialized = true;
                for (auto& reg : SceneRegistry::GetRegistrations())
                {
                    reg();
                }
                sceneStack.push_back(Start);
                scenes[Start]->Start();
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
                        sceneStack = {Start};
                        scenes[Start]->Start();
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

        void PushScene(ESceneState type) override
        {
            EnsureInitialized();
            if (scenes.contains(type))
            {
                sceneStack.push_back(type);
                scenes[type]->Start();
            }
        }

        void PopScene() override
        {
            EnsureInitialized();
            if (sceneStack.size() > 1)
            {
                sceneStack.pop_back();
            }
        }

        void Update(float deltaTime) override
        {
            EnsureInitialized();
            if (!sceneStack.empty())
            {
                scenes[sceneStack.back()]->Update(deltaTime);
            }
        }

        ESceneState GetCurrentScene() override
        {
            return sceneStack.empty() ? Start : sceneStack.back();
        }

        void SetCurrentScene(ESceneState state) override
        {
            sceneStack.clear();
            sceneStack.push_back(state);
        }
    };

    Shared<ISceneService> CreateSceneService()
    {
        return std::make_shared<SceneService>();
    }
} // namespace mc
