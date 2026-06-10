#pragma once
#include <wchar.h>
#include <vector>
#include <unordered_map>



//------------------------------------------------------------------------------------------------------------------
// 
// file path 
// 


constexpr const wchar_t* FILE_PATH_OTF_03TAKIBI_REGULAR = L"resource/font/03Takibi-Regular.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_06TAKIBI_BOLD = L"resource/font/06Takibi-Bold.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_AKABARA_CINDERELLA = L"resource/font/akabara-cinderella.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_APPLIMINCHO = L"resource/font/AppliMincho.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3 = L"resource/font/Corporate-Logo-Rounded-Bold-ver3.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_CORPORATE_MINCHO_VER3 = L"resource/font/Corporate-Mincho-ver3.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_DIGITALISM = L"resource/font/digitalism.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_DS_DIGIB = L"resource/font/DS-DIGIB.TTF"; 
constexpr const wchar_t* FILE_PATH_TTF_GENKAI_MINCHO = L"resource/font/genkai-mincho.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_HANGYAKU_XRRWQ = L"resource/font/Hangyaku-xRRwq.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_HARENOSORA = L"resource/font/Harenosora.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_HONOKA_SHIN_MARU_GOTHIC_R = L"resource/font/Honoka-Shin-Maru-Gothic_R.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_HONOKA_SHIN_MINCHO_L = L"resource/font/Honoka_Shin_Mincho_L.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_IPAEXM = L"resource/font/ipaexm.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_KOWAIFONT = L"resource/font/kowaiFont.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_KUROBARA_CINDERELLA = L"resource/font/kurobara-cinderella.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_LED_DIGITAL_7 = L"resource/font/led_digital_7.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_LIGHTNOVELPOPV2 = L"resource/font/LightNovelPOPv2.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_MAGICRING = L"resource/font/MagicRing.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS = L"resource/font/Mattsun-Marus_samurai_spirits.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_MOBILEFONT = L"resource/font/MobileFont.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT = L"resource/font/NagomiGokubosoGothic-ExtraLight.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_NEKO_NO_MEZAME = L"resource/font/Neko_no_Mezame.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_OTOMANOPEEONE_REGULAR = L"resource/font/OtomanopeeOne-Regular.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_PENGS = L"resource/font/PENGS.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_PLANETARIUM = L"resource/font/Planetarium.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_SAWARABIGOTHIC_REGULAR = L"resource/font/SawarabiGothic-Regular.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_SENOBI_GOTHIC_REGULAR = L"resource/font/Senobi-Gothic-Regular.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_SMARTFONTUI = L"resource/font/SmartFontUI.otf"; 
constexpr const wchar_t* FILE_PATH_OTF_SOLID_REGULAR = L"resource/font/Solid-Regular.otf"; 
constexpr const wchar_t* FILE_PATH_TTF_SOUKOUMINCHO = L"resource/font/SoukouMincho.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_TIMEMACHINE_WA = L"resource/font/timemachine-wa.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_TSUNAGIGOTHIC = L"resource/font/TsunagiGothic.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_UMEBOSHI_ = L"resource/font/umeboshi_.ttf"; 
constexpr const wchar_t* FILE_PATH_TTF_VOYNICH_MONOSPACE = L"resource/font/Voynich_MonoSpace.ttf"; 
constexpr const wchar_t* FILE_PATH_OTF_ZEROGOTHIC = L"resource/font/ZeroGothic.otf"; 

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
    FILE_PATH_OTF_03TAKIBI_REGULAR,
    FILE_PATH_OTF_06TAKIBI_BOLD,
    FILE_PATH_TTF_AKABARA_CINDERELLA,
    FILE_PATH_OTF_APPLIMINCHO,
    FILE_PATH_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3,
    FILE_PATH_OTF_CORPORATE_MINCHO_VER3,
    FILE_PATH_TTF_DIGITALISM,
    FILE_PATH_TTF_DS_DIGIB,
    FILE_PATH_TTF_GENKAI_MINCHO,
    FILE_PATH_TTF_HANGYAKU_XRRWQ,
    FILE_PATH_OTF_HARENOSORA,
    FILE_PATH_OTF_HONOKA_SHIN_MARU_GOTHIC_R,
    FILE_PATH_OTF_HONOKA_SHIN_MINCHO_L,
    FILE_PATH_TTF_IPAEXM,
    FILE_PATH_OTF_KOWAIFONT,
    FILE_PATH_TTF_KUROBARA_CINDERELLA,
    FILE_PATH_TTF_LED_DIGITAL_7,
    FILE_PATH_OTF_LIGHTNOVELPOPV2,
    FILE_PATH_TTF_MAGICRING,
    FILE_PATH_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS,
    FILE_PATH_TTF_MOBILEFONT,
    FILE_PATH_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT,
    FILE_PATH_TTF_NEKO_NO_MEZAME,
    FILE_PATH_TTF_OTOMANOPEEONE_REGULAR,
    FILE_PATH_TTF_PENGS,
    FILE_PATH_OTF_PLANETARIUM,
    FILE_PATH_TTF_SAWARABIGOTHIC_REGULAR,
    FILE_PATH_TTF_SENOBI_GOTHIC_REGULAR,
    FILE_PATH_OTF_SMARTFONTUI,
    FILE_PATH_OTF_SOLID_REGULAR,
    FILE_PATH_TTF_SOUKOUMINCHO,
    FILE_PATH_TTF_TIMEMACHINE_WA,
    FILE_PATH_TTF_TSUNAGIGOTHIC,
    FILE_PATH_TTF_UMEBOSHI_,
    FILE_PATH_TTF_VOYNICH_MONOSPACE,
    FILE_PATH_OTF_ZEROGOTHIC,
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
    { std::wstring( L"03Takibi-Regular.otf" ), FILE_PATH_OTF_03TAKIBI_REGULAR },
    { std::wstring( L"06Takibi-Bold.otf" ), FILE_PATH_OTF_06TAKIBI_BOLD },
    { std::wstring( L"akabara-cinderella.ttf" ), FILE_PATH_TTF_AKABARA_CINDERELLA },
    { std::wstring( L"AppliMincho.otf" ), FILE_PATH_OTF_APPLIMINCHO },
    { std::wstring( L"Corporate-Logo-Rounded-Bold-ver3.otf" ), FILE_PATH_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3 },
    { std::wstring( L"Corporate-Mincho-ver3.otf" ), FILE_PATH_OTF_CORPORATE_MINCHO_VER3 },
    { std::wstring( L"digitalism.ttf" ), FILE_PATH_TTF_DIGITALISM },
    { std::wstring( L"DS-DIGIB.TTF" ), FILE_PATH_TTF_DS_DIGIB },
    { std::wstring( L"genkai-mincho.ttf" ), FILE_PATH_TTF_GENKAI_MINCHO },
    { std::wstring( L"Hangyaku-xRRwq.ttf" ), FILE_PATH_TTF_HANGYAKU_XRRWQ },
    { std::wstring( L"Harenosora.otf" ), FILE_PATH_OTF_HARENOSORA },
    { std::wstring( L"Honoka-Shin-Maru-Gothic_R.otf" ), FILE_PATH_OTF_HONOKA_SHIN_MARU_GOTHIC_R },
    { std::wstring( L"Honoka_Shin_Mincho_L.otf" ), FILE_PATH_OTF_HONOKA_SHIN_MINCHO_L },
    { std::wstring( L"ipaexm.ttf" ), FILE_PATH_TTF_IPAEXM },
    { std::wstring( L"kowaiFont.otf" ), FILE_PATH_OTF_KOWAIFONT },
    { std::wstring( L"kurobara-cinderella.ttf" ), FILE_PATH_TTF_KUROBARA_CINDERELLA },
    { std::wstring( L"led_digital_7.ttf" ), FILE_PATH_TTF_LED_DIGITAL_7 },
    { std::wstring( L"LightNovelPOPv2.otf" ), FILE_PATH_OTF_LIGHTNOVELPOPV2 },
    { std::wstring( L"MagicRing.ttf" ), FILE_PATH_TTF_MAGICRING },
    { std::wstring( L"Mattsun-Marus_samurai_spirits.ttf" ), FILE_PATH_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS },
    { std::wstring( L"MobileFont.ttf" ), FILE_PATH_TTF_MOBILEFONT },
    { std::wstring( L"NagomiGokubosoGothic-ExtraLight.otf" ), FILE_PATH_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT },
    { std::wstring( L"Neko_no_Mezame.ttf" ), FILE_PATH_TTF_NEKO_NO_MEZAME },
    { std::wstring( L"OtomanopeeOne-Regular.ttf" ), FILE_PATH_TTF_OTOMANOPEEONE_REGULAR },
    { std::wstring( L"PENGS.ttf" ), FILE_PATH_TTF_PENGS },
    { std::wstring( L"Planetarium.otf" ), FILE_PATH_OTF_PLANETARIUM },
    { std::wstring( L"SawarabiGothic-Regular.ttf" ), FILE_PATH_TTF_SAWARABIGOTHIC_REGULAR },
    { std::wstring( L"Senobi-Gothic-Regular.ttf" ), FILE_PATH_TTF_SENOBI_GOTHIC_REGULAR },
    { std::wstring( L"SmartFontUI.otf" ), FILE_PATH_OTF_SMARTFONTUI },
    { std::wstring( L"Solid-Regular.otf" ), FILE_PATH_OTF_SOLID_REGULAR },
    { std::wstring( L"SoukouMincho.ttf" ), FILE_PATH_TTF_SOUKOUMINCHO },
    { std::wstring( L"timemachine-wa.ttf" ), FILE_PATH_TTF_TIMEMACHINE_WA },
    { std::wstring( L"TsunagiGothic.ttf" ), FILE_PATH_TTF_TSUNAGIGOTHIC },
    { std::wstring( L"umeboshi_.ttf" ), FILE_PATH_TTF_UMEBOSHI_ },
    { std::wstring( L"Voynich_MonoSpace.ttf" ), FILE_PATH_TTF_VOYNICH_MONOSPACE },
    { std::wstring( L"ZeroGothic.otf" ), FILE_PATH_OTF_ZEROGOTHIC },
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


constexpr const wchar_t* FILE_NAME_OTF_03TAKIBI_REGULAR = L"03Takibi-Regular.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_06TAKIBI_BOLD = L"06Takibi-Bold.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_AKABARA_CINDERELLA = L"akabara-cinderella.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_APPLIMINCHO = L"AppliMincho.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3 = L"Corporate-Logo-Rounded-Bold-ver3.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_CORPORATE_MINCHO_VER3 = L"Corporate-Mincho-ver3.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_DIGITALISM = L"digitalism.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_DS_DIGIB = L"DS-DIGIB.TTF"; 
constexpr const wchar_t* FILE_NAME_TTF_GENKAI_MINCHO = L"genkai-mincho.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_HANGYAKU_XRRWQ = L"Hangyaku-xRRwq.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_HARENOSORA = L"Harenosora.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_HONOKA_SHIN_MARU_GOTHIC_R = L"Honoka-Shin-Maru-Gothic_R.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_HONOKA_SHIN_MINCHO_L = L"Honoka_Shin_Mincho_L.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_IPAEXM = L"ipaexm.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_KOWAIFONT = L"kowaiFont.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_KUROBARA_CINDERELLA = L"kurobara-cinderella.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_LED_DIGITAL_7 = L"led_digital_7.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_LIGHTNOVELPOPV2 = L"LightNovelPOPv2.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_MAGICRING = L"MagicRing.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS = L"Mattsun-Marus_samurai_spirits.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_MOBILEFONT = L"MobileFont.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT = L"NagomiGokubosoGothic-ExtraLight.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_NEKO_NO_MEZAME = L"Neko_no_Mezame.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_OTOMANOPEEONE_REGULAR = L"OtomanopeeOne-Regular.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_PENGS = L"PENGS.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_PLANETARIUM = L"Planetarium.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_SAWARABIGOTHIC_REGULAR = L"SawarabiGothic-Regular.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_SENOBI_GOTHIC_REGULAR = L"Senobi-Gothic-Regular.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_SMARTFONTUI = L"SmartFontUI.otf"; 
constexpr const wchar_t* FILE_NAME_OTF_SOLID_REGULAR = L"Solid-Regular.otf"; 
constexpr const wchar_t* FILE_NAME_TTF_SOUKOUMINCHO = L"SoukouMincho.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_TIMEMACHINE_WA = L"timemachine-wa.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_TSUNAGIGOTHIC = L"TsunagiGothic.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_UMEBOSHI_ = L"umeboshi_.ttf"; 
constexpr const wchar_t* FILE_NAME_TTF_VOYNICH_MONOSPACE = L"Voynich_MonoSpace.ttf"; 
constexpr const wchar_t* FILE_NAME_OTF_ZEROGOTHIC = L"ZeroGothic.otf"; 

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
    FILE_NAME_OTF_03TAKIBI_REGULAR,
    FILE_NAME_OTF_06TAKIBI_BOLD,
    FILE_NAME_TTF_AKABARA_CINDERELLA,
    FILE_NAME_OTF_APPLIMINCHO,
    FILE_NAME_OTF_CORPORATE_LOGO_ROUNDED_BOLD_VER3,
    FILE_NAME_OTF_CORPORATE_MINCHO_VER3,
    FILE_NAME_TTF_DIGITALISM,
    FILE_NAME_TTF_DS_DIGIB,
    FILE_NAME_TTF_GENKAI_MINCHO,
    FILE_NAME_TTF_HANGYAKU_XRRWQ,
    FILE_NAME_OTF_HARENOSORA,
    FILE_NAME_OTF_HONOKA_SHIN_MARU_GOTHIC_R,
    FILE_NAME_OTF_HONOKA_SHIN_MINCHO_L,
    FILE_NAME_TTF_IPAEXM,
    FILE_NAME_OTF_KOWAIFONT,
    FILE_NAME_TTF_KUROBARA_CINDERELLA,
    FILE_NAME_TTF_LED_DIGITAL_7,
    FILE_NAME_OTF_LIGHTNOVELPOPV2,
    FILE_NAME_TTF_MAGICRING,
    FILE_NAME_TTF_MATTSUN_MARUS_SAMURAI_SPIRITS,
    FILE_NAME_TTF_MOBILEFONT,
    FILE_NAME_OTF_NAGOMIGOKUBOSOGOTHIC_EXTRALIGHT,
    FILE_NAME_TTF_NEKO_NO_MEZAME,
    FILE_NAME_TTF_OTOMANOPEEONE_REGULAR,
    FILE_NAME_TTF_PENGS,
    FILE_NAME_OTF_PLANETARIUM,
    FILE_NAME_TTF_SAWARABIGOTHIC_REGULAR,
    FILE_NAME_TTF_SENOBI_GOTHIC_REGULAR,
    FILE_NAME_OTF_SMARTFONTUI,
    FILE_NAME_OTF_SOLID_REGULAR,
    FILE_NAME_TTF_SOUKOUMINCHO,
    FILE_NAME_TTF_TIMEMACHINE_WA,
    FILE_NAME_TTF_TSUNAGIGOTHIC,
    FILE_NAME_TTF_UMEBOSHI_,
    FILE_NAME_TTF_VOYNICH_MONOSPACE,
    FILE_NAME_OTF_ZEROGOTHIC,
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


constexpr const wchar_t* FONT_NAME_03TAKIBI_REGULAR = L"03Takibi-Regular"; 
constexpr const wchar_t* FONT_NAME_06TAKIBI_BOLD = L"06Takibi-Bold"; 
constexpr const wchar_t* FONT_NAME_AKABARA_CINDERELLA = L"akabara-cinderella"; 
constexpr const wchar_t* FONT_NAME_APPLIMINCHO = L"AppliMincho"; 
constexpr const wchar_t* FONT_NAME_CORPORATE_LOGO_ROUNDED_VER3 = L"Corporate Logo Rounded ver3"; 
constexpr const wchar_t* FONT_NAME_CORPORATE_MINCHO_VER3 = L"Corporate Mincho ver3"; 
constexpr const wchar_t* FONT_NAME_DIGITALISM = L"Digitalism"; 
constexpr const wchar_t* FONT_NAME_GENKAIMINCHO = L"Genkaimincho"; 
constexpr const wchar_t* FONT_NAME_HANGYAKU = L"Hangyaku"; 
constexpr const wchar_t* FONT_NAME_HARENOSORAMINCHO = L"HarenosoraMincho"; 
constexpr const wchar_t* FONT_NAME_HONOKA_SHIN_MARU_GOTHIC = L"Honoka Shin Maru-Gothic"; 
constexpr const wchar_t* FONT_NAME_HONOKA_SHIN_MINCHO = L"Honoka Shin Mincho"; 
constexpr const wchar_t* FONT_NAME_IPAEXMINCHO = L"IPAexMincho"; 
constexpr const wchar_t* FONT_NAME_07REALLYSCARYMINCHOTAI = L"07ReallyScaryMinchotai"; 
constexpr const wchar_t* FONT_NAME_KUROBARA_CINDERELLA = L"kurobara-cinderella"; 
constexpr const wchar_t* FONT_NAME_LED_DIGITAL_7 = L"LED Digital 7"; 
constexpr const wchar_t* FONT_NAME_LIGHTNOVELPOPV2 = L"LightNovelPopV2"; 
constexpr const wchar_t* FONT_NAME_MAGICRING = L"MagicRing"; 
constexpr const wchar_t* FONT_NAME_SAMURAI_SPIRITS = L"samurai_spirits"; 
constexpr const wchar_t* FONT_NAME_MOBILE_FONT = L"Mobile Font"; 
constexpr const wchar_t* FONT_NAME_NAGOMI_GOKUBOSO_GOTHIC = L"Nagomi Gokuboso Gothic"; 
constexpr const wchar_t* FONT_NAME_NEKO_NO_MEZAME = L"Neko_no_Mezame"; 
constexpr const wchar_t* FONT_NAME_OTOMANOPEE_ONE = L"Otomanopee One"; 
constexpr const wchar_t* FONT_NAME_PENGS = L"PENGS"; 
constexpr const wchar_t* FONT_NAME_PLANETARIUM = L"Planetarium"; 
constexpr const wchar_t* FONT_NAME_SAWARABI_GOTHIC = L"Sawarabi Gothic"; 
constexpr const wchar_t* FONT_NAME_SENOBI_GOTHIC = L"Senobi Gothic"; 
constexpr const wchar_t* FONT_NAME_03SMARTFONTUI = L"03SmartFontUI"; 
constexpr const wchar_t* FONT_NAME_SOLID = L"Solid"; 
constexpr const wchar_t* FONT_NAME_SOUKOUMINCHO = L"SoukouMincho"; 
constexpr const wchar_t* FONT_NAME_TIMEMACHINE_WA = L"timemachine wa"; 
constexpr const wchar_t* FONT_NAME_TSUNAGI_GOTHIC_BLACK = L"Tsunagi Gothic Black"; 
constexpr const wchar_t* FONT_NAME_UMEBOSHI = L"umeboshi"; 
constexpr const wchar_t* FONT_NAME_VOYNICH_SPACE = L"Voynich_Space"; 
constexpr const wchar_t* FONT_NAME_ZEROGOTHIC = L"ZeroGothic"; 


//------------------------------------------------------------------------------------------------------------------
// 
// font name table 
// 


const std::vector<const wchar_t*> FONT_NAME_TABLE = { 
     FONT_NAME_03TAKIBI_REGULAR,
     FONT_NAME_06TAKIBI_BOLD,
     FONT_NAME_AKABARA_CINDERELLA,
     FONT_NAME_APPLIMINCHO,
     FONT_NAME_CORPORATE_LOGO_ROUNDED_VER3,
     FONT_NAME_CORPORATE_MINCHO_VER3,
     FONT_NAME_DIGITALISM,
     FONT_NAME_GENKAIMINCHO,
     FONT_NAME_HANGYAKU,
     FONT_NAME_HARENOSORAMINCHO,
     FONT_NAME_HONOKA_SHIN_MARU_GOTHIC,
     FONT_NAME_HONOKA_SHIN_MINCHO,
     FONT_NAME_IPAEXMINCHO,
     FONT_NAME_07REALLYSCARYMINCHOTAI,
     FONT_NAME_KUROBARA_CINDERELLA,
     FONT_NAME_LED_DIGITAL_7,
     FONT_NAME_LIGHTNOVELPOPV2,
     FONT_NAME_MAGICRING,
     FONT_NAME_SAMURAI_SPIRITS,
     FONT_NAME_MOBILE_FONT,
     FONT_NAME_NAGOMI_GOKUBOSO_GOTHIC,
     FONT_NAME_NEKO_NO_MEZAME,
     FONT_NAME_OTOMANOPEE_ONE,
     FONT_NAME_PENGS,
     FONT_NAME_PLANETARIUM,
     FONT_NAME_SAWARABI_GOTHIC,
     FONT_NAME_SENOBI_GOTHIC,
     FONT_NAME_03SMARTFONTUI,
     FONT_NAME_SOLID,
     FONT_NAME_SOUKOUMINCHO,
     FONT_NAME_TIMEMACHINE_WA,
     FONT_NAME_TSUNAGI_GOTHIC_BLACK,
     FONT_NAME_UMEBOSHI,
     FONT_NAME_VOYNICH_SPACE,
     FONT_NAME_ZEROGOTHIC,
};


//------------------------------------------------------------------------------------------------------------------
// 
// font name to path map 
// 


const std::unordered_map<std::wstring, const wchar_t*> FONT_NAME_TO_PATH_DICTIONARY = { 
     { std::wstring( FONT_NAME_03TAKIBI_REGULAR ), L"resource/font/03Takibi-Regular.otf" },
     { std::wstring( FONT_NAME_06TAKIBI_BOLD ), L"resource/font/06Takibi-Bold.otf" },
     { std::wstring( FONT_NAME_AKABARA_CINDERELLA ), L"resource/font/akabara-cinderella.ttf" },
     { std::wstring( FONT_NAME_APPLIMINCHO ), L"resource/font/AppliMincho.otf" },
     { std::wstring( FONT_NAME_CORPORATE_LOGO_ROUNDED_VER3 ), L"resource/font/Corporate-Logo-Rounded-Bold-ver3.otf" },
     { std::wstring( FONT_NAME_CORPORATE_MINCHO_VER3 ), L"resource/font/Corporate-Mincho-ver3.otf" },
     { std::wstring( FONT_NAME_DIGITALISM ), L"resource/font/digitalism.ttf" },
     { std::wstring( FONT_NAME_GENKAIMINCHO ), L"resource/font/genkai-mincho.ttf" },
     { std::wstring( FONT_NAME_HANGYAKU ), L"resource/font/Hangyaku-xRRwq.ttf" },
     { std::wstring( FONT_NAME_HARENOSORAMINCHO ), L"resource/font/Harenosora.otf" },
     { std::wstring( FONT_NAME_HONOKA_SHIN_MARU_GOTHIC ), L"resource/font/Honoka-Shin-Maru-Gothic_R.otf" },
     { std::wstring( FONT_NAME_HONOKA_SHIN_MINCHO ), L"resource/font/Honoka_Shin_Mincho_L.otf" },
     { std::wstring( FONT_NAME_IPAEXMINCHO ), L"resource/font/ipaexm.ttf" },
     { std::wstring( FONT_NAME_07REALLYSCARYMINCHOTAI ), L"resource/font/kowaiFont.otf" },
     { std::wstring( FONT_NAME_KUROBARA_CINDERELLA ), L"resource/font/kurobara-cinderella.ttf" },
     { std::wstring( FONT_NAME_LED_DIGITAL_7 ), L"resource/font/led_digital_7.ttf" },
     { std::wstring( FONT_NAME_LIGHTNOVELPOPV2 ), L"resource/font/LightNovelPOPv2.otf" },
     { std::wstring( FONT_NAME_MAGICRING ), L"resource/font/MagicRing.ttf" },
     { std::wstring( FONT_NAME_SAMURAI_SPIRITS ), L"resource/font/Mattsun-Marus_samurai_spirits.ttf" },
     { std::wstring( FONT_NAME_MOBILE_FONT ), L"resource/font/MobileFont.ttf" },
     { std::wstring( FONT_NAME_NAGOMI_GOKUBOSO_GOTHIC ), L"resource/font/NagomiGokubosoGothic-ExtraLight.otf" },
     { std::wstring( FONT_NAME_NEKO_NO_MEZAME ), L"resource/font/Neko_no_Mezame.ttf" },
     { std::wstring( FONT_NAME_OTOMANOPEE_ONE ), L"resource/font/OtomanopeeOne-Regular.ttf" },
     { std::wstring( FONT_NAME_PENGS ), L"resource/font/PENGS.ttf" },
     { std::wstring( FONT_NAME_PLANETARIUM ), L"resource/font/Planetarium.otf" },
     { std::wstring( FONT_NAME_SAWARABI_GOTHIC ), L"resource/font/SawarabiGothic-Regular.ttf" },
     { std::wstring( FONT_NAME_SENOBI_GOTHIC ), L"resource/font/Senobi-Gothic-Regular.ttf" },
     { std::wstring( FONT_NAME_03SMARTFONTUI ), L"resource/font/SmartFontUI.otf" },
     { std::wstring( FONT_NAME_SOLID ), L"resource/font/Solid-Regular.otf" },
     { std::wstring( FONT_NAME_SOUKOUMINCHO ), L"resource/font/SoukouMincho.ttf" },
     { std::wstring( FONT_NAME_TIMEMACHINE_WA ), L"resource/font/timemachine-wa.ttf" },
     { std::wstring( FONT_NAME_TSUNAGI_GOTHIC_BLACK ), L"resource/font/TsunagiGothic.ttf" },
     { std::wstring( FONT_NAME_UMEBOSHI ), L"resource/font/umeboshi_.ttf" },
     { std::wstring( FONT_NAME_VOYNICH_SPACE ), L"resource/font/Voynich_MonoSpace.ttf" },
     { std::wstring( FONT_NAME_ZEROGOTHIC ), L"resource/font/ZeroGothic.otf" },
};
