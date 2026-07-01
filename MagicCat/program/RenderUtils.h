#pragma once
#include <cstdint>
#include <concepts>

namespace mc {

    static constexpr uint32_t COLOR_BG = 0x071F38;

    static constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
    static constexpr uint32_t COLOR_BLACK = 0x000000;
    static constexpr uint32_t COLOR_RED = 0xFF0000;
    static constexpr uint32_t COLOR_GREEN = 0x00FF00;
    static constexpr uint32_t COLOR_BLUE = 0x0000FF;
    static constexpr uint32_t COLOR_GRAY = 0x808080;

    static constexpr uint32_t COLOR_TEXT_NORMAL = 0xC8C8C8;
    static constexpr uint32_t COLOR_TEXT_BLUE = 0x96C8FF;
    static constexpr uint32_t COLOR_TEXT_RED = 0xFF9696;
    static constexpr uint32_t COLOR_TEXT_GREEN = 0xC8FFC8;

    static constexpr uint32_t COLOR_CARD_DEFAULT = 0x79D5EE;
    static constexpr uint32_t COLOR_CARD_ROCK = 0x555555;
    static constexpr uint32_t COLOR_CARD_PAPER = 0xF5F5DC;
    static constexpr uint32_t COLOR_CARD_SCISSORS = 0xB0C4DE;
    static constexpr uint32_t COLOR_CARD_MAGIC = 0x9370DB;

    template <typename T>
    requires std::integral<T> || std::floating_point<T>
    struct Point
    {
        T x, y;
    };

}
