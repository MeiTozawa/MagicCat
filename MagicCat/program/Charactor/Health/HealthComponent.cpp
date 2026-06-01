module;

module HealthComponent;
#include <functional>

class HealthComponent : public IDamageable
{
    int health = 100;
    bool isDead = false;

public:
    std::vector<std::function<void()>> OnDeathEvent;
    
    void TakeDamage(int damage) override
    {
        health -= damage;
        if (health <= 0)
        {
            isDead = true;
            for (auto i : OnDeathEvent)
            {
                i();
            }
        }
    }

    void RegisterDeathCallback(const std::function<void()> callback) override
    {
        if (callback != nullptr)
           OnDeathEvent.push_back(callback);
    }

    bool IsDead() override
    {
        return isDead;
    }
};
