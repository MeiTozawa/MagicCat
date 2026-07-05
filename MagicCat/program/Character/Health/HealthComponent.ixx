module;

#include <functional>
#include <vector>
export module HealthComponent;

import IDamageable;
import EventBus;
import Character;

namespace mc {
    /// @brief キャラクターが死亡したときに発行されるイベント
    export struct DeathEvent : IEvent
    {
        const Character* Victim;
        explicit DeathEvent(const Character* victim) : Victim(victim) {}
    };

    /// @brief キャラクターのHPが変動したときに発行されるイベント
    export struct HealthChangedEvent : IEvent
    {
        const Character* Victim;
        int CurrentHealth;

        HealthChangedEvent(const Character* victim, int currentHealth) : Victim(victim), CurrentHealth(currentHealth) {}
        ;
    };

    /// @brief キャラクターのライフ（HPや死亡状態）の管理を担当するコンポーネント
    export class HealthComponent : public IDamageable
    {
    public:
        /// @brief 所有者と最大HPを指定してコンポーネントを構築する
        /// @param owner 所有するキャラクター
        /// @param hp 初期HP（および最大HP）
        explicit HealthComponent(Character* owner, int hp)
            : maxHp(hp), hp(hp), owner(owner) {}

        /// @brief ダメージを受けてHPを減らし、必要に応じて死亡イベントを発行する
        /// @param damage ダメージ値
        void TakeDamage(int damage) override
        {
            if (isDead) return;

            hp -= damage;
            EventBus::Publish(HealthChangedEvent(GetOwner(), hp));
            if (hp <= 0)
            {
                isDead = true;
                EventBus::Publish(DeathEvent(GetOwner()));
            }
        }

        /// @brief HPを回復させる
        /// @param amount 回復量
        void Heal(int amount)
        {
            if (isDead) return;

            hp += amount;
            if (hp > maxHp) hp = maxHp;
            EventBus::Publish(HealthChangedEvent(GetOwner(), hp));
        }

        /// @brief 静かに HP を最大値にリセットする（イベント発行なし）。
        /// @param newMaxHp 新しい最大HP
        void Reset(int newMaxHp)
        {
            maxHp = newMaxHp;
            hp = newMaxHp;
            isDead = false;
        }

        /// @brief 死亡状態かどうかを取得する
        /// @return 死亡している場合はtrue、生存している場合はfalse
        bool IsDead() const override { return isDead; }

        /// @brief 現在のHPを取得する
        /// @return 現在のHP値
        int GetHealth() const override { return hp; }

        /// @brief 最大HPを取得する
        /// @return 最大HP値
        int GetMaxHealth() const override { return maxHp; }

        /// @brief 所有者となるキャラクターを取得する
        /// @return キャラクターポインタ
        const Character* GetOwner() const override { return owner; }

    private:
        int maxHp;
        int hp;
        bool isDead = false;
        Character* owner;
    };
} // namespace mc
