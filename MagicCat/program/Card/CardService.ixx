module;

#include <string>
#include <vector>

export module CardService;

export constexpr int CARD_HEIGHT = 300;
export constexpr int CARD_WIDTH = 200;


export enum ECardType {
    ROCK, SCISSORS, PAPER
};

export struct Card {
    ECardType CardType;
    // 敵が「ECardType」を出す確率のオフセット
    int Offset = 0; 
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