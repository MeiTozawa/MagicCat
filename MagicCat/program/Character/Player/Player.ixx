module;

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
        Rock, Scissors, Paper, Magic
    };

    export enum class EMagic
    {
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

    export class Player : public Character
    {
        std::unique_ptr<HealthComponent> healthComp;
        EventHandle changeMpEvent;
        EventHandle combatEvent;
        bool isUsedClairvoyance = false;
        int mp = 0;
        int maxMp = 10;

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
                    if (Fail(e.playerAttackType, e.enemyAttackType))
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

        void ChangeMp(int offset)
        {
            mp += offset;
            if (mp > maxMp)
                mp = maxMp;
        }

        int GetMp() const { return mp; }

        int GetMaxMp() const { return maxMp; }

        void UseMagic(EMagic e)
        {
            switch (e)
            {
            case EMagic::Clairvoyance:
                if (isUsedClairvoyance)
                    return;
                if (mp >= 5)
                {
                    ChangeMp(-5);
                    isUsedClairvoyance = true;
                    EventBus::Publish<MagicEvent>({EMagic::Clairvoyance});
                }
                else
                {
                    EventBus::Publish<LackOfMpEvent>({});
                }
            default: ;
            }
        }

        const HealthComponent& GetHealthComponent() const { return *healthComp; }
    };
} // namespace mc
