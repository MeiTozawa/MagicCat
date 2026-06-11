#pragma once
#include <wchar.h>
#include <vector>
#include <unordered_map>



//------------------------------------------------------------------------------------------------------------------
// 
// file path 
// 


constexpr const wchar_t* FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_JA = L"resource/font/ark-pixel-16px-proportional-ja.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_KO = L"resource/font/ark-pixel-16px-proportional-ko.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_LATIN = L"resource/font/ark-pixel-16px-proportional-latin.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_CN = L"resource/font/ark-pixel-16px-proportional-zh_cn.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_HK = L"resource/font/ark-pixel-16px-proportional-zh_hk.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TR = L"resource/font/ark-pixel-16px-proportional-zh_tr.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TW = L"resource/font/ark-pixel-16px-proportional-zh_tw.ttf"; 

constexpr const wchar_t* FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE = L"resource/Images/Keyboard/keyboard_arrow_down_outline.png"; 
constexpr const wchar_t* FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE = L"resource/Images/Keyboard/keyboard_arrow_up_outline.png"; 
constexpr const wchar_t* FILE_PATH_PNG_KEYBOARD_Q_OUTLINE = L"resource/Images/Keyboard/keyboard_q_outline.png"; 
constexpr const wchar_t* FILE_PATH_PNG_KEYBOARD_SPACE_OUTLINE = L"resource/Images/Keyboard/keyboard_space_outline.png"; 

constexpr const wchar_t* FILE_PATH_PNG_MINIBUNNY = L"resource/Images/MiniAnimals/MiniBunny.png"; 
constexpr const wchar_t* FILE_PATH_PNG_MINIWOLF = L"resource/Images/MiniAnimals/MiniWolf.png"; 

constexpr const wchar_t* FILE_PATH_PNG_PAPER = L"resource/Images/RPS/paper.png"; 
constexpr const wchar_t* FILE_PATH_PNG_POINT = L"resource/Images/RPS/point.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SCISSORS = L"resource/Images/RPS/scissors.png"; 
constexpr const wchar_t* FILE_PATH_PNG_STONE = L"resource/Images/RPS/stone.png"; 

constexpr const wchar_t* FILE_PATH_JSON_CARD_CONFIG = L"resource/json/card_config.json"; 
constexpr const wchar_t* FILE_PATH_JSON_ENEMY_CONFIG = L"resource/json/enemy_config.json"; 
constexpr const wchar_t* FILE_PATH_JSON_EXAMPLE = L"resource/json/example.json"; 



//------------------------------------------------------------------------------------------------------------------
// 
// file path table 
// 


const std::vector<const wchar_t*> FILE_PATH_TBL_FONT = {
    FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_JA,
    FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_KO,
    FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_LATIN,
    FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_CN,
    FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_HK,
    FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TR,
    FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TW,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_IMAGES_KEYBOARD = {
    FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE,
    FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE,
    FILE_PATH_PNG_KEYBOARD_Q_OUTLINE,
    FILE_PATH_PNG_KEYBOARD_SPACE_OUTLINE,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_IMAGES_MINIANIMALS = {
    FILE_PATH_PNG_MINIBUNNY,
    FILE_PATH_PNG_MINIWOLF,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_IMAGES_RPS = {
    FILE_PATH_PNG_PAPER,
    FILE_PATH_PNG_POINT,
    FILE_PATH_PNG_SCISSORS,
    FILE_PATH_PNG_STONE,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_JSON = {
    FILE_PATH_JSON_CARD_CONFIG,
    FILE_PATH_JSON_ENEMY_CONFIG,
    FILE_PATH_JSON_EXAMPLE,
};


//------------------------------------------------------------------------------------------------------------------
// 
// file path map 
// 


const std::unordered_map<std::wstring, const wchar_t*> FILE_PATH_DICTIONARY = { 
    { std::wstring( L"ark-pixel-16px-proportional-ja.ttf" ), FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_JA },
    { std::wstring( L"ark-pixel-16px-proportional-ko.ttf" ), FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_KO },
    { std::wstring( L"ark-pixel-16px-proportional-latin.ttf" ), FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_LATIN },
    { std::wstring( L"ark-pixel-16px-proportional-zh_cn.ttf" ), FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_CN },
    { std::wstring( L"ark-pixel-16px-proportional-zh_hk.ttf" ), FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_HK },
    { std::wstring( L"ark-pixel-16px-proportional-zh_tr.ttf" ), FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TR },
    { std::wstring( L"ark-pixel-16px-proportional-zh_tw.ttf" ), FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TW },
    { std::wstring( L"keyboard_arrow_down_outline.png" ), FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE },
    { std::wstring( L"keyboard_arrow_up_outline.png" ), FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE },
    { std::wstring( L"keyboard_q_outline.png" ), FILE_PATH_PNG_KEYBOARD_Q_OUTLINE },
    { std::wstring( L"keyboard_space_outline.png" ), FILE_PATH_PNG_KEYBOARD_SPACE_OUTLINE },
    { std::wstring( L"MiniBunny.png" ), FILE_PATH_PNG_MINIBUNNY },
    { std::wstring( L"MiniWolf.png" ), FILE_PATH_PNG_MINIWOLF },
    { std::wstring( L"paper.png" ), FILE_PATH_PNG_PAPER },
    { std::wstring( L"point.png" ), FILE_PATH_PNG_POINT },
    { std::wstring( L"scissors.png" ), FILE_PATH_PNG_SCISSORS },
    { std::wstring( L"stone.png" ), FILE_PATH_PNG_STONE },
    { std::wstring( L"card_config.json" ), FILE_PATH_JSON_CARD_CONFIG },
    { std::wstring( L"enemy_config.json" ), FILE_PATH_JSON_ENEMY_CONFIG },
    { std::wstring( L"example.json" ), FILE_PATH_JSON_EXAMPLE },
};


//------------------------------------------------------------------------------------------------------------------
// 
// file name 
// 


constexpr const wchar_t* FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_JA = L"ark-pixel-16px-proportional-ja.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_KO = L"ark-pixel-16px-proportional-ko.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_LATIN = L"ark-pixel-16px-proportional-latin.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_CN = L"ark-pixel-16px-proportional-zh_cn.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_HK = L"ark-pixel-16px-proportional-zh_hk.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TR = L"ark-pixel-16px-proportional-zh_tr.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TW = L"ark-pixel-16px-proportional-zh_tw.ttf"; 

constexpr const wchar_t* FILE_NAME_PNG_KEYBOARD_ARROW_DOWN_OUTLINE = L"keyboard_arrow_down_outline.png"; 
constexpr const wchar_t* FILE_NAME_PNG_KEYBOARD_ARROW_UP_OUTLINE = L"keyboard_arrow_up_outline.png"; 
constexpr const wchar_t* FILE_NAME_PNG_KEYBOARD_Q_OUTLINE = L"keyboard_q_outline.png"; 
constexpr const wchar_t* FILE_NAME_PNG_KEYBOARD_SPACE_OUTLINE = L"keyboard_space_outline.png"; 

constexpr const wchar_t* FILE_NAME_PNG_MINIBUNNY = L"MiniBunny.png"; 
constexpr const wchar_t* FILE_NAME_PNG_MINIWOLF = L"MiniWolf.png"; 

constexpr const wchar_t* FILE_NAME_PNG_PAPER = L"paper.png"; 
constexpr const wchar_t* FILE_NAME_PNG_POINT = L"point.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SCISSORS = L"scissors.png"; 
constexpr const wchar_t* FILE_NAME_PNG_STONE = L"stone.png"; 

constexpr const wchar_t* FILE_NAME_JSON_CARD_CONFIG = L"card_config.json"; 
constexpr const wchar_t* FILE_NAME_JSON_ENEMY_CONFIG = L"enemy_config.json"; 
constexpr const wchar_t* FILE_NAME_JSON_EXAMPLE = L"example.json"; 



//------------------------------------------------------------------------------------------------------------------
// 
// file name table 
// 


const std::vector<const wchar_t*> FILE_NAME_TBL_FONT = {
    FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_JA,
    FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_KO,
    FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_LATIN,
    FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_CN,
    FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_HK,
    FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TR,
    FILE_NAME_TTF_ARK_PIXEL_16PX_PROPORTIONAL_ZH_TW,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_IMAGES_KEYBOARD = {
    FILE_NAME_PNG_KEYBOARD_ARROW_DOWN_OUTLINE,
    FILE_NAME_PNG_KEYBOARD_ARROW_UP_OUTLINE,
    FILE_NAME_PNG_KEYBOARD_Q_OUTLINE,
    FILE_NAME_PNG_KEYBOARD_SPACE_OUTLINE,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_IMAGES_MINIANIMALS = {
    FILE_NAME_PNG_MINIBUNNY,
    FILE_NAME_PNG_MINIWOLF,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_IMAGES_RPS = {
    FILE_NAME_PNG_PAPER,
    FILE_NAME_PNG_POINT,
    FILE_NAME_PNG_SCISSORS,
    FILE_NAME_PNG_STONE,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_JSON = {
    FILE_NAME_JSON_CARD_CONFIG,
    FILE_NAME_JSON_ENEMY_CONFIG,
    FILE_NAME_JSON_EXAMPLE,
};


//------------------------------------------------------------------------------------------------------------------
// 
// font name 
// 


constexpr const wchar_t* FONT_NAME_ARK_PIXEL_16PX_PROP_JA = L"Ark Pixel 16px Prop ja"; 
constexpr const wchar_t* FONT_NAME_ARK_PIXEL_16PX_PROP_KO = L"Ark Pixel 16px Prop ko"; 
constexpr const wchar_t* FONT_NAME_ARK_PIXEL_16PX_PROP_LATIN = L"Ark Pixel 16px Prop latin"; 
constexpr const wchar_t* FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_CN = L"Ark Pixel 16px Prop zh_cn"; 
constexpr const wchar_t* FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_HK = L"Ark Pixel 16px Prop zh_hk"; 
constexpr const wchar_t* FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_TR = L"Ark Pixel 16px Prop zh_tr"; 
constexpr const wchar_t* FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_TW = L"Ark Pixel 16px Prop zh_tw"; 


//------------------------------------------------------------------------------------------------------------------
// 
// font name table 
// 


const std::vector<const wchar_t*> FONT_NAME_TABLE = { 
     FONT_NAME_ARK_PIXEL_16PX_PROP_JA,
     FONT_NAME_ARK_PIXEL_16PX_PROP_KO,
     FONT_NAME_ARK_PIXEL_16PX_PROP_LATIN,
     FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_CN,
     FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_HK,
     FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_TR,
     FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_TW,
};


//------------------------------------------------------------------------------------------------------------------
// 
// font name to path map 
// 


const std::unordered_map<std::wstring, const wchar_t*> FONT_NAME_TO_PATH_DICTIONARY = { 
     { std::wstring( FONT_NAME_ARK_PIXEL_16PX_PROP_JA ), L"resource/font/ark-pixel-16px-proportional-ja.ttf" },
     { std::wstring( FONT_NAME_ARK_PIXEL_16PX_PROP_KO ), L"resource/font/ark-pixel-16px-proportional-ko.ttf" },
     { std::wstring( FONT_NAME_ARK_PIXEL_16PX_PROP_LATIN ), L"resource/font/ark-pixel-16px-proportional-latin.ttf" },
     { std::wstring( FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_CN ), L"resource/font/ark-pixel-16px-proportional-zh_cn.ttf" },
     { std::wstring( FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_HK ), L"resource/font/ark-pixel-16px-proportional-zh_hk.ttf" },
     { std::wstring( FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_TR ), L"resource/font/ark-pixel-16px-proportional-zh_tr.ttf" },
     { std::wstring( FONT_NAME_ARK_PIXEL_16PX_PROP_ZH_TW ), L"resource/font/ark-pixel-16px-proportional-zh_tw.ttf" },
};
