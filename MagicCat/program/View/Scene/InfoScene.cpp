module;

#include <dxe.h>
#include <memory>

module SceneService;
import GameService;
import ServiceLocator;
import InputService;
import EventBus;
import HealthComponent;
import Character;

namespace mc
{
    class InfoScene : public IScene
    {
        IInputService* inputService = nullptr;
        ISceneService* sceneService = nullptr;
        EventHandle deathEvent;
        std::wstring info = {};
        uint32_t infoColor;
        int winCount = 0;
        int failCount = 0;

        static void DrawCenterString(int x, int y, const std::wstring& text, uint32_t color)
        {
            int textWidth = GetDrawStringWidth(text.c_str(), text.size());

            int textHeight = GetFontSize();

            int drawX = x - (textWidth / 2);
            int drawY = y - (textHeight / 2);

            DrawString(drawX, drawY, text.c_str(), color);
        }

        static void DrawRightString(int x, int y, const std::wstring& text, uint32_t color)
        {
            int textWidth = GetDrawStringWidth(text.c_str(), text.size());

            int drawX = x - textWidth;
            int drawY = y;

            DrawString(drawX, drawY, text.c_str(), color);
        }

    public:
        InfoScene()
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

        void Start() override
        {
            inputService = ServiceLocator::Get<IInputService>();
            sceneService = ServiceLocator::Get<ISceneService>();
        }

        void Update(float deltaTime) override
        {
            if (inputService->IsPressed(InputAction::IgConfirm))
            {
                sceneService->PushScene(Combat);
            }
            if (!info.empty())
            {
                SetFontSize(320);
                DrawCenterString(dxe::GetWindowWidthF(.5f), dxe::GetWindowHeightF(.4f),
                                 info, infoColor);
                SetFontSize(48);
            }

            DrawString(0, 20,
                       std::format(L" 勝利回数: {} ", winCount).c_str(), 0xFFFFFF);
            DrawRightString(dxe::GetWindowWidthF(1.f), 20,
                            std::format(L" 失敗回数: {} ", failCount).c_str(), 0xFFFFFF);
            DrawCenterString(dxe::GetWindowWidthF(.5f), dxe::GetWindowHeightF(.8f),
                             L"Enterキーを押してゲームをスタートする！", 0xFFFFFF);
        }
    };

    std::unique_ptr<IScene> CreateInfoScene()
    {
        return std::make_unique<InfoScene>();
    }
} // namespace mc
