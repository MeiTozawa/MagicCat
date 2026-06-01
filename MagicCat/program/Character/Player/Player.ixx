module;

#include <vector>

module Player;

import Character;
import HealthComponent;
import CardService;


class Player: public Character, IDamageable
{
private:
    std::vector<Card> _cards = std::vector<Card>();
};
