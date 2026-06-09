module;

#include <RandomUtils.h>
#include <tnl_rect.h>

module CardService;

import ServiceLocator;
import Character;
import InputService;
import EventBus;

class CardService : public ICardService
{
    std::vector<tnl::Rect> rectOfCards = {};

    EventHandle drawCardEvent;

public:
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

        drawCardEvent = EventBus::Subscribe<DrawCardEvent>(
            [this](const DrawCardEvent&) { DrawCard(); }
        );
    }

    ~CardService() override
    {
        EventBus::Unsubscribe(drawCardEvent);
    }

    const std::vector<Card>& GetHandCards() override
    {
        return hand;
    }

    const void DrawCard() override
    {
        if (hand.size() >= HAND_SIZE_MAX)
        {
            return;
        }
        if (drawPile.empty())
        {
            drawPile.insert(drawPile.end(), discardPile.begin(), discardPile.end());
            Random::Shuffle(drawPile);
        }
        auto c = drawPile.back();

        drawPile.pop_back();
        if (c.CardType == Rock || c.CardType == Scissors || c.CardType == Paper)
            EventBus::Publish(AddWeightEvent(static_cast<EAttackType>(c.CardType), c.Offset));

        hand.push_back(c);
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


    const std::vector<Card>& GetDrawCards() override
    {
        return drawPile;
    }

    const std::vector<Card>& GetDiscardCards() override
    {
        return discardPile;
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
