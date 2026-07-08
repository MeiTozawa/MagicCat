module;

#include <memory>
#include <RenderUtils.h>

export module Displayer:Menu;

import DisplayerBase;
import InputService;
import RenderService;
import AudioService;
import BattleService;
import ConfigService;
import SceneService;
import OSService;
import ButtonGroup;

namespace mc {

/// @brief メニュー内の各サブパネルの基底クラス。
///        `Displayer` のライフサイクル（`OnUpdate` / `OnDraw`）を通じて
///        入力処理と描画を担い、`MenuScene` から多態的に操作される。
///        座標 `boxX1` / `textY` は構築時に注入され、`Displayer::x` / `y` に格納される。
export class MenuPanel : public Displayer {
public:
    MenuPanel(int boxX1, int textY) { SetPosition(boxX1, textY); }
    virtual ~MenuPanel() = default;

    /// @brief パネルが行フォーカス層（アクティブ状態）にあるかを返す。
    /// @return アクティブなら `true`
    virtual bool IsActive() const = 0;

    /// @brief トップボタンバーから本パネルに遷移する際に `MenuScene` が呼び出す。
    ///        内部フォーカスを先頭にリセットする。
    virtual void Activate() = 0;

    /// @brief ユーザーがパネルを離れる際に呼び出す。非アクティブ状態に移行する。
    virtual void Deactivate() = 0;
};

/// @brief `RulesPanel` を生成して返す。
/// @param boxX1   メニューボックスの左端 X 座標
/// @param textY   コンテンツ描画開始 Y 座標
/// @param input   入力サービス
/// @param render  描画サービス
/// @return 生成した `RulesPanel` の所有権
export std::unique_ptr<MenuPanel> CreateRulesPanel(int boxX1, int textY,
                                                    IInputService& input,
                                                    IRenderService& render);

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

} // namespace mc
