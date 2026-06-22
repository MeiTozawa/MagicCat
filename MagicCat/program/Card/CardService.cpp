module;

#include <RandomUtils.h>
#include <cassert>
#include <stdexcept>

module CardService;

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
    public:
        CardService(IConfigService& configService)
        {
            auto& deckConfig = configService.GetCardConfigs();
            for (const auto& c : deckConfig)
            {
                auto card = Card{ToCardType(c.type), c.value};
                deck.push_back(card);
            }
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
        }

        std::vector<Card> GetHandCards() override
        {
            return hand;
        }

        void DiscardHand() override
        {
            discardPile.insert(discardPile.end(), hand.begin(), hand.end());
            hand.clear();
            EventBus::Publish(HandUpdatedEvent{hand});
        }

        Card DrawCard() override
        {
            if (hand.size() >= HAND_SIZE_MAX)
            {
                return Card{ECardType::Null, 0};
            }
            if (drawPile.empty())
            {
                if (discardPile.empty()) return Card{ECardType::Null, 0};
                drawPile.insert(drawPile.end(), discardPile.begin(), discardPile.end());
                discardPile.clear();
                Random::Shuffle(drawPile);
                EventBus::Publish(ShuffleEvent());
            }
            assert(drawPile.size() > 0 && "山札が空です");
            auto c = drawPile.back();

            drawPile.pop_back();

            hand.push_back(c);

            EventBus::Publish(DeckUpdatedEvent{drawPile.size(), discardPile.size()});
            EventBus::Publish(HandUpdatedEvent{hand});

            return c;
        }


        std::vector<Card> GetDrawCards() override
        {
            return drawPile;
        }

        std::vector<Card> GetDiscardCards() override
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
            case 0: return ECardType::Rock;
            case 1: return ECardType::Scissors;
            case 2: return ECardType::Paper;
            case 3: return ECardType::Magic;
            default:
                assert(false && "外部設定から不正なカードタイプが読み込まれました");
                return ECardType::Null;
            }
        }

    };

    std::unique_ptr<ICardService> CreateCardService(IConfigService& configService)
    {
        return std::make_unique<CardService>(configService);
    }
} // namespace mc
