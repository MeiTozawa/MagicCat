module;

#include <unordered_map>
#include <memory>

module SceneService;

import GameService;
import ServiceLocator;

class SceneService : public ISceneService
{
    std::unordered_map<ESceneState, std::unique_ptr<IScene>> scenes;
    ESceneState currentScene = START;
    bool initialized = false;

    void EnsureInitialized()
    {
        if (!initialized)
        {
            initialized = true;
            for (auto& reg : SceneRegistry::GetRegistrations())
            {
                reg();
            }
        }
    }

public:
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

static struct RegisterSceneManager
{
    RegisterSceneManager()
    {
        ServiceLocator::RegisterSingleton<ISceneService, SceneService>(std::make_shared<SceneService>());
    }
} autoRegister_SceneManager;
