module;

#include <unordered_map>
#include <memory>

module UiService;

import GameService;
import ServiceLocator;

class UiService : public IUiService {
private:
    std::unordered_map<EGameState, std::shared_ptr<IUi>> scenes;
    std::shared_ptr<IUi> currentScene = nullptr;
    bool initialized = false;

    void EnsureInitialized() {
        if (!initialized) {
            initialized = true;
            for (auto& reg : UiRegistry::GetRegistrations()) {
                reg();
            }
        }
    }

public:
    
    void RegisterScene(EGameState type, std::shared_ptr<IUi> scene) override {
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
        }
    }

    void Draw() override {
        EnsureInitialized();
        if (currentScene) {
            currentScene->Draw();
        }
    }
};

static struct RegisterUiManager {
    RegisterUiManager() {
        ServiceLocator::RegisterSingleton<IUiService, UiService>(std::make_shared<UiService>());
    }
} autoRegister_UiManager;