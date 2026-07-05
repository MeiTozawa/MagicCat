module;

#include <memory>

export module CombatController;

import InputService;
import BattleService;
import SceneService;
import CardService;

namespace mc {
    export constexpr int ACTION_MAGIC = 0;
    export constexpr int ACTION_ROCK = 1;
    export constexpr int ACTION_SCISSORS = 2;
    export constexpr int ACTION_PAPER = 3;
    export constexpr int ACTION_MAX = ACTION_PAPER;

    /// @brief 戦闘（ターン解決、カード選択、勝敗解決など）のプレイヤー入力および進行管理を制御するインターフェース
    export class ICombatController
    {
    public:
        virtual ~ICombatController() = default;

        /// @brief 毎フレームの更新処理。プレイヤーのカード選択判定や戦闘アクション解決を行う。
        /// @param deltaTime 前のフレームからの経過時間（秒）
        virtual void Update(float deltaTime) = 0;

        /// @brief コントローラーの状態（選択インデックス等）を初期状態にリセットする
        virtual void Reset() = 0;
    };

    export std::unique_ptr<ICombatController> CreateCombatController(IInputService& inputService,
                                                                     IBattleService& battleService,
                                                                     ISceneService& sceneService,
                                                                     ICardService& cardService);
}
