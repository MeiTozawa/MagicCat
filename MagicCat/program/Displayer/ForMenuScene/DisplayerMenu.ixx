module;

#include <memory>

export module Displayer:Menu;
export import :Core;
import AssetService;
import RenderService;
import InputService;
import OSService;
import SceneService;
import AudioService;
import BattleService;
import PersistenceService;

namespace mc {
    export enum class EMenuPanel
    {
        Rules = 0,
        Volume = 1,
        Save = 2,
        Load = 3,
        Exit = 4
    };

    export class MenuPanel : public DelegatingDisplayer
    {
    public:
        MenuPanel();

        virtual bool IsPanelFocus() const;
    };

    export enum class SlotPanelMode { Save, Load };

    export std::unique_ptr<MenuPanel> CreateRulesPanel(
        IInputService& input,
        IRenderService& render,
        IAssetService& asset);

    export std::unique_ptr<MenuPanel> CreateVolumePanel(
        IInputService& input,
        IRenderService& render,
        IAudioService& audio,
        IOSService& os);

    export std::unique_ptr<MenuPanel> CreateSavePanel(
        IInputService& input,
        IRenderService& render,
        IBattleService& battle,
        IPersistenceService& persistence,
        ISceneService& scene,
        IOSService& os);

    export std::unique_ptr<MenuPanel> CreateLoadPanel(
        IInputService& input,
        IRenderService& render,
        IBattleService& battle,
        IPersistenceService& persistence,
        ISceneService& scene,
        IOSService& os);

    export std::unique_ptr<MenuPanel> CreateExitPanel(
        IInputService& input,
        IRenderService& render,
        IAssetService& asset);
}
