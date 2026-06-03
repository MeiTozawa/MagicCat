module;

#include <vector>
#include <memory>
#include <string>
#include <functional>

export module UiService;

import CardService;
import ServiceLocator;
import GameService;

export struct UiRegistry {
    static std::vector<std::function<void()>>& GetRegistrations() {
        static std::vector<std::function<void()>> registrations;
        return registrations;
    }
};

export constexpr float SPRITE_OFFSET = 50;
export constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
export constexpr uint32_t COLOR_BLACK = 0;
export constexpr uint32_t COLOR_ROCK = 0x555555;
export constexpr uint32_t COLOR_PAPER = 0xF5F5DC;
export constexpr uint32_t COLOR_SCISSORS = 0xB0C4DE;
export std::vector<std::wstring> textMappings{L"グー", L"チョキ", L"パー"};

export class IUi
{
public:
    IUi() = default;
    virtual ~IUi() = default;
    virtual void Draw() = 0;
};

export class IUiService
{
public:
    virtual ~IUiService() = default;

    virtual void ChangeScene(EGameState type) = 0;

    virtual void Draw() = 0;

    virtual void RegisterScene(EGameState type, std::shared_ptr<IUi> scene) = 0;
};
