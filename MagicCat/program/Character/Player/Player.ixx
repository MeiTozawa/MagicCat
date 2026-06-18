module;
#include <cassert>

#include <dxe.h>

export module Player;

import Character;
import HealthComponent;
import ServiceLocator;
import EventBus;
import AssetService;

namespace mc
{
    export enum class EPlayerAction
    {
        Null = -1,
        Rock, Scissors, Paper, Magic
    };

    export enum class EMagic
    {
        Null = -1,
        Clairvoyance,
    };

    export struct ChangeMpEvent : IEvent
    {
        int offset;
        ChangeMpEvent(int offset) : offset(offset) {}
    };

    export struct LackOfMpEvent : IEvent {};

    export struct MagicEvent : IEvent
    {
        EMagic magic;
        MagicEvent(EMagic magic) : magic(magic) {}
    };

    /**
     * @brief プレイヤーキャラクターを表すクラス。
     * MPの管理や、プレイヤー専用の魔法（Clairvoyance: 透視）などを扱います。
     */
    export class Player : public Character
    {
        static constexpr int DEFAULT_MAX_MP = 10;
        static constexpr int CLAIRVOYANCE_MP_COST = 5;

        std::unique_ptr<HealthComponent> healthComp;
        EventHandle changeMpEvent;
        EventHandle combatEvent;
        bool hasUsedClairvoyance = false;
        int mp = DEFAULT_MAX_MP;
        int maxMp = DEFAULT_MAX_MP;

    public:
        Player()
        {
            name = L"Player";
            sprite = ESprite::MeowingCat;
#ifdef _DEBUG
            mp = 10;
            rockDamage = 10;
            scissorsDamage = 10;
            paperDamage = 10;
#endif
            healthComp = std::make_unique<HealthComponent>(this);
            tags.push_back(ETag::Player);
            changeMpEvent = EventBus::Subscribe<ChangeMpEvent>(
                [this](const ChangeMpEvent& e) { ChangeMp(e.offset); }
            );

            combatEvent = EventBus::Subscribe<CombatEvent>(
                [this](const CombatEvent& e)
                {
                    if (LosesTo(e.playerAttackType, e.enemyAttackType))
                    {
                        healthComp->TakeDamage(e.enemyAttackDamage);
                    }
                }
            );
        }

        ~Player()
        {
            EventBus::Unsubscribe(changeMpEvent);
            EventBus::Unsubscribe(combatEvent);
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

        /**
         * @brief 魔法を使用します。
         * @param e 使用する魔法の種類
         */
        void UseMagic(EMagic e)
        {
            switch (e)
            {
            case EMagic::Null:
                return;
            case EMagic::Clairvoyance:
                if (hasUsedClairvoyance)
                    return;
                if (mp >= CLAIRVOYANCE_MP_COST)
                {
                    ChangeMp(-CLAIRVOYANCE_MP_COST);
                    hasUsedClairvoyance = true;
                    EventBus::Publish<MagicEvent>({EMagic::Clairvoyance});
                }
                else
                {
                    EventBus::Publish<LackOfMpEvent>({});
                }
                break;
            default:
                assert(false && "未実装、または未知の魔法タイプです");
                break;
            }
        }

        const HealthComponent& GetHealthComponent() const { return *healthComp; }
    };
} // namespace mc
