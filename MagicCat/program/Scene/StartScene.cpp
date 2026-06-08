module;

#include <dxe.h>
#include <memory>

module SceneService;

import GameService;
import ServiceLocator;
class StartScene : public IScene {
public:
    void Update(float deltaTime) override {
        DrawString(300, 300, L"Press SPACE to Start", 0xFFFFFF, TRUE);
    }

    void Start() override
    {
        
    }
};

static struct RegisterStartScene {
    RegisterStartScene() {
        SceneRegistry::GetRegistrations().push_back([]() {
            if(auto uiService = ServiceLocator::Get<ISceneService>()) {
                uiService->RegisterScene(START, std::make_shared<StartScene>());
            }
        });
    }
} autoRegister_StartScene;