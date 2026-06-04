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
    Player()
    {
        deck.push_back(CARD_ROCK_2);
        deck.push_back(CARD_ROCK_3);
        deck.push_back(CARD_ROCK_4);
        deck.push_back(CARD_SCISSORS_2);
        deck.push_back(CARD_SCISSORS_3);
        deck.push_back(CARD_SCISSORS_4);
        deck.push_back(CARD_PAPER_2);
        deck.push_back(CARD_PAPER_3);
        deck.push_back(CARD_PAPER_4);

        drawPile = std::vector<Card>(deck);

        Random::Shuffle(drawPile);
    }

    const std::vector<Card>& DrawCards(int count)
    {
        discardPile.insert(discardPile.end(), hand.begin(), hand.end());
        hand.clear();
        for (int i = 0; i < count; ++i)
        {
            if (drawPile.size() == 0)
            {
                drawPile.insert(drawPile.end(), discardPile.begin(), discardPile.end());
                Random::Shuffle(drawPile);
            }
            auto c = drawPile.back();
            drawPile.pop_back();
            discardPile.push_back(c);
            hand.push_back(c);
        }
        return hand;
    }

    void PlayCard(int index, Enemy& enemy) const
    {
        enemy.AddWeight(hand[index]);
    }

private:
    std::vector<Card> deck = std::vector<Card>();
    std::vector<Card> hand = std::vector<Card>();
    std::vector<Card> drawPile = std::vector<Card>();
    std::vector<Card> discardPile = std::vector<Card>();
};
