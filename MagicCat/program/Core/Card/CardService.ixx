module;

#include <string>
#include <vector>
#include <tnl_rect.h>

export module CardService;

import Enemy;
import EventBus;


export constexpr int CARD_HEIGHT = 300;
export constexpr int CARD_WIDTH = 200;

export enum ECardType
{
    Rock = 0, Scissors = 1, Paper = 2,
    Null = -1,
    Magic = 100
};

// ⚠️ 警告：伝達性が損なわれているため、ソートや関連付けコンテナには絶対に使用しないでください！
constexpr std::strong_ordering operator<=>(ECardType l, ECardType r)
{
    if (l == r) return std::strong_ordering::equal;
    if (l == ECardType::Null || r == ECardType::Null || l == ECardType::Magic || r == ECardType::Magic)
    {
        return static_cast<int>(l) <=> static_cast<int>(r);
    }

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
    int Offset = 0;
    bool is_selected = false;
};

export class ICardService
{
public:
    virtual ~ICardService() = default;
    virtual const std::vector<Card>& DrawCard() = 0;
    virtual const std::vector<Card>& GetHandCards() = 0;
    virtual const std::vector<Card>& GetDrawCards() = 0;
    virtual const std::vector<Card>& GetDiscardCards() = 0;
    virtual void PushBackRectOfCard(tnl::Rect) = 0;
    virtual const std::vector<tnl::Rect>& GetRectOfCards() = 0;
    virtual void ClearRectOfCards() = 0;
    virtual void MoveFocusToRight() = 0;
    virtual void MoveFocusToLeft() = 0;
    virtual void PlayCard() = 0;
};

export struct MoveFocusToLeftEvent : IEvent {};
export struct MoveFocusToRightEvent : IEvent {};
export struct PlayCardEvent : IEvent {};
export struct DrawCardEvent : IEvent {};
export struct HandUpdatedEvent : IEvent
{
    const std::vector<Card>& Hand;
    HandUpdatedEvent(const std::vector<Card>& hand) : Hand(hand) {}
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
