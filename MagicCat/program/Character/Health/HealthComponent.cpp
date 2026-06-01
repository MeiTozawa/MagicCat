module;

#include <functional>

module HealthComponent;

class HealthComponent : public IDamageable
{
    int _health = 100;
    bool _isDead = false;

public:
    std::vector<std::function<void()>> OnDeathEvent;
    
    void TakeDamage(int damage) override
    {
        _health -= damage;
        if (_health <= 0)
        {
            _isDead = true;
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
        return _isDead;
    }
};
