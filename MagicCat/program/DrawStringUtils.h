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
	
	inline int GetWindowWidth() {
		static const int x = (int) dxe::GetWindowWidthF();
		return x;
	}
	
	inline int GetWindowHeight() {
		static const int y = (int) dxe::GetWindowHeightF();
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

    /**
     * @brief 中空の矩形（枠線）を太さを指定して描画します。
     * @param x1 左上のX座標
     * @param y1 左上のY座標
     * @param x2 右下のX座標
     * @param y2 右下のY座標
     * @param thickness 枠線の太さ（ピクセル）
     * @param color 描画する色
     */
    inline void DrawHollowBox(float x1, float y1, float x2, float y2, int thickness, uint32_t color)
    {
        for (int k = 0; k < thickness; ++k)
        {
            DrawBoxAA(x1 + k, y1 + k, x2 - k, y2 - k, color, FALSE);
        }
    }
}