module;

#include <vector>
#include <memory>
#include <string>
#include <functional>

export module SceneService;

import CardService;
import ServiceLocator;
import GameService;
import EventBus;

namespace mc
{
    export enum ESceneState
    {
        Start, Combat, Result
    };

    export struct SceneRegistry
    {
        static std::vector<std::function<void()>>& GetRegistrations()
        {
            static std::vector<std::function<void()>> registrations;
            return registrations;
        }
    };

    export struct EnterCutSceneEvent : IEvent {};

    export struct ExitCutSceneEvent : IEvent {};

    export struct ActionSelectionEvent : IEvent
    {
        ActionSelectionEvent(const int selected_index)
            : selectedIndex(selected_index) {}

        int selectedIndex;
    };

    export class IView
    {
    public:
        virtual ~IView() = default;
        virtual void Update(float deltaTime) = 0;
    };

    export class IScene
    {
    public:
        virtual ~IScene() = default;
        virtual void Start() = 0;
        virtual void Update(float deltaTime) = 0;
    };


    export class ISceneService
    {
    public:
        virtual ~ISceneService() = default;

        virtual void ChangeSceneTo(ESceneState type) = 0;

        virtual void Update(float deltaTime) = 0;

        virtual void RegisterScene(ESceneState type, std::unique_ptr<IScene>&& scene) = 0;

        virtual ESceneState GetCurrentScene() = 0;

        virtual void SetCurrentScene(ESceneState) = 0;
    };

    export Shared<ISceneService> CreateSceneService();
    export std::unique_ptr<IScene> CreateStartScene();
    export std::unique_ptr<IScene> CreateCombatScene();
} // namespace mc
