#pragma once
#include "DxLib.h"
#include <string>
#include <cstdint>

import RenderService;

namespace mc {
    // 基本的な色 (Basic Colors)
	static constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
    static constexpr uint32_t COLOR_BLACK = 0x000000;
	static constexpr uint32_t COLOR_RED = 0xFF0000;
	static constexpr uint32_t COLOR_GREEN = 0x00FF00;
	static constexpr uint32_t COLOR_BLUE = 0x0000FF;
	
    // 一般的なテキストの色 (Common text colors)
    static constexpr uint32_t COLOR_TEXT_NORMAL = 0xC8C8C8;
    static constexpr uint32_t COLOR_TEXT_BLUE = 0x96C8FF;
    static constexpr uint32_t COLOR_TEXT_RED = 0xFF9696;
    static constexpr uint32_t COLOR_TEXT_GREEN = 0xC8FFC8;

    // カードの色 (Card colors)
    static constexpr uint32_t COLOR_CARD_DEFAULT = 0x79D5EE;
    static constexpr uint32_t COLOR_CARD_ROCK = 0x555555;
    static constexpr uint32_t COLOR_CARD_PAPER = 0xF5F5DC;
    static constexpr uint32_t COLOR_CARD_SCISSORS = 0xB0C4DE;
	
    /**
     * @brief ウィンドウの幅を取得します。
     * @return ウィンドウの幅（ピクセル）
     */
	inline int GetWindowWidth() {
		static const int x = (int) dxe::GetWindowWidthF();
		return x;
	}
	
    /**
     * @brief ウィンドウの高さを取得します。
     * @return ウィンドウの高さ（ピクセル）
     */
	inline int GetWindowHeight() {
		static const int y = (int) dxe::GetWindowHeightF();
		return y;
	}

    /**
     * @brief 文字列を中央揃えで描画します。
     * @param renderer 描画サービス
     * @param x 中心となるX座標
     * @param y 中心となるY座標
     * @param text 描画する文字列
     * @param color 描画する色
     */
    inline void DrawCenterString(IRenderService& renderer, int x, int y, const std::wstring& text, uint32_t color)
    {
        int textWidth = renderer.GetDrawStringWidth(text.c_str());
        int textHeight = renderer.GetFontSize();

        int drawX = x - (textWidth / 2);
        int drawY = y - (textHeight / 2);

        renderer.DrawString(drawX, drawY, text.c_str(), color);
    }

    /**
     * @brief 文字列を右揃えで描画します。
     * @param renderer 描画サービス
     * @param x 右端となるX座標
     * @param y 上端となるY座標
     * @param text 描画する文字列
     * @param color 描画する色
     */
    inline void DrawRightString(IRenderService& renderer, int x, int y, const std::wstring& text, uint32_t color)
    {
        int textWidth = renderer.GetDrawStringWidth(text.c_str());

        int drawX = x - textWidth;
        int drawY = y;

        renderer.DrawString(drawX, drawY, text.c_str(), color);
    }

    /**
     * @brief 中空の矩形（枠線）を太さを指定して描画します。
     * @param renderer 描画サービス
     * @param x1 左上のX座標
     * @param y1 左上のY座標
     * @param x2 右下のX座標
     * @param y2 右下のY座標
     * @param thickness 枠線の太さ（ピクセル）
     * @param color 描画する色
     */
    inline void DrawHollowBox(IRenderService& renderer, float x1, float y1, float x2, float y2, int thickness, uint32_t color)
    {
        for (int k = 0; k < thickness; ++k)
        {
            renderer.DrawBoxAA(x1 + k, y1 + k, x2 - k, y2 - k, color, false);
        }
    }
}
