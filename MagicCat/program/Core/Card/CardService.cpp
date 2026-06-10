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


    EventHandle moveFocusToLeftEvent;
    EventHandle moveFocusToRightEvent;
    EventHandle drawCardEvent;
    EventHandle playCardEvent;

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

        moveFocusToLeftEvent = EventBus::Subscribe<MoveFocusToLeftEvent>(
            [this](const MoveFocusToLeftEvent&) { MoveFocusToLeft(); }
        );
        moveFocusToRightEvent = EventBus::Subscribe<MoveFocusToRightEvent>(
            [this](const MoveFocusToRightEvent&) { MoveFocusToRight(); }
        );
        drawCardEvent = EventBus::Subscribe<DrawCardEvent>(
            [this](const DrawCardEvent&) { DrawCard(); }
        );
        playCardEvent = EventBus::Subscribe<PlayCardEvent>(
            [this](const PlayCardEvent&) { PlayCard(); }
        );
    }

    ~CardService() override
    {
        EventBus::Unsubscribe(moveFocusToLeftEvent);
        EventBus::Unsubscribe(moveFocusToRightEvent);
        EventBus::Unsubscribe(drawCardEvent);
        EventBus::Unsubscribe(playCardEvent);
    }

    const std::vector<Card>& GetHandCards() override
    {
        return hand;
    }

    const std::vector<Card>& DrawCard() override
    {
        if (drawPile.empty())
        {
            drawPile.insert(drawPile.end(), discardPile.begin(), discardPile.end());
            discardPile.clear();
            Random::Shuffle(drawPile);
        }
        if (drawPile.empty()) return hand;
        auto c = drawPile.back();

        drawPile.pop_back();

        if (hand.empty())
        {
            focus = 0;
            c.is_selected = true;
        }
        hand.push_back(c);

        EventBus::Publish(HandUpdatedEvent(hand));
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

    void MoveFocusToRight() override
    {
        if (hand.empty()) return;
        if (focus >= hand.size() - 1) return;

        hand[focus].is_selected = false;
        focus++;
        hand[focus].is_selected = true;
        EventBus::Publish(HandUpdatedEvent{hand});
    }

    void MoveFocusToLeft() override
    {
        if (hand.empty()) return;
        if (focus == 0) return;

        hand[focus].is_selected = false;
        focus--;
        hand[focus].is_selected = true;
        EventBus::Publish(HandUpdatedEvent{hand});
    }

    void PlayCard() override
    {
        if (hand.empty()) return;
        auto card = hand.begin() + focus;
        int t = card->CardType;
        assert(t >= 0 && t <= 2);
        EventBus::Publish(AddWeightEvent(static_cast<EAttackType>(t), card->Offset));

        discardPile.push_back(*card);
        hand.erase(card);

        if (!hand.empty())
        {
            if (focus >= static_cast<int>(hand.size()))
                focus = static_cast<int>(hand.size()) - 1;
            hand[focus].is_selected = true;
        }
        else
        {
            focus = 0;
        }
            
        EventBus::Publish(HandUpdatedEvent(hand));
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
    std::vector<Card> deck;
    std::vector<Card> hand;
    std::vector<Card> drawPile;
    std::vector<Card> discardPile;
    int focus = 0;
};

static struct RegisterCardService
{
    RegisterCardService()
    {
        ServiceLocator::RegisterSingleton<ICardService, CardService>(std::make_shared<CardService>());
    }
} autoRegister_CardService;
