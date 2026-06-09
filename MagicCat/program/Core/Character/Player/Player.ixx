module;

#include <dxe.h>

export module Player;

import Character;
import HealthComponent;
import CardService;
import ServiceLocator;
import EventBus;


export class Player : public Character
{
private:
    std::unique_ptr<HealthComponent> healthComp;

public:
    Player()
    {
        healthComp = std::make_unique<HealthComponent>(this);

        EventBus::Subscribe<DeathEvent>(
            [this](const DeathEvent&) { OnPlayerDeath(); }
        );
    }

private:
    void OnPlayerDeath()
    {
        // TODO
    }
};
