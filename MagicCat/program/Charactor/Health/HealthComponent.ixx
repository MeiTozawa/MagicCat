module;

export module HealthComponent;
#include <functional>

export class IDamageable
{
public:
    virtual ~IDamageable() const = 0;
    
    virtual void TakeDamage(int damage) = 0;

    virtual void RegisterDeathCallback(std::function<void()> callback) = 0;
    
    virtual bool IsDead() = 0;
};


