module;

#include <vector>
#include <memory>
#include <functional>

export module SceneService;

import CardService;

import EventBus;
import InputService;
import AssetService;
import CharacterService;
import RenderService;

namespace mc
{
    export enum ESceneState
    {
        Info, Combat, Rules
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

        virtual void Update(float deltaTime) = 0;

        virtual void PushScene(ESceneState type) = 0;
        virtual void PopScene() = 0;

        virtual void RegisterScene(ESceneState type, std::unique_ptr<IScene>&& scene) = 0;

        virtual ESceneState GetCurrentScene() = 0;

        virtual void SetCurrentScene(ESceneState) = 0;
    };

    export Shared<ISceneService> CreateSceneService(ICharacterService* characterService);
    // InfoScene, CombatScene, RulesScene factories should now take their dependencies:
    export std::unique_ptr<IScene> CreateInfoScene(IInputService* inputService, ISceneService* sceneService, IRenderService* renderService);
    export std::unique_ptr<IScene> CreateCombatScene(ICharacterService* characterService, ISceneService* sceneService, IAssetService* assetService, ICardService* cardService, IInputService* inputService, IRenderService* renderService);
    export std::unique_ptr<IScene> CreateRulesScene(IInputService* inputService, ISceneService* sceneService, IAssetService* assetService, IRenderService* renderService);
} // namespace mc
