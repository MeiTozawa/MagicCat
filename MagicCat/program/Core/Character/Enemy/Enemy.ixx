module;

export module Enemy;

import Character;
import HealthComponent;
import CardService;


class Player;

export class Enemy : public Character, public HealthComponent
{
public:
    Enemy(int baseWeight = 0, int rockDamage = 0, int scissorsDamage = 0, int paperDamage = 0, const wchar_t* name = L"Unknown")
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
        case Rock:
            AddRockWeight(card.Offset);
            break;
        case Scissors:
            AddScissorsWeight(card.Offset);
            break;
        case Paper:
            AddPaperWeight(card.Offset);
            break;
        }
    }

    ECardType Attack() const
    {
        return static_cast<ECardType>(0);
    }

    bool operator==(const Enemy& e) const
    {
        // For simplicity, we just compare pointers for name since they are string literals
        return this->name == e.name &&
            this->baseWeight == e.baseWeight &&
            this->rockDamage == e.rockDamage &&
            this->scissorsDamage == e.scissorsDamage &&
            this->paperDamage == e.paperDamage;
    }

    const wchar_t* name = L"Unknown";

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
