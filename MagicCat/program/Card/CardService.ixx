module;

#include <string>
#include <vector>
#include <tnl_rect.h>

export module CardService;

import Enemy;
import Character;
import EventBus;
import ConfigService;

namespace mc
{
    export constexpr int CARD_HEIGHT = 300;
    export constexpr int CARD_WIDTH = 200;
    export constexpr int HAND_SIZE_MAX = 4;

    export enum class ECardType
    {
        Null = -1,
        Rock = 0, Scissors = 1, Paper = 2, Magic = 3,
    };

    export EAttackType ToAttackType(ECardType type)
    {
        switch (type)
        {
        case ECardType::Rock: return EAttackType::Rock;
        case ECardType::Scissors: return EAttackType::Scissors;
        case ECardType::Paper: return EAttackType::Paper;
        default:
            throw std::invalid_argument("攻撃に指定されたカードの種類が無効");
        }
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
        virtual void Start() = 0;
        virtual Card DrawCard() = 0;
        virtual void DiscardHand() = 0;
        virtual const std::vector<Card>& GetHandCards() = 0;
        virtual const std::vector<Card>& GetDrawCards() = 0;
        virtual const std::vector<Card>& GetDiscardCards() = 0;
    };

    export struct DrawCardEvent : IEvent {};

    export struct ShuffleEvent : IEvent {};


    export struct DeckUpdatedEvent : IEvent
    {
        DeckUpdatedEvent(const size_t draw_pile_count, const size_t discard_pile_count)
            : drawPileCount(draw_pile_count),
              discardPileCount(discard_pile_count) {}

        const size_t drawPileCount;
        const size_t discardPileCount;
    };

    export struct HandUpdatedEvent : IEvent
    {
        HandUpdatedEvent(const std::vector<Card>& cards)
            : cards(cards) {}

        const std::vector<Card>& cards;
    };

    export constexpr Card CARD_ROCK_2 = {ECardType::Rock, 2};
    export constexpr Card CARD_ROCK_3 = {ECardType::Rock, 3};
    export constexpr Card CARD_ROCK_4 = {ECardType::Rock, 4};
    export constexpr Card CARD_SCISSORS_2 = {ECardType::Scissors, 2};
    export constexpr Card CARD_SCISSORS_3 = {ECardType::Scissors, 3};
    export constexpr Card CARD_SCISSORS_4 = {ECardType::Scissors, 4};
    export constexpr Card CARD_PAPER_2 = {ECardType::Paper, 2};
    export constexpr Card CARD_PAPER_3 = {ECardType::Paper, 3};
    export constexpr Card CARD_PAPER_4 = {ECardType::Paper, 4};
    export constexpr Card CARD_MAGIC_2 = {ECardType::Magic, 2};
    export constexpr Card CARD_MAGIC_3 = {ECardType::Magic, 3};
    export constexpr Card CARD_MAGIC_4 = {ECardType::Magic, 4};

    export Shared<ICardService> CreateCardService(IConfigService* configService);
} // namespace mc
