module;

#include <dxe.h>

export module UiService;

import CardService;
import ServiceLocator;
import GameService;
export constexpr float SPRITE_OFFSET = 50;
export constexpr uint16_t COLOR_WHITE = 0xFFFFFF;
export constexpr uint16_t COLOR_BLACK = 0;
export constexpr uint16_t COLOR_ROCK = 0x555555;
export constexpr uint16_t COLOR_PAPER = 0xF5F5DC;
export constexpr uint16_t COLOR_SCISSORS = 0xB0C4DE;
export std::vector<std::wstring> textMappings{L"グー", L"チョキ", L"パー"};


export class IUiService
{
public:
    IUiService() = default;
    virtual ~IUiService() = default;
    virtual void Draw() = 0;
};

export class IUiManager
{
public:
    virtual ~IUiManager() = default;

    virtual void ChangeScene(EGameState type) = 0;

    virtual void Draw() = 0;

    virtual void RegisterScene(EGameState type, Shared<IUiService> scene) = 0;
};