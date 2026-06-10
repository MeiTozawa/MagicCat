module;

export module IDamageable;

import EventBus;
import Character;

namespace mc {

export class IDamageable
{
public:
    virtual ~IDamageable() = default;

    virtual void TakeDamage(int damage) = 0;

    virtual bool IsDead() const = 0;

    virtual int GetHealth() const = 0;

    virtual int GetMaxHealth() const = 0;

    virtual const Character* GetOwner() const = 0;
};

} // namespace mc

