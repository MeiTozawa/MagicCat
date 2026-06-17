#pragma once
#include "DxLib.h"
#include <string>
#include <cstdint>

namespace mc {
	static constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
    static constexpr uint32_t COLOR_BLACK = 0x000000;
	static constexpr uint32_t COLOR_RED = 0xFF0000;
	static constexpr uint32_t COLOR_GREEN = 0x00FF00;
	static constexpr uint32_t COLOR_BLUE = 0x0000FF;
	
    // Common text colors
    static constexpr uint32_t COLOR_TEXT_NORMAL = 0xC8C8C8;
    static constexpr uint32_t COLOR_TEXT_BLUE = 0x96C8FF;
    static constexpr uint32_t COLOR_TEXT_RED = 0xFF9696;
    static constexpr uint32_t COLOR_TEXT_GREEN = 0xC8FFC8;

    // Card colors
    static constexpr uint32_t COLOR_CARD_DEFAULT = 0x79D5EE;
    static constexpr uint32_t COLOR_CARD_ROCK = 0x555555;
    static constexpr uint32_t COLOR_CARD_PAPER = 0xF5F5DC;
    static constexpr uint32_t COLOR_CARD_SCISSORS = 0xB0C4DE;
	
	static int GetWindowX() {
		static int x = (int) dxe::GetWindowWidthF();
		return x;
	}
	
	static int GetWindowY() {
		static int y = (int) dxe::GetWindowWidthF();
		return y;
	}

    inline void DrawCenterString(int x, int y, const std::wstring& text, uint32_t color)
    {
        int textWidth = GetDrawStringWidth(text.c_str(), -1);

        int textHeight = GetFontSize();

        int drawX = x - (textWidth / 2);
        int drawY = y - (textHeight / 2);

        DrawString(drawX, drawY, text.c_str(), color);
    }

    inline void DrawRightString(int x, int y, const std::wstring& text, uint32_t color)
    {
        int textWidth = GetDrawStringWidth(text.c_str(), -1);

        int drawX = x - textWidth;
        int drawY = y;

        DrawString(drawX, drawY, text.c_str(), color);
    }

}