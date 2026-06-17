module;

#include <dxe.h>

export module Player;

import Character;
import HealthComponent;
import ServiceLocator;
import EventBus;
import AssetService;

namespace mc {

export enum class EPlayerAction
{
    Rock, Scissors, Paper, Magic
};

export struct ChangeMpEvent : IEvent
{
    int offset;
    ChangeMpEvent(int offset) : offset(offset) {}
};

export class Player : public Character
{
    std::unique_ptr<HealthComponent> healthComp;
    EventHandle changeMpEvent;
    EventHandle combatEvent;
    int mp = 0;
    int maxMp = 10;

public:
    Player()
    {
        name = L"Player";
        sprite = ESprite::MeowingCat;
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

    void ChangeMp(int offset) { mp += offset; }

    int GetMp() const { return mp; }

    int GetMaxMp() const { return maxMp; }

    const HealthComponent& GetHealthComponent() const { return *healthComp; }
};

} // namespace mc

