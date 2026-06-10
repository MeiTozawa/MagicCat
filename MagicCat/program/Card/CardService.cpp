module;

#include <RandomUtils.h>
#include <tnl_rect.h>

module CardService;

import ServiceLocator;
import Character;
import InputService;
import EventBus;
import Player;
import AssetService;

namespace mc
{
    class CardService : public ICardService
    {
        std::vector<tnl::Rect> rectOfCards = {};

        EventHandle drawCardEvent;
        EventHandle cardRectsUpdatedEvent;

    public:
        CardService()
        {
            auto& deckConfig = ServiceLocator::Get<IAssetService>()->GetCardConfigs();
            for (const auto& c : deckConfig)
            {
                auto card = Card{static_cast<ECardType>(c.type), c.value};

                deck.push_back(card);
            }

            drawPile = std::vector<Card>(deck);

            Random::Shuffle(drawPile);

            drawCardEvent = EventBus::Subscribe<DrawCardEvent>(
                [this](const DrawCardEvent&) { DrawCard(); }
            );
            cardRectsUpdatedEvent = EventBus::Subscribe<CardRectsUpdatedEvent>(
                [this](const CardRectsUpdatedEvent& e) { rectOfCards = e.rects; }
            );

            EventBus::Publish(DeckUpdatedEvent{drawPile.size(), discardPile.size()});
            EventBus::Publish(HandUpdatedEvent{hand});
        }

        ~CardService() override
        {
            EventBus::Unsubscribe(drawCardEvent);
            EventBus::Unsubscribe(cardRectsUpdatedEvent);
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
                discardPile.clear(); // Ensure discard pile is cleared when reshuffled
                Random::Shuffle(drawPile);
            }
            assert(drawPile.size() > 0);
            auto c = drawPile.back();

            drawPile.pop_back();
            if (c.CardType == Magic)
                EventBus::Publish(ChangeMpEvent(c.Value));
            else if (c.CardType == Rock || c.CardType == Scissors || c.CardType == Paper)
                EventBus::Publish(AddWeightEvent(static_cast<EAttackType>(c.CardType), c.Value));

            hand.push_back(c);

            EventBus::Publish(DeckUpdatedEvent{drawPile.size(), discardPile.size()});
            EventBus::Publish(HandUpdatedEvent{hand});
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

    Shared<ICardService> CreateCardService()
    {
        return std::make_shared<CardService>();
    }
} // namespace mc
