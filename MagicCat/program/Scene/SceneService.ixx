module;

#include <vector>
#include <memory>
#include <string>
#include <functional>

export module SceneService;

import CardService;
import ServiceLocator;
import GameService;

export struct SceneRegistry
{
    static std::vector<std::function<void()>>& GetRegistrations()
    {
        static std::vector<std::function<void()>> registrations;
        return registrations;
    }
};
export class IView
{
public:
    virtual ~IView() = default;
    virtual void Update(float delta_time) = 0;
};

export class IScene
{
public:
    virtual ~IScene() = default;
    virtual void Start() = 0;
    virtual void Update(float delta_time) = 0;
};


export class ISceneService
{
public:
    virtual ~ISceneService() = default;

    virtual void ChangeSceneTo(EGameState type) = 0;

    virtual void Update(float delta_time) = 0;

    virtual void RegisterScene(EGameState type, std::shared_ptr<IScene> scene) = 0;
};
