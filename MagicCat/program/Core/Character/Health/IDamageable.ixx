module;

#include <functional>

export module IDamageable;

export class IDamageable
{
public:
    virtual ~IDamageable() = default;
    
    virtual void TakeDamage(int damage) = 0;
    
    virtual bool IsDead() const = 0;
};


