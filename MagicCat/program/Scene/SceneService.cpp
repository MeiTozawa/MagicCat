module;

#include <unordered_map>
#include <memory>

module SceneService;

import GameService;
import ServiceLocator;

class SceneService : public ISceneService {
private:
    std::unordered_map<EGameState, std::shared_ptr<IScene>> scenes;
    std::shared_ptr<IScene> currentScene = nullptr;
    bool initialized = false;

    void EnsureInitialized() {
        if (!initialized) {
            initialized = true;
            for (auto& reg : SceneRegistry::GetRegistrations()) {
                reg();
            }
        }
    }

public:
    
    void RegisterScene(EGameState type, std::shared_ptr<IScene> scene) override {
        scenes[type] = scene;
        if (type == START)
        {
            currentScene = scene;
        }
    }

    void ChangeSceneTo(EGameState type) override {
        EnsureInitialized();
        if (scenes.contains(type)) {
            currentScene = scenes[type];
            currentScene->Start();
        }
    }

    void Update(float deltaTime) override {
        EnsureInitialized();
        if (currentScene) {
            currentScene->Update(deltaTime);
        }
    }
};

static struct RegisterSceneManager {
    RegisterSceneManager() {
        ServiceLocator::RegisterSingleton<ISceneService, SceneService>(std::make_shared<SceneService>());
    }
} autoRegister_SceneManager;