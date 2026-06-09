module;

#include <dxe.h>

export module Player;

import Character;
import HealthComponent;
import CardService;
import ServiceLocator;
import EventBus;

export enum class EPlayerAction
{
     Magic, Rock, Scissors, Paper,
};


export class Player : public Character
{
private:
    std::unique_ptr<HealthComponent> healthComp;
    EventHandle deathEvent;

public:
    Player()
    {
        healthComp = std::make_unique<HealthComponent>(this);

        deathEvent = EventBus::Subscribe<DeathEvent>(
            [this](const DeathEvent&) { OnPlayerDeath(); }
        );
    }
    
    ~Player()
    {
        EventBus::Unsubscribe(deathEvent);
    }

private:
    void OnPlayerDeath()
    {
        // TODO
    }
};
