module;

#include <RandomUtils.h>
#include <cassert>

module CardService;

import Character;
import InputService;
import EventBus;
import Player;
import AssetService;
import ConfigService;

namespace mc {
    class CardService : public ICardService
    {
    public:
        CardService(IConfigService& configService)
        {
            auto& deckConfig = configService.GetCardConfigs();
            for (const auto& c : deckConfig)
                deck.push_back(Card{ToCardType(c.type), c.value});
        }

        ~CardService() override {}

        void Start() override
        {
            drawPile = std::vector(deck);
            discardPile.clear();
            hand.clear();
            Random::Shuffle(drawPile);
            EventBus::Publish(DeckUpdatedEvent{drawPile.size(), discardPile.size()});
        }

        Card DrawCard() override
        {
            if (hand.size() >= HAND_SIZE_MAX)
                return Card{ECardType::Null, 0};

            if (drawPile.empty() && !discardPile.empty())
                Shuffle();

            assert(drawPile.size() > 0 && "山札が空です");
            auto c = drawPile.back();
            drawPile.pop_back();
            hand.push_back(c);

            EventBus::Publish(DeckUpdatedEvent{drawPile.size(), discardPile.size()});
            EventBus::Publish(HandUpdatedEvent{hand});

            if (drawPile.size() == 0)
                Shuffle();
            return c;
        }

        void DiscardHand() override
        {
            discardPile.insert(discardPile.end(), hand.begin(), hand.end());
            hand.clear();
            EventBus::Publish(HandUpdatedEvent{hand});
        }

        std::vector<Card> GetHandCards() override { return hand; }
        std::vector<Card> GetDrawCards() override { return drawPile; }
        std::vector<Card> GetDiscardCards() override { return discardPile; }

        std::vector<CardData> GetHand() const override
        {
            return ToCardData(hand);
        }

        std::vector<CardData> GetDrawPile() const override
        {
            return ToCardData(drawPile);
        }

        std::vector<CardData> GetDiscardPile() const override
        {
            return ToCardData(discardPile);
        }

        void SetHand(const std::vector<CardData>& cards) override
        {
            hand = FromCardData(cards);
            AssertCardCount();
        }

        void SetDrawPile(const std::vector<CardData>& cards) override
        {
            drawPile = FromCardData(cards);
            AssertCardCount();
        }

        void SetDiscardPile(const std::vector<CardData>& cards) override
        {
            discardPile = FromCardData(cards);
            AssertCardCount();
        }

    private:
        void Shuffle()
        {
            drawPile.insert(drawPile.end(), discardPile.begin(), discardPile.end());
            discardPile.clear();
            Random::Shuffle(drawPile);
            EventBus::Publish(ShuffleEvent());
            EventBus::Publish(DeckUpdatedEvent{drawPile.size(), discardPile.size()});
        }

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

        static std::vector<CardData> ToCardData(const std::vector<Card>& cards)
        {
            std::vector<CardData> result;
            result.reserve(cards.size());
            for (const auto& c : cards)
                result.push_back(CardData{static_cast<int>(c.CardType), c.Power});
            return result;
        }

        static std::vector<Card> FromCardData(const std::vector<CardData>& data)
        {
            std::vector<Card> result;
            result.reserve(data.size());
            for (const auto& cd : data)
                result.push_back(Card{static_cast<ECardType>(cd.type), cd.power});
            return result;
        }

        void AssertCardCount() const
        {
            assert(hand.size() + drawPile.size() + discardPile.size() <= deck.size()
                && "カード枚数がデッキサイズを超えています");
        }

        std::vector<Card> deck = std::vector<Card>();
        std::vector<Card> hand = std::vector<Card>();
        std::vector<Card> drawPile = std::vector<Card>();
        std::vector<Card> discardPile = std::vector<Card>();
    };

    std::unique_ptr<ICardService> CreateCardService(IConfigService& configService)
    {
        return std::make_unique<CardService>(configService);
    }
} // namespace mc
