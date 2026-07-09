module;

#include <memory>

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

export class MenuPanel : public Displayer {
public:
    MenuPanel(int boxX1, int textY) { SetPosition(boxX1, textY); }
    virtual ~MenuPanel() = default;

    /// @brief 更新処理を行う。呼び出し元に戻りたい（パネルを閉じたい）場合は false を返す。
    virtual bool UpdatePanel(float deltaTime)
    {
        Update(deltaTime);
        return true;
    }

    /// @brief 詳細な調整状態（値微調整など）にあるかを返す。
    virtual bool IsPanelFocus() const { return false; }
};

/// @brief `RulesPanel` を生成して返す。
/// @param boxX1   メニューボックスの左端 X 座標
/// @param textY   コンテンツ描画開始 Y 座標
/// @param input   入力サービス
/// @param render  描画サービス
/// @param asset   アセットサービス
/// @return 生成した `RulesPanel` の所有権
export std::unique_ptr<MenuPanel> CreateRulesPanel(int boxX1, int textY,
                                                    IInputService& input,
                                                    IRenderService& render,
                                                    IAssetService& asset);

/// @brief `VolumePanel` を生成して返す。
/// @param boxX1   メニューボックスの左端 X 座標
/// @param textY   コンテンツ描画開始 Y 座標
/// @param input   入力サービス
/// @param render  描画サービス
/// @param audio   音量取得・設定サービス
/// @param os      カーソル変更サービス
/// @return 生成した `VolumePanel` の所有権
export std::unique_ptr<MenuPanel> CreateVolumePanel(int boxX1, int textY,
                                                     IInputService& input,
                                                     IRenderService& render,
                                                     IAudioService& audio,
                                                     IOSService& os);

/// @brief セーブ／ロードパネルの動作モード
export enum class SlotPanelMode { Save, Load };

/// @brief セーブパネルを生成して返す（`SlotPanelMode::Save` で構築）。
/// @param boxX1   メニューボックスの左端 X 座標
/// @param textY   コンテンツ描画開始 Y 座標
/// @param input   入力サービス
/// @param render  描画サービス
/// @param battle  セーブ操作サービス
/// @param config  スロットメタデータ取得サービス
/// @param scene   シーン遷移サービス（ロード時のみ使用）
/// @param os      カーソル変更サービス
/// @return 生成した `SaveAndLoadPanel` の所有権
export std::unique_ptr<MenuPanel> CreateSavePanel(int boxX1, int textY,
                                                   IInputService& input,
                                                   IRenderService& render,
                                                   IBattleService& battle,
                                                   IConfigService& config,
                                                   ISceneService& scene,
                                                   IOSService& os);

/// @brief ロードパネルを生成して返す（`SlotPanelMode::Load` で構築）。
/// @param boxX1   メニューボックスの左端 X 座標
/// @param textY   コンテンツ描画開始 Y 座標
/// @param input   入力サービス
/// @param render  描画サービス
/// @param battle  ロード操作サービス
/// @param config  スロットメタデータ取得サービス
/// @param scene   シーン遷移サービス
/// @param os      カーソル変更サービス
/// @return 生成した `SaveAndLoadPanel` の所有権
export std::unique_ptr<MenuPanel> CreateLoadPanel(int boxX1, int textY,
                                                   IInputService& input,
                                                   IRenderService& render,
                                                   IBattleService& battle,
                                                   IConfigService& config,
                                                   ISceneService& scene,
                                                   IOSService& os);

/// @brief `ExitPanel` を生成して返す。
/// @param boxX1   メニューボックスの左端 X 座標
/// @param textY   コンテンツ描画開始 Y 座標
/// @param input   入力サービス
/// @param render  描画サービス
/// @return 生成した `ExitPanel` の所有権
export std::unique_ptr<MenuPanel> CreateExitPanel(int boxX1, int textY,
                                                   IInputService& input,
                                                   IRenderService& render);

} // namespace mc