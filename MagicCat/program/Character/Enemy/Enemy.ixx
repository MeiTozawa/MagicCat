module;

#include <RandomUtils.h>

#include <RandomUtils.h>

export module Enemy;

import Character;
import HealthComponent;
import EventBus;

namespace mc
{
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
        EventHandle combatEvent;

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
            tags.push_back(ETag::Enemy);
            deathEvent = EventBus::Subscribe<DeathEvent>(
                [this](const DeathEvent& e) { if (e.Victim == this) OnEnemyDeath(); }
            );
            addWeightEvent = EventBus::Subscribe<AddWeightEvent>(
                [this](const AddWeightEvent& e) { AddWeight(e.AttackType, e.Offset); }
            );

            combatEvent = EventBus::Subscribe<CombatEvent>(
                [this](const CombatEvent& e)
                {
                    if (Fail(e.enemyAttackType, e.playerAttackType))
                    {
                        healthComp->TakeDamage(e.playerAttackDamage);
                    }
                }
            );
        }

        ~Enemy()
        {
            EventBus::Unsubscribe(deathEvent);
            EventBus::Unsubscribe(addWeightEvent);
            EventBus::Unsubscribe(combatEvent);
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

        EAttackType GetAttackIntent() const
        {
            int rockWeight = baseWeight + rockWeightOffset;
            int scissorsWeight = baseWeight + scissorsWeightOffset;
            int paperWeight = baseWeight + paperWeightOffset;
            int index = Random::RandomSelection(
                rockWeight, scissorsWeight, paperWeight
            );
            auto attackType = static_cast<EAttackType>(index);
            return attackType;
        }

        bool operator==(const Enemy& e) const
        {
            // For simplicity, we just compare pointers for name since they are string literals
            return this->name == e.name &&
                this->baseWeight == e.baseWeight &&
                this->rockDamage == e.rockDamage &&
                this->scissorsDamage == e.scissorsDamage &&
                this->paperDamage == e.paperDamage;
        }

        int GetWeightOffset(EAttackType t) const
        {
            switch (t)
            {
            case EAttackType::Rock:
                return rockWeightOffset;
            case EAttackType::Scissors:
                return scissorsWeightOffset;
            case EAttackType::Paper:
                return paperWeightOffset;
            }
            return -1;
        }

        const HealthComponent& GetHealthComponent() const { return *healthComp; }
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
} // namespace mc
