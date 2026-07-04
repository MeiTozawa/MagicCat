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
    
    static constexpr int FONT_SIZE = 48;  
    
    static constexpr int CARD_HEIGHT = 300;
    static constexpr int CARD_WIDTH = 200;
    
    static constexpr int ICON_SIZE_X = 128; 
    static constexpr int ICON_SIZE_Y = 128; 

    static constexpr int ACTION_MENU_X = 400;  ///< 行動ボタン左端 X
    static constexpr int ACTION_MENU_Y = 200;  ///< 行動ボタン最上段 Y
    static constexpr int ACTION_MENU_W = 300;  ///< 行動ボタン幅
    static constexpr int ACTION_MENU_H = 100;  ///< 行動ボタン高さ
    static constexpr int ACTION_MENU_STEP_Y = 120;  ///< 行動ボタン間の Y オフセット

    static constexpr int DRAW_PILE_X1 = 50;   ///< 山札領域 左端 X
    static constexpr int DRAW_PILE_Y1 = 400;  ///< 山札領域 上端 Y
    static constexpr int DRAW_PILE_X2 = DRAW_PILE_X1 + CARD_WIDTH;  ///< 山札領域 右端 X  
    static constexpr int DRAW_PILE_Y2 = DRAW_PILE_Y1 + CARD_HEIGHT;  ///< 山札領域 下端 Y  

    // -------------------------------------------------------------------------
    // メニューボタン HitBox（ControlDisplayer / CombatScene 共用）
    // menuIconX = windowWidth - MENU_ICON_X_OFFSET  （X はウィンドウ幅依存）
    // -------------------------------------------------------------------------
    static constexpr int MENU_ICON_X_OFFSET = 60;   ///< 右端からのオフセット
    static constexpr int MENU_ICON_Y        = 60;   ///< ボタン中心 Y
    static constexpr int MENU_ICON_HALF_W   = 40;   ///< 判定半幅 X
    static constexpr int MENU_ICON_HALF_H   = 28;   ///< 判定半幅 Y

    template <typename T>
    requires std::integral<T> || std::floating_point<T>
    struct Point
    {
        T x, y;
    };

}
