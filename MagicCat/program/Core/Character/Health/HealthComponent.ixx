module;

#include <functional>
#include <vector>
export module HealthComponent;

import IDamageable;
import EventBus;
import Character;

export struct DeathEvent : public IEvent
{
    const Character* Victim;
    DeathEvent(const Character* victim) : Victim(victim) {}
};

export struct HealthChangedEvent : public IEvent
{
    const Character* Victim;
    int CurrentHealth;

    HealthChangedEvent(const Character* victim, int currentHealth) : Victim(victim), CurrentHealth(currentHealth) {};
};

export class HealthComponent : public IDamageable
{
    int health = 100;
    bool isDead = false;
    Character* owner;

public:
    explicit HealthComponent(Character* owner, int initialHealth = 100)
        : health(initialHealth), owner(owner) {}

    void TakeDamage(int damage) override
    {
        if (isDead) return;

        health -= damage;

        EventBus::Publish(HealthChangedEvent(GetOwner(), health));
        if (health <= 0)
        {
            isDead = true;
            EventBus::Publish(DeathEvent(GetOwner()));
        }
    }

    bool IsDead() const override
    {
        return isDead;
    }

    int GetHealth() const override
    {
        return health;
    }

    const Character* GetOwner() const override
    {
        return owner;
    }
};
