module;
#include <cassert>

#include <RandomUtils.h>

export module Enemy;

import Character;
import HealthComponent;
import EventBus;
import AssetService;
import Player;

namespace mc {
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
              const std::wstring& name = L"Unknown", ESprite sprite = ESprite::Null, int hp = 10)
            : baseWeight(baseWeight)
        {
#ifdef _DEBUG
            this->baseWeight = 1;
#endif
            Character::name = name;
            Character::sprite = sprite;
            Character::rockDamage = rockDamage;
            Character::scissorsDamage = scissorsDamage;
            Character::paperDamage = paperDamage;
            healthComp = std::make_unique<HealthComponent>(this, hp);
            tags.push_back(ETag::Enemy);
        }

        /// @brief 既存の Enemy インスタンスを破棄・再生成せず、その内部データを
        /// 新しい敵の設定で上書きする。HealthComponent や Enemy への参照は維持される。
        void Reset(int baseWeight, int rockDamage, int scissorsDamage, int paperDamage,
                   const std::wstring& name, ESprite sprite, int hp)
        {
            this->baseWeight = baseWeight;
            Character::name = name;
            Character::sprite = sprite;
            Character::rockDamage = rockDamage;
            Character::scissorsDamage = scissorsDamage;
            Character::paperDamage = paperDamage;
            healthComp->Reset(hp);
            ResetWeights();
            isExposed = false;
        }

        void AddWeight(EAttackType t, int weight)
        {
#ifdef _DEBUG
            this->rockWeightOffset = 7;
#else
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
#endif
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
            constexpr EAttackType mappedTypes[] = {EAttackType::Rock, EAttackType::Scissors, EAttackType::Paper};
            return mappedTypes[index];
        }

        float GetLoseRateAgainst(EAttackType playerAttack) const
        {
            int rockWeight = baseWeight + rockWeightOffset;
            int scissorsWeight = baseWeight + scissorsWeightOffset;
            int paperWeight = baseWeight + paperWeightOffset;
            int total = rockWeight + scissorsWeight + paperWeight;
            if (total <= 0)
                return 0.f;

            int losingWeight = 0;
            if (LosesTo(EAttackType::Rock, playerAttack)) losingWeight += rockWeight;
            if (LosesTo(EAttackType::Scissors, playerAttack)) losingWeight += scissorsWeight;
            if (LosesTo(EAttackType::Paper, playerAttack)) losingWeight += paperWeight;

            return static_cast<float>(losingWeight) / total;
        }

        bool operator==(const Enemy& e) const
        {
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

        void TakeDamage(int amount) const override
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
