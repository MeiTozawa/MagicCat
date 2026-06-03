module;

#include <functional>

export module IDamagable;

export class IDamageable
{
public:
    virtual ~IDamageable() = default;
    
    virtual void TakeDamage(int damage) = 0;
    
    virtual bool IsDead() = 0;
};


