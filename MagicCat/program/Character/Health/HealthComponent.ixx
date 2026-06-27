module;

#include <functional>
#include <vector>
export module HealthComponent;

import IDamageable;
import EventBus;
import Character;

namespace mc {

export struct DeathEvent : IEvent
{
    const Character* Victim;
    explicit DeathEvent(const Character* victim) : Victim(victim) {}
};

export struct HealthChangedEvent : IEvent
{
    const Character* Victim;
    int CurrentHealth;

    HealthChangedEvent(const Character* victim, int currentHealth) : Victim(victim), CurrentHealth(currentHealth) {};
};

export class HealthComponent : public IDamageable
{
    int maxHp = 10;
    int hp = 10;
    bool isDead = false;
    Character* owner;

public:
    explicit HealthComponent(Character* owner)
        : owner(owner) {}

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

    void Heal(int amount)
    {
        if (isDead) return;

        hp += amount;
        if (hp > maxHp)
            hp = maxHp;

        EventBus::Publish(HealthChangedEvent(GetOwner(), hp));
    }

    bool IsDead() const override
    {
        return isDead;
    }

    int GetHealth() const override
    {
        return hp;
    }

    int GetMaxHealth() const override
    {
        return maxHp;
    }

    const Character* GetOwner() const override
    {
        return owner;
    }

    /// @brief 静かに HP を最大値にリセットする（イベント発行なし）。
    void Reset(int newMaxHp)
    {
        maxHp = newMaxHp;
        hp = newMaxHp;
        isDead = false;
    }
};

} // namespace mc

