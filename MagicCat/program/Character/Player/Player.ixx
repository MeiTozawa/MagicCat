module;

#include "../../RandomUtils.h"
export module Player;

import Character;
import HealthComponent;
import CardService;
import Enmey;

export class Player : public Character, public HealthComponent
{
public:
    void PlayCard(int index, Enemy& enemy, const std::vector<Card>& hand) const
    {
        enemy.AddWeight(hand[index]);
    }

private:

};
