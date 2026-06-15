module;

#include <dxe.h>
#include <memory>

module SceneService;
import GameService;
import ServiceLocator;
import InputService;

namespace mc
{
    class StartScene : public IScene
    {
        IInputService* inputService = nullptr;
        ISceneService* sceneService = nullptr;

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
                sceneService->ChangeSceneTo(Combat);
            }

            DrawString(300, 300, L"Enterキーを押してゲームをスタートする！", 0xFFFFFF, TRUE);
        }
    };

    std::unique_ptr<IScene> CreateStartScene()
    {
        return std::make_unique<StartScene>();
    }
} // namespace mc
