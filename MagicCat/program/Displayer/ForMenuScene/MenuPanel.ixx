module;

#include <memory>
#include <RenderUtils.h>

export module Displayer:Menu;

import DisplayerBase;
import InputService;
import RenderService;
import AssetService;
import AudioService;
import BattleService;
import ConfigService;
import SceneService;
import OSService;
import ButtonGroup;

namespace mc {
    export enum class EMenuPanel
    {
        Rules = 0,
        Volume = 1,
        Save = 2,
        Load = 3,
        Exit = 4
    };

    export class MenuPanel : public Displayer
    {
    public:
        MenuPanel() { SetPosition(MENU_BOX_X1, MENU_TEXT_Y); }

        /// @brief 詳細な調整状態（値微調整など）にあるかを返す。
        virtual bool IsPanelFocus() const { return false; }
    };

    /// @brief `RulesPanel` を生成して返す。
    /// @param input   入力サービス
    /// @param render  描画サービス
    /// @param asset   アセットサービス
    /// @return 生成した `RulesPanel` の所有権
    export std::unique_ptr<MenuPanel> CreateRulesPanel(IInputService& input,
                                                       IRenderService& render,
                                                       IAssetService& asset);

    /// @brief `VolumePanel` を生成して返す。
    /// @param input   入力サービス
    /// @param render  描画サービス
    /// @param audio   音量取得・設定サービス
    /// @param os      カーソル変更サービス
    /// @return 生成した `VolumePanel` の所有権
    export std::unique_ptr<MenuPanel> CreateVolumePanel(IInputService& input,
                                                        IRenderService& render,
                                                        IAudioService& audio,
                                                        IOSService& os);

    /// @brief セーブ／ロードパネルの動作モード
    export enum class SlotPanelMode { Save, Load };

    /// @brief セーブパネルを生成して返す（`SlotPanelMode::Save` で構築）。
    /// @param input   入力サービス
    /// @param render  描画サービス
    /// @param battle  セーブ操作サービス
    /// @param config  スロットメタデータ取得サービス
    /// @param scene   シーン遷移サービス（ロード時のみ使用）
    /// @param os      カーソル変更サービス
    /// @return 生成した `SaveAndLoadPanel` の所有権
    export std::unique_ptr<MenuPanel> CreateSavePanel(IInputService& input,
                                                      IRenderService& render,
                                                      IBattleService& battle,
                                                      IConfigService& config,
                                                      ISceneService& scene,
                                                      IOSService& os);

    /// @brief ロードパネルを生成して返す（`SlotPanelMode::Load` で構築）。
    /// @param input   入力サービス
    /// @param render  描画サービス
    /// @param battle  ロード操作サービス
    /// @param config  スロットメタデータ取得サービス
    /// @param scene   シーン遷移サービス
    /// @param os      カーソル変更サービス
    /// @return 生成した `SaveAndLoadPanel` の所有権
    export std::unique_ptr<MenuPanel> CreateLoadPanel(IInputService& input,
                                                      IRenderService& render,
                                                      IBattleService& battle,
                                                      IConfigService& config,
                                                      ISceneService& scene,
                                                      IOSService& os);

    /// @brief `ExitPanel` を生成して返す。
    /// @param input   入力サービス
    /// @param render  描画サービス
    /// @return 生成した `ExitPanel` の所有権
    export std::unique_ptr<MenuPanel> CreateExitPanel(IInputService& input,
                                                      IRenderService& render);
} // namespace mc
