module;

export module Enemy;

import Character;
import HealthComponent;
import EventBus;

class Player;

export class Enemy : public Character
{
    std::unique_ptr<HealthComponent> healthComp;
public:
    Enemy(int baseWeight = 0, int rockDamage = 0, int scissorsDamage = 0, int paperDamage = 0,
          const wchar_t* name = L"Unknown")
        : name(name),
          baseWeight(baseWeight),
          rockDamage(rockDamage),
          scissorsDamage(scissorsDamage),
          paperDamage(paperDamage)
    {
        rockWeight = baseWeight;
        scissorsWeight = baseWeight;
        paperWeight = baseWeight;
        
        healthComp = std::make_unique<HealthComponent>(this);

        EventBus::Subscribe<DeathEvent>(
            [this](const DeathEvent&) { OnEnemyDeath(); }
        );
    }

    void AddWeight(EAttackType t, int weight)
    {
        switch (t)
        {
        case EAttackType::Rock:
            AddRockWeight(weight);
            break;
        case EAttackType::Scissors:
            AddScissorsWeight(weight);
            break;
        case EAttackType::Paper:
            AddPaperWeight(weight);
            break;
        }
    }

    // EAttackType Attack() const
    // {
    //     return static_cast<EAttackType>(0);
    // }

    bool operator==(const Enemy& e) const
    {
        // For simplicity, we just compare pointers for name since they are string literals
        return this->name == e.name &&
            this->baseWeight == e.baseWeight &&
            this->rockDamage == e.rockDamage &&
            this->scissorsDamage == e.scissorsDamage &&
            this->paperDamage == e.paperDamage;
    }

    const wchar_t* name = L"Unknown";

    int baseWeight = 0;
    int rockWeight = 0;
    int scissorsWeight = 0;
    int paperWeight = 0;

    int rockDamage = 0;
    int scissorsDamage = 0;
    int paperDamage = 0;

private:
    void AddRockWeight(int weight)
    {
        rockWeight += weight;
    }

    void AddScissorsWeight(int weight)
    {
        scissorsWeight += weight;
    }

    void AddPaperWeight(int weight)
    {
        paperWeight += weight;
    }

    void OnEnemyDeath()
    {
        // TODO
    }
};
