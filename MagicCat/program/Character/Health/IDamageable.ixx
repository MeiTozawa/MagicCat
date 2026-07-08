module;

export module IDamageable;

import EventBus;
import Character;

namespace mc {
    /// @brief ダメージ処理とライフ状態の取得を提供するインターフェース
    export class IDamageable
    {
    public:
        virtual ~IDamageable() = default;

        /// @brief ダメージを受ける処理
        /// @param damage ダメージ値
        virtual void TakeDamage(int damage) = 0;

        /// @brief 死亡状態かどうかを取得する
        /// @return 死亡している場合はtrue、生存している場合はfalse
        virtual bool IsDead() const = 0;

        /// @brief 現在のHPを取得する
        /// @return 現在のHP値
        virtual int GetHealth() const = 0;

        /// @brief 最大HPを取得する
        /// @return 最大HP値
        virtual int GetMaxHealth() const = 0;

        /// @brief HP を直接設定する（ロード時の状態復元用）。[0, maxHealth] にクランプされる（イベント発行なし）。
        /// @param value 設定するHP値
        virtual void SetHealth(int value) = 0;

        /// @brief 最大HP を直接設定する（ロード時の状態復元用）（イベント発行なし）。
        /// @param value 設定する最大HP値
        virtual void SetMaxHealth(int value) = 0;

        /// @brief 所有者となるキャラクターを取得する
        /// @return キャラクターポインタ
        virtual const Character* GetOwner() const = 0;
    };
} // namespace mc
