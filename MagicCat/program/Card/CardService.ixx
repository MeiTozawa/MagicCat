module;

#include <string>

export module CardService;

export constexpr float CARD_HEIGHT = 300;
export constexpr float CARD_WIDTH = 200;

export enum ECardType {
    ROCK, PAPER, SCISSORS
};

export struct Card {
    ECardType CardType;
    // 敵が「ECardType」を出す確率のオフセット
    int Offset = 0;          
};
