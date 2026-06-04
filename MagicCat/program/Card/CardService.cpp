module;

#include <RandomUtils.h>
#include <tnl_rect.h>

module CardService;

import ServiceLocator;

class CardService : public ICardService
{
public:
    std::vector<tnl::Rect> rectOfCards = {};
    
    
    CardService()
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

    const std::vector<Card>& GetHandCards()
    {
        return hand;
    }

    const std::vector<Card>& CardService::DrawCards(int count) override
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

    void ClearRectOfCards() override
    {
        rectOfCards.clear();
    }
    
    const std::vector<tnl::Rect>& GetRectOfCards() override
    {
        return rectOfCards;
    }
    
    void PushBackRectOfCard(tnl::Rect r) override
    {
        rectOfCards.push_back(r);
    }

private:
    std::vector<Card> deck = std::vector<Card>();
    std::vector<Card> hand = std::vector<Card>();
    std::vector<Card> drawPile = std::vector<Card>();
    std::vector<Card> discardPile = std::vector<Card>();
};

static struct RegisterCardService
{
    RegisterCardService()
    {
        ServiceLocator::RegisterSingleton<ICardService, CardService>(std::make_shared<CardService>());
    }
} autoRegister_CardService;
