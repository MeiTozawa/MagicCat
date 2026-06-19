module;

#include <dxe.h>
#include <memory>
#include <RenderUtils.h>

module SceneService;
import SceneService;
import InputService;
import RenderService;
import EventBus;
import HealthComponent;
import Character;

namespace mc
{
    class InfoScene : public IScene
    {
        IInputService* inputService = nullptr;
        ISceneService* sceneService = nullptr;
        IRenderService* renderService = nullptr;
        EventHandle deathEvent;
        std::wstring info = {};
        uint32_t infoColor;
        int winCount = 0;
        int failCount = 0;
        float blinkTimer = 0;
    
    public:
        InfoScene(IInputService* input, ISceneService* scene, IRenderService* render)
            : inputService(input), sceneService(scene), renderService(render)
        {
            info = L"MagicCat";
            infoColor = 0xF259FF;
            deathEvent = EventBus::Subscribe<DeathEvent>([this](const DeathEvent& e)
            {
                auto tags = e.Victim->GetTags();
                if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                {
                    winCount++;
                    info = L"勝利";
                    infoColor = 0xFFD700;
                }
                else if (std::ranges::find(tags, ETag::Player) != tags.end())
                {
                    failCount++;
                    info = L"失敗";
                    infoColor = 0xff3333;
                }
            });
        }

        void Start() override {}

        void Update(float deltaTime) override
        {
            blinkTimer += deltaTime * 60;
            if (inputService->IsPressed(InputAction::IgConfirm))
            {
                sceneService->PushScene(Combat);
            }
            if (!info.empty())
            {
                SetFontSize(320);
                DrawCenterString(renderService, dxe::GetWindowWidthF(.5f), dxe::GetWindowHeightF(.4f),
                                 info, infoColor);
                SetFontSize(48);
            }

            DrawLeftString(renderService, 20, 20,
                       std::format(L" 勝利回数: {} ", winCount).c_str(), COLOR_WHITE);
            DrawRightString(renderService, dxe::GetWindowWidthF(1.f), 20,
                            std::format(L" 失敗回数: {} ", failCount).c_str(), COLOR_WHITE);
            DrawCenterString(renderService, dxe::GetWindowWidthF(.5f), dxe::GetWindowHeightF(.8f),
                             L"Enterキーを押してゲームをスタートする！", COLOR_WHITE);
        }
    };

    std::unique_ptr<IScene> CreateInfoScene(IInputService* inputService, ISceneService* sceneService, IRenderService* renderService)
    {
        return std::make_unique<InfoScene>(inputService, sceneService, renderService);
    }
} // namespace mc
