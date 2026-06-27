module;
#include <cassert>

#include <dxe.h>

export module Player;

import Character;
import HealthComponent;
import EventBus;
import AssetService;

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
        static constexpr int DEFAULT_MAX_MP = 10;
        static constexpr int CLAIRVOYANCE_MP_COST = 10;
        static constexpr int POWER_BOOST_MP_COST = 7;
        static constexpr int HEAL_MP_COST = 5;

        std::unique_ptr<HealthComponent> healthComp;
        bool hasUsedClairvoyance = false;
        int healUses = 0;
        int mp = 0;
        int maxMp = DEFAULT_MAX_MP;

    public:
        Player()
        {
            name = L"Player";
            sprite = ESprite::MeowingCat;
            rockDamage = 2;
            scissorsDamage = 2;
            paperDamage = 2;
#ifdef _DEBUG
            maxMp = 100;
            mp = 100;
            rockDamage = 12;
            scissorsDamage = 12;
            paperDamage = 12;
#endif
            healthComp = std::make_unique<HealthComponent>(this);
            tags.push_back(ETag::Player);
        }

        /**
         * @brief プレイヤーのMPを増減させます。
         * 限界値（0未満、または最大MP以上）は自動でクランプされます。
         * @param offset MPの増減量（負の数で消費、正の数で回復）
         */
        void ChangeMp(int offset)
        {
            mp += offset;
            if (mp > maxMp)
                mp = maxMp;
            if (mp < 0)
                mp = 0;
        }

        int GetMp() const { return mp; }

        int GetMaxMp() const { return maxMp; }

        bool IsMagicUsable(EMagic e) const
        {
            switch (e)
            {
            case EMagic::Clairvoyance:
                return !hasUsedClairvoyance && mp >= CLAIRVOYANCE_MP_COST;
            case EMagic::PowerBoost:
                return mp >= POWER_BOOST_MP_COST;
            case EMagic::Heal:
                return healUses < 3 && mp >= HEAL_MP_COST;
            default:
                return false;
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
            case EMagic::Null:
                return false;
            case EMagic::Clairvoyance:
                if (hasUsedClairvoyance)
                    return false;
                if (mp >= CLAIRVOYANCE_MP_COST)
                {
                    ChangeMp(-CLAIRVOYANCE_MP_COST);
                    hasUsedClairvoyance = true;
                    EventBus::Publish<MagicEvent>(MagicEvent{EMagic::Clairvoyance});
                    return true;
                }
                else
                {
                    EventBus::Publish<LackOfMpEvent>({});
                    return false;
                }
            case EMagic::PowerBoost:
                if (mp >= POWER_BOOST_MP_COST)
                {
                    ChangeMp(-POWER_BOOST_MP_COST);
                    SetAttackOffset(3);
                    EventBus::Publish<MagicEvent>(MagicEvent{EMagic::PowerBoost});
                    return true;
                }
                else
                {
                    EventBus::Publish<LackOfMpEvent>({});
                    return false;
                }
            case EMagic::Heal:
                if (healUses >= 3)
                    return false;
                if (mp >= HEAL_MP_COST)
                {
                    ChangeMp(-HEAL_MP_COST);
                    healUses++;
                    healthComp->Heal(1);
                    EventBus::Publish<MagicEvent>(MagicEvent{EMagic::Heal});
                    return true;
                }
                else
                {
                    EventBus::Publish<LackOfMpEvent>({});
                    return false;
                }
            default:
                assert(false && "未実装、または未知の魔法タイプです");
                return false;
            }
        }

        const HealthComponent& GetHealthComponent() const { return *healthComp; }

        void TakeDamage(int amount) const override
        {
            healthComp->TakeDamage(amount);
        }
    };
} // namespace mc
