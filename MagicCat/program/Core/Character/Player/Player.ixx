module;

#include <dxe.h>

export module Player;

import Character;
import HealthComponent;
import CardService;
import ServiceLocator;



export class Player : public Character, public HealthComponent
{
public:
    Player()
    {
        cardService = ServiceLocator::Get<ICardService>();
    }
private:
    Shared<ICardService> cardService;



};
