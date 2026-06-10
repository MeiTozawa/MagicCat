module;

#include <cwchar>

export module Enemy;

import Character;
import HealthComponent;
import EventBus;

export struct AddWeightEvent : IEvent
{
    EAttackType AttackType; 
    int Offset;
    AddWeightEvent(EAttackType attackType, int offset) : AttackType(attackType), Offset(offset) {}
};

export class Enemy : public Character
{
    std::unique_ptr<HealthComponent> healthComp;
    EventHandle deathEvent;
    EventHandle addWeightEvent;
public:
    Enemy(int baseWeight = 0, int rockDamage = 0, int scissorsDamage = 0, int paperDamage = 0,
          const wchar_t* name = L"Unknown")
        : baseWeight(baseWeight),
          rockDamage(rockDamage),
          scissorsDamage(scissorsDamage),
          paperDamage(paperDamage),
          name(name)
    {
        
        healthComp = std::make_unique<HealthComponent>(this);

        deathEvent = EventBus::Subscribe<DeathEvent>(
            [this](const DeathEvent&e) {if (e.Victim == this) OnEnemyDeath(); }
        );
        addWeightEvent = EventBus::Subscribe<AddWeightEvent>(
    [this](const AddWeightEvent& e) { AddWeight(e.AttackType, e.Offset); }
        );
    }
    
    ~Enemy()
    {
        EventBus::Unsubscribe(deathEvent);
        EventBus::Unsubscribe(addWeightEvent);
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
        return std::wcscmp(this->name, e.name) == 0 &&
            this->baseWeight == e.baseWeight &&
            this->rockDamage == e.rockDamage &&
            this->scissorsDamage == e.scissorsDamage &&
            this->paperDamage == e.paperDamage;
    }
    
    [[nodiscard]] int GetRockWeightOffset() const { return rockWeightOffset; }
    [[nodiscard]] int GetScissorsWeightOffset() const { return scissorsWeightOffset; }
    [[nodiscard]] int GetPaperWeightOffset() const { return paperWeightOffset; }
private:
    int baseWeight = 0;
    int rockWeightOffset = 0;
    int scissorsWeightOffset = 0;
    int paperWeightOffset = 0;

    int rockDamage = 0;
    int scissorsDamage = 0;
    int paperDamage = 0;

    const wchar_t* name = L"Unknown";
    
    void AddRockWeight(int weight)
    {
        rockWeightOffset += weight;
    }

    void AddScissorsWeight(int weight)
    {
        scissorsWeightOffset += weight;
    }

    void AddPaperWeight(int weight)
    {
        paperWeightOffset += weight;
    }

    void OnEnemyDeath()
    {
        // TODO
    }
};
