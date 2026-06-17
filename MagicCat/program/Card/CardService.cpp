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
import ConfigService;

namespace mc
{
    class CardService : public ICardService
    {
        EventHandle drawCardEvent;
        EventHandle combatEvent;

    public:
        CardService()
        {
            auto& deckConfig = ServiceLocator::Get<IConfigService>()->GetCardConfigs();
            for (const auto& c : deckConfig)
            {
                auto card = Card{ToCardType(c.type), c.value};

                deck.push_back(card);
            }
            drawCardEvent = EventBus::Subscribe<DrawCardEvent>(
                [this](const DrawCardEvent&) { DrawCard(); }
            );

            combatEvent = EventBus::Subscribe<CombatEvent>(
                [this](const CombatEvent&)
                {
                    discardPile.insert(discardPile.end(), hand.begin(), hand.end());
                    hand.clear();
                    EventBus::Publish(HandUpdatedEvent{hand});
                }
            );
        }

        void Start() override
        {
            drawPile = std::vector(deck);
            discardPile.clear();
            hand.clear();

            Random::Shuffle(drawPile);
            EventBus::Publish(DeckUpdatedEvent{drawPile.size(), discardPile.size()});
        }

        ~CardService() override
        {
            EventBus::Unsubscribe(drawCardEvent);
            EventBus::Unsubscribe(combatEvent);
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
                discardPile.clear(); // 再シャッフル時に捨て札を確実にクリアする
                Random::Shuffle(drawPile);
                EventBus::Publish(ShuffleEvent());
            }
            assert(drawPile.size() > 0);
            auto c = drawPile.back();

            drawPile.pop_back();
            if (c.CardType == Magic)
                EventBus::Publish(ChangeMpEvent(c.Value));
            else if (c.CardType == Rock || c.CardType == Scissors || c.CardType == Paper)
                EventBus::Publish(AddWeightEvent(ToAttackType(c.CardType), c.Value));

            hand.push_back(c);

            EventBus::Publish(DeckUpdatedEvent{drawPile.size(), discardPile.size()});
            EventBus::Publish(HandUpdatedEvent{hand});
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

        static ECardType ToCardType(int type)
        {
            switch (type)
            {
            case 0: return Rock;
            case 1: return Scissors;
            case 2: return Paper;
            case 3: return Magic;
            default: return Null;
            }
        }

        static EAttackType ToAttackType(ECardType type)
        {
            switch (type)
            {
            case Rock: return EAttackType::Rock;
            case Scissors: return EAttackType::Scissors;
            case Paper: return EAttackType::Paper;
            default: return EAttackType::Rock;
            }
        }
    };

    Shared<ICardService> CreateCardService()
    {
        return std::make_shared<CardService>();
    }
} // namespace mc
