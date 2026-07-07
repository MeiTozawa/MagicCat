module;

#include <vector>

export module CardService;

import Enemy;
import Character;
import EventBus;
import ConfigService;

namespace mc {

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

    /// @brief カードのデータ（種類とパワー値）を表す構造体
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
        /// @return 手札のカードリスト
        virtual std::vector<Card> GetHandCards() = 0;

        /// @brief 現在の山札のリストを取得する
        /// @return 山札のカードリスト
        virtual std::vector<Card> GetDrawCards() = 0;

        /// @brief 現在の捨て札のリストを取得する
        /// @return 捨て札のカードリスト
        virtual std::vector<Card> GetDiscardCards() = 0;
    };

    /// @brief プレイヤーがカードを引いたときに発行されるイベント
    export struct DrawCardEvent : IEvent {};

    /// @brief 捨て札から山札へのリシャッフルが発生したときに発行されるイベント
    export struct ShuffleEvent : IEvent {};

    /// @brief 山札または捨て札の枚数が更新されたときに発行されるイベント
    export struct DeckUpdatedEvent : IEvent
    {
        DeckUpdatedEvent(const size_t draw_pile_count, const size_t discard_pile_count)
            : drawPileCount(draw_pile_count),
              discardPileCount(discard_pile_count) {}

        const size_t drawPileCount;
        const size_t discardPileCount;
    };

    /// @brief 手札が更新されたときに発行されるイベント
    export struct HandUpdatedEvent : IEvent
    {
        explicit HandUpdatedEvent(std::vector<Card> cards)
            : cards(std::move(cards)) {}

        std::vector<Card> cards;
    };

    export std::unique_ptr<ICardService> CreateCardService(IConfigService& configService);
} // namespace mc
