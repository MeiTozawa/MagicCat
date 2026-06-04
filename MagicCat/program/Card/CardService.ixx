module;

#include <string>
#include <vector>
#include <tnl_rect.h>

export module CardService;

export constexpr int CARD_HEIGHT = 300;
export constexpr int CARD_WIDTH = 200;


export enum ECardType
{
    ROCK = 0, SCISSORS = 1, PAPER = 2
};

// ⚠️ 警告：伝達性が損なわれているため、ソートや関連付けコンテナには絶対に使用しないでください！
constexpr std::strong_ordering operator<=>(ECardType l, ECardType r) {
    if (l == r) return std::strong_ordering::equal;

    int diff = (static_cast<int>(l) - static_cast<int>(r) + 3) % 3;
    
    if (diff == 1) {
        return std::strong_ordering::greater;
    } else {
        return std::strong_ordering::less;   
    }
}

constexpr bool operator==(ECardType l, ECardType r) {
    return (l <=> r) == std::strong_ordering::equal;
}


export struct Card
{
    ECardType CardType;
    // 敵が「ECardType」を出す確率のオフセット
    int Offset = 0;
};

export class ICardService
{
public:
    virtual ~ICardService() = default;
    virtual const std::vector<Card>& DrawCards(int count);
    virtual const std::vector<Card>& GetHandCards();
    virtual void PushBackRectOfCard(tnl::Rect);
    virtual const std::vector<tnl::Rect>& GetRectOfCards();
    virtual void ClearRectOfCards();
};


export constexpr Card CARD_ROCK_2 = {ROCK, 2};
export constexpr Card CARD_ROCK_3 = {ROCK, 3};
export constexpr Card CARD_ROCK_4 = {ROCK, 4};
export constexpr Card CARD_SCISSORS_2 = {SCISSORS, 2};
export constexpr Card CARD_SCISSORS_3 = {SCISSORS, 3};
export constexpr Card CARD_SCISSORS_4 = {SCISSORS, 4};
export constexpr Card CARD_PAPER_2 = {PAPER, 2};
export constexpr Card CARD_PAPER_3 = {PAPER, 3};
export constexpr Card CARD_PAPER_4 = {PAPER, 4};
