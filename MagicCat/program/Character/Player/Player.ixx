module;

#include <vector>
#include <functional>

export module Player;

import Character;
import HealthComponent;
import CardService;


export class Player: public Character, public HealthComponent
{

private:
    std::vector<Card> _cards = std::vector<Card>();
};
