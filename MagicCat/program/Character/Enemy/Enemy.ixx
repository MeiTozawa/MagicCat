module;
#include <cassert>

#include <RandomUtils.h>

export module Enemy;

import Character;
import HealthComponent;
import EventBus;
import AssetService;
import Player;

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
        bool isExposed = false;

    public:
        Enemy(int baseWeight = 0, int rockDamage = 0, int scissorsDamage = 0, int paperDamage = 0,
              const std::wstring& name = L"Unknown", ESprite sprite = ESprite::Null)
            : baseWeight(baseWeight)
        {
            Character::name = name;
            Character::sprite = sprite;
            Character::rockDamage = rockDamage;
            Character::scissorsDamage = scissorsDamage;
            Character::paperDamage = paperDamage;
            healthComp = std::make_unique<HealthComponent>(this);
            tags.push_back(ETag::Enemy);


        }

        ~Enemy()
        {
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
            default:
                assert(false && "未知の攻撃タイプです");
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
            assert(index >= 0 && index <= 2 && "ランダム攻撃のインデックスが範囲外です");
            constexpr EAttackType mappedTypes[] = { EAttackType::Rock, EAttackType::Scissors, EAttackType::Paper };
            return mappedTypes[index];
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
            default:
                assert(false && "未知の攻撃タイプです");
                return 0;
            }
        }

        bool IsExposed() const { return isExposed; }
        
        int GetBaseWeight() const { return baseWeight; }

        const HealthComponent& GetHealthComponent() const { return *healthComp; }

        void TakeDamage(int amount)
        {
            healthComp->TakeDamage(amount);
        }

        void ResetWeights()
        {
            rockWeightOffset = 0;
            scissorsWeightOffset = 0;
            paperWeightOffset = 0;
        }

        void SetExposed(bool exposed)
        {
            isExposed = exposed;
        }

    private:
        int baseWeight = 0;
        int rockWeightOffset = 0;
        int scissorsWeightOffset = 0;
        int paperWeightOffset = 0;

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
    };
} // namespace mc
