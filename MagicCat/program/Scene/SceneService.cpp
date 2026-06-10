module;

#include <unordered_map>
#include <memory>

module SceneService;

import GameService;
import ServiceLocator;
import HealthComponent;
import EventBus;
import CharacterService;

class SceneService : public ISceneService
{
    std::unordered_map<ESceneState, std::unique_ptr<IScene>> scenes;
    ESceneState currentScene = START;
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
            scenes[currentScene]->Start();
        }
    }

public:
    SceneService()
    {
        characterDiedHandle = EventBus::Subscribe<DeathEvent>([this](const DeathEvent& event) {
            if (auto characterService = ServiceLocator::Get<ICharacterService>())
            {
                if (event.Victim == &characterService->GetPlayer())
                {
                    ChangeSceneTo(START);
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
            scenes[currentScene]->Start();
        }
    }

    void Update(float deltaTime) override
    {
        EnsureInitialized();
        if (auto it = scenes.find(currentScene); it != scenes.end() && it->second)
        {
            it->second->Update(deltaTime);
        }
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

static struct RegisterSceneManager
{
    RegisterSceneManager()
    {
        ServiceLocator::RegisterSingleton<ISceneService, SceneService>(std::make_shared<SceneService>());
    }
} autoRegister_SceneManager;
