module;

#include <dxe.h>
#include <memory>

module SceneService;
import GameService;
import ServiceLocator;
import InputService;

class StartScene : public IScene
{
    Shared<IInputService> inputService;
    Shared<ISceneService> sceneService;

public:
    StartScene() {}

    void Start() override
    {
        inputService = ServiceLocator::Get<IInputService>();
        sceneService = ServiceLocator::Get<ISceneService>();
    }

    void Update(float deltaTime) override
    {
        if (inputService->IsPressed(InputAction::IgConfirm))
        {
            sceneService->ChangeSceneTo(COMBAT);
        }

        DrawString(300, 300, L"Press SPACE to Start", 0xFFFFFF, TRUE);
    }
};

static struct RegisterStartScene
{
    RegisterStartScene()
    {
        SceneRegistry::GetRegistrations().push_back([]()
        {
            if (auto uiService = ServiceLocator::Get<ISceneService>())
            {
                uiService->RegisterScene(START, std::unique_ptr<StartScene>());
            }
        });
    }
} autoRegister_StartScene;
