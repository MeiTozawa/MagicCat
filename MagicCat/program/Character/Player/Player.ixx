module;
#include <cassert>

#include <dxe.h>

export module Player;

import Character;
import HealthComponent;
import EventBus;
import AssetService;
import ConfigService;

namespace mc {
    export enum class EPlayerAction
    {
        Null = -1,
        Rock, Scissors, Paper, Magic
    };

    export enum class EMagic
    {
        Null = -1,
        Clairvoyance,
        PowerBoost,
        Heal
    };

    export struct ChangeMpEvent : IEvent
    {
        int offset;
        explicit ChangeMpEvent(int offset) : offset(offset) {}
    };

    export struct LackOfMpEvent : IEvent {};

    export struct MagicEvent : IEvent
    {
        EMagic magic;
        explicit MagicEvent(EMagic magic) : magic(magic) {}
    };

    /**
     * @brief プレイヤーキャラクターを表すクラス。
     * MPの管理や、プレイヤー専用の魔法（Clairvoyance: 透視）などを扱います。
     */
    export class Player : public Character
    {
    public:
        static PlayerConfig GetDefaultConfig()
        {
            PlayerConfig config;
            config.initialHp = 15;
            config.maxMp = 10;
            config.rockDamage = 2;
            config.scissorsDamage = 2;
            config.paperDamage = 2;
            config.spriteName = "MeowingCat";
            config.clairvoyanceMpCost = 10;
            config.powerBoostMpCost = 7;
            config.powerBoostDamageOffset = 2;
            config.healMpCost = 5;
            config.healAmount = 2;
            config.maxHealUses = 3;
            return config;
        }

        Player() : Player(GetDefaultConfig(), ESprite::MeowingCat) {}

        Player(const PlayerConfig& config, ESprite spriteVal)
        {
            name = L"Player";
            sprite = spriteVal;
            rockDamage = config.rockDamage;
            scissorsDamage = config.scissorsDamage;
            paperDamage = config.paperDamage;
            maxMp = config.maxMp;
            mp = 0;
#ifdef _DEBUG
            maxMp = 100;
            mp = 10;
#endif
            healthComp = std::make_unique<HealthComponent>(this, config.initialHp);
            tags.push_back(ETag::Player);

            clairvoyanceMpCost = config.clairvoyanceMpCost;
            powerBoostMpCost = config.powerBoostMpCost;
            powerBoostDamageOffset = config.powerBoostDamageOffset;
            healMpCost = config.healMpCost;
            healAmount = config.healAmount;
            maxHealUses = config.maxHealUses;
        }

        /**
         * @brief プレイヤーのMPを増減させます。
         * 限界値（0未満、または最大MP以上）は自動でクランプされます。
         * @param offset MPの増減量（負の数で消費、正の数で回復）
         */
        void ChangeMp(int offset)
        {
            mp += offset;
            if (mp > maxMp) mp = maxMp;
            if (mp < 0)     mp = 0;
        }

        int GetMp() const { return mp; }
        int GetMaxMp() const { return maxMp; }

        bool IsMagicUsable(EMagic e) const
        {
            switch (e)
            {
            case EMagic::Clairvoyance: return !hasUsedClairvoyance && mp >= clairvoyanceMpCost;
            case EMagic::PowerBoost:   return mp >= powerBoostMpCost;
            case EMagic::Heal:         return healUses < maxHealUses && mp >= healMpCost;
            default:                   return false;
            }
        }

        /**
         * @brief 魔法を使用します。
         * @param e 使用する魔法の種類
         */
        bool UseMagic(EMagic e)
        {
            switch (e)
            {
            case EMagic::Null:         return false;
            case EMagic::Clairvoyance: return UseClairvoyance();
            case EMagic::PowerBoost:   return UsePowerBoost();
            case EMagic::Heal:         return UseHeal();
            default:
                assert(false && "未実装、または未知の魔法タイプです");
                return false;
            }
        }

        const HealthComponent& GetHealthComponent() const { return *healthComp; }

        void TakeDamage(int amount) const override { healthComp->TakeDamage(amount); }

    private:
        bool UseClairvoyance()
        {
            if (hasUsedClairvoyance) return false;
            if (mp >= clairvoyanceMpCost)
            {
                ChangeMp(-clairvoyanceMpCost);
                hasUsedClairvoyance = true;
                EventBus::Publish<MagicEvent>(MagicEvent{EMagic::Clairvoyance});
                return true;
            }
            EventBus::Publish<LackOfMpEvent>({});
            return false;
        }

        bool UsePowerBoost()
        {
            if (mp >= powerBoostMpCost)
            {
                ChangeMp(-powerBoostMpCost);
                SetDamageOffset(powerBoostDamageOffset);
                EventBus::Publish<MagicEvent>(MagicEvent{EMagic::PowerBoost});
                return true;
            }
            EventBus::Publish<LackOfMpEvent>({});
            return false;
        }

        bool UseHeal()
        {
            if (healUses >= maxHealUses) return false;
            if (mp >= healMpCost)
            {
                ChangeMp(-healMpCost);
                healUses++;
                healthComp->Heal(healAmount);
                EventBus::Publish<MagicEvent>(MagicEvent{EMagic::Heal});
                return true;
            }
            EventBus::Publish<LackOfMpEvent>({});
            return false;
        }

        std::unique_ptr<HealthComponent> healthComp;
        bool hasUsedClairvoyance = false;
        int healUses = 0;
        int mp = 0;
        int maxMp = 10;
        int clairvoyanceMpCost = 10;
        int powerBoostMpCost = 7;
        int powerBoostDamageOffset = 2;
        int healMpCost = 5;
        int healAmount = 2;
        int maxHealUses = 3;
    };
} // namespace mc
