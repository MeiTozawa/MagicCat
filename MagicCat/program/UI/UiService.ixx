module;

#include <vector>
#include <memory>
#include <string>
#include <functional>

export module UiService;

import CardService;
import ServiceLocator;
import GameService;

export struct UiRegistry
{
    static std::vector<std::function<void()>>& GetRegistrations()
    {
        static std::vector<std::function<void()>> registrations;
        return registrations;
    }
};


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

    virtual void ChangeSceneTo(EGameState type) = 0;

    virtual void Draw() = 0;

    virtual void RegisterScene(EGameState type, std::shared_ptr<IUi> scene) = 0;
};
