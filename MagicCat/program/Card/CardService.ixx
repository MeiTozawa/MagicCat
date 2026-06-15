module;

#include <string>
#include <vector>
#include <tnl_rect.h>

export module CardService;

import Enemy;
import EventBus;

namespace mc
{
    export constexpr int CARD_HEIGHT = 300;
    export constexpr int CARD_WIDTH = 200;
    export constexpr int HAND_SIZE_MAX = 4;

    export enum ECardType
    {
        Rock = 0, Scissors = 1, Paper = 2, Magic = 3,
        Null = -1,
    };

    // ⚠️ 警告：伝達性が損なわれているため、ソートや関連付けコンテナには絶対に使用しないでください！
    constexpr std::strong_ordering operator<=>(ECardType l, ECardType r)
    {
        if (l == r) return std::strong_ordering::equal;

        int diff = (static_cast<int>(l) - static_cast<int>(r) + 3) % 3;

        if (diff == 1)
        {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::less;
    }

    constexpr bool operator==(ECardType l, ECardType r)
    {
        return (l <=> r) == std::strong_ordering::equal;
    }


    export struct Card
    {
        ECardType CardType;
        // 敵が「ECardType」を出す確率のオフセット
        int Value = 0;
    };

    export class ICardService
    {
    public:
        virtual ~ICardService() = default;
        virtual const void DrawCard() = 0;
        virtual const std::vector<Card>& GetHandCards() = 0;
        virtual const std::vector<Card>& GetDrawCards() = 0;
        virtual const std::vector<Card>& GetDiscardCards() = 0;
    };

    export struct DrawCardEvent : IEvent {};
    export struct ShuffleEvent : IEvent {};


    export struct DeckUpdatedEvent : IEvent
    {
        explicit DeckUpdatedEvent(const size_t draw_pile_count, const size_t discard_pile_count)
            : drawPileCount(draw_pile_count),
              discardPileCount(discard_pile_count) {}

        const size_t drawPileCount;
        const size_t discardPileCount;
    };

    export struct HandUpdatedEvent : IEvent
    {
        explicit HandUpdatedEvent(const std::vector<Card>& cards)
            : cards(cards) {}

        const std::vector<Card>& cards;
    };

    export constexpr Card CARD_ROCK_2 = {Rock, 2};
    export constexpr Card CARD_ROCK_3 = {Rock, 3};
    export constexpr Card CARD_ROCK_4 = {Rock, 4};
    export constexpr Card CARD_SCISSORS_2 = {Scissors, 2};
    export constexpr Card CARD_SCISSORS_3 = {Scissors, 3};
    export constexpr Card CARD_SCISSORS_4 = {Scissors, 4};
    export constexpr Card CARD_PAPER_2 = {Paper, 2};
    export constexpr Card CARD_PAPER_3 = {Paper, 3};
    export constexpr Card CARD_PAPER_4 = {Paper, 4};
    export constexpr Card CARD_MAGIC_2 = {Magic, 2};
    export constexpr Card CARD_MAGIC_3 = {Magic, 3};
    export constexpr Card CARD_MAGIC_4 = {Magic, 4};

    export Shared<ICardService> CreateCardService();
} // namespace mc
