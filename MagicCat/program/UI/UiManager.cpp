module;

#include <unordered_map>
#include <memory>

module UiService;

class UiManager : public IUiManager {
private:
    std::unordered_map<EGameState, std::shared_ptr<IUiService>> _scenes;
    std::shared_ptr<IUiService> _currentScene = nullptr;

public:
    void RegisterScene(EGameState type, std::shared_ptr<IUiService> scene) override {
        _scenes[type] = scene;
        if (_currentScene == nullptr) {
            _currentScene = scene;
        }
    }

    void ChangeScene(EGameState type) override {
        if (_scenes.find(type) != _scenes.end()) {
            _currentScene = _scenes[type];
        }
    }

    void Draw() override {
        if (_currentScene) {
            _currentScene->Draw();
        }
    }
};

static struct RegisterUiManager {
    RegisterUiManager() {
        ServiceLocator::RegisterSingleton<IUiManager, UiManager>(std::make_shared<UiManager>());
    }
} autoRegister_UiManager;