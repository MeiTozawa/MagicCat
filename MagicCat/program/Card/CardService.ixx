module;

#include <vector>

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
        int Power = 0;
    };

    /// @brief カード関連の管理（手札、山札、捨て札）を行うサービス
    export class ICardService
    {
    public:
        virtual ~ICardService() = default;

        /// @brief サービスを初期化し、初期デッキの構築やシャッフルを行う
        virtual void Start() = 0;

        /// @brief 山札からカードを1枚引く。山札が空の場合は捨て札をリシャッフルする
        /// @return 引いたカード。デッキが完全に空の場合はNullカードを返す
        virtual Card DrawCard() = 0;

        /// @brief 現在の手札をすべて捨て札に移動する
        virtual void DiscardHand() = 0;

        /// @brief 現在の手札のリストを取得する
        virtual std::vector<Card> GetHandCards() = 0;

        /// @brief 現在の山札のリストを取得する
        virtual std::vector<Card> GetDrawCards() = 0;

        /// @brief 現在の捨て札のリストを取得する
        virtual std::vector<Card> GetDiscardCards() = 0;
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
        explicit HandUpdatedEvent(std::vector<Card> cards)
            : cards(std::move(cards)) {}

        std::vector<Card> cards;
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

    export std::unique_ptr<ICardService> CreateCardService(IConfigService& configService);
} // namespace mc
