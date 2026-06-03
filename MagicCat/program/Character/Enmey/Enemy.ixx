module;

#include <random>
#include "../../RandomUtils.h"

export module Enmey;

import Character;
import HealthComponent;
import CardService;


export class Enemy : public Character, public HealthComponent
{
public:
    Enemy(int baseWeight, int rockDamage, int scissorsDamage, int paperDamage, std::wstring name)
        : baseWeight(baseWeight),
          rockDamage(rockDamage),
          scissorsDamage(scissorsDamage),
          paperDamage(paperDamage),
          name(name)
    {
        rockWeight = baseWeight;
        scissorsWeight = baseWeight;
        paperWeight = baseWeight;
    }

    void AddWeight(Card card)
    {
        switch (card.CardType)
        {
        case ROCK:
            AddRockWeight(card.Offset);
            break;
        case SCISSORS:
            AddScissorsWeight(card.Offset);
            break;
        case PAPER:
            AddPaperWeight(card.Offset);
            break;
        }
    }

    void Attack()
    {
        int index = Random::RandomSelection(
            rockWeight, scissorsWeight, paperWeight
        );
        ECardType attackType = static_cast<ECardType>(index);
    }

    bool operator==(const Enemy& e) const
    {
        return this->name == e.name &&
            this->baseWeight == e.baseWeight &&
            this->rockDamage == e.rockDamage &&
            this->scissorsDamage == e.scissorsDamage &&
            this->paperDamage == e.paperDamage;
    }
    
    
    std::wstring name = L"";

    int baseWeight = 0;
    int rockWeight = 0;
    int scissorsWeight = 0;
    int paperWeight = 0;

    int rockDamage = 0;
    int scissorsDamage = 0;
    int paperDamage = 0;
    
private:
    void AddRockWeight(int weight)
    {
        rockWeight += weight;
    }

    void AddScissorsWeight(int weight)
    {
        scissorsWeight += weight;
    }

    void AddPaperWeight(int weight)
    {
        paperWeight += weight;
    }
};
