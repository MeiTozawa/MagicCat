#pragma once
#include <wchar.h>
#include <vector>
#include <unordered_map>



//------------------------------------------------------------------------------------------------------------------
// 
// file path 
// 


constexpr const wchar_t* FILE_PATH_OTF_UNIFONT_17 = L"resource/Fonts/unifont-17.otf"; 

constexpr const wchar_t* FILE_PATH_PNG_CLUCKINGCHICKEN = L"resource/Images/BasicAnimals/CluckingChicken.png"; 
constexpr const wchar_t* FILE_PATH_PNG_CORALCRAB = L"resource/Images/BasicAnimals/CoralCrab.png"; 
constexpr const wchar_t* FILE_PATH_PNG_CROAKINGTOAD = L"resource/Images/BasicAnimals/CroakingToad.png"; 
constexpr const wchar_t* FILE_PATH_PNG_DAINTYPIG = L"resource/Images/BasicAnimals/DaintyPig.png"; 
constexpr const wchar_t* FILE_PATH_PNG_HONKINGGOOSE = L"resource/Images/BasicAnimals/HonkingGoose.png"; 
constexpr const wchar_t* FILE_PATH_PNG_LEAPINGFROG = L"resource/Images/BasicAnimals/LeapingFrog.png"; 
constexpr const wchar_t* FILE_PATH_PNG_MADBOAR = L"resource/Images/BasicAnimals/MadBoar.png"; 
constexpr const wchar_t* FILE_PATH_PNG_MEOWINGCAT = L"resource/Images/BasicAnimals/MeowingCat.png"; 
constexpr const wchar_t* FILE_PATH_PNG_PASTURINGSHEEP = L"resource/Images/BasicAnimals/PasturingSheep.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SLOWTURTLE = L"resource/Images/BasicAnimals/SlowTurtle.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SNOWFOX = L"resource/Images/BasicAnimals/SnowFox.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SPIKEYPORCUPINE = L"resource/Images/BasicAnimals/SpikeyPorcupine.png"; 
constexpr const wchar_t* FILE_PATH_PNG_STINKYSKUNK = L"resource/Images/BasicAnimals/StinkySkunk.png"; 
constexpr const wchar_t* FILE_PATH_PNG_TIMBERWOLF = L"resource/Images/BasicAnimals/TimberWolf.png"; 
constexpr const wchar_t* FILE_PATH_PNG_TINYCHICK = L"resource/Images/BasicAnimals/TinyChick.png"; 

constexpr const wchar_t* FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE = L"resource/Images/Keyboard/keyboard_arrow_down_outline.png"; 
constexpr const wchar_t* FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE = L"resource/Images/Keyboard/keyboard_arrow_up_outline.png"; 
constexpr const wchar_t* FILE_PATH_PNG_KEYBOARD_Q_OUTLINE = L"resource/Images/Keyboard/keyboard_q_outline.png"; 
constexpr const wchar_t* FILE_PATH_PNG_KEYBOARD_R_OUTLINE = L"resource/Images/Keyboard/keyboard_r_outline.png"; 
constexpr const wchar_t* FILE_PATH_PNG_KEYBOARD_SPACE_OUTLINE = L"resource/Images/Keyboard/keyboard_space_outline.png"; 

constexpr const wchar_t* FILE_PATH_PNG_MINIBUNNY = L"resource/Images/MiniAnimals/MiniBunny.png"; 
constexpr const wchar_t* FILE_PATH_PNG_MINIWOLF = L"resource/Images/MiniAnimals/MiniWolf.png"; 

constexpr const wchar_t* FILE_PATH_PNG_PAPER = L"resource/Images/RPS/paper.png"; 
constexpr const wchar_t* FILE_PATH_PNG_POINT = L"resource/Images/RPS/point.png"; 
constexpr const wchar_t* FILE_PATH_PNG_SCISSORS = L"resource/Images/RPS/scissors.png"; 
constexpr const wchar_t* FILE_PATH_PNG_STONE = L"resource/Images/RPS/stone.png"; 

constexpr const wchar_t* FILE_PATH_JSON_CARD_CONFIG = L"resource/Json/card_config.json"; 
constexpr const wchar_t* FILE_PATH_JSON_ENEMY_CONFIG = L"resource/Json/enemy_config.json"; 
constexpr const wchar_t* FILE_PATH_JSON_EXAMPLE = L"resource/Json/example.json"; 

constexpr const wchar_t* FILE_PATH_MP3_BEEP = L"resource/Sounds/Beep.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_BGM = L"resource/Sounds/BGM.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_CONFIRM = L"resource/Sounds/Confirm.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_DRAWCARD = L"resource/Sounds/DrawCard.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_ENEMYHURT = L"resource/Sounds/EnemyHurt.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_FAIL = L"resource/Sounds/Fail.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_MAGIC = L"resource/Sounds/Magic.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_PLAYERHURT = L"resource/Sounds/PlayerHurt.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_SELECT = L"resource/Sounds/Select.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_SHUFFLE = L"resource/Sounds/Shuffle.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_WARNING = L"resource/Sounds/Warning.mp3"; 
constexpr const wchar_t* FILE_PATH_MP3_WIN = L"resource/Sounds/Win.mp3"; 



//------------------------------------------------------------------------------------------------------------------
// 
// file path table 
// 


const std::vector<const wchar_t*> FILE_PATH_TBL_FONTS = {
    FILE_PATH_OTF_UNIFONT_17,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_IMAGES_BASICANIMALS = {
    FILE_PATH_PNG_CLUCKINGCHICKEN,
    FILE_PATH_PNG_CORALCRAB,
    FILE_PATH_PNG_CROAKINGTOAD,
    FILE_PATH_PNG_DAINTYPIG,
    FILE_PATH_PNG_HONKINGGOOSE,
    FILE_PATH_PNG_LEAPINGFROG,
    FILE_PATH_PNG_MADBOAR,
    FILE_PATH_PNG_MEOWINGCAT,
    FILE_PATH_PNG_PASTURINGSHEEP,
    FILE_PATH_PNG_SLOWTURTLE,
    FILE_PATH_PNG_SNOWFOX,
    FILE_PATH_PNG_SPIKEYPORCUPINE,
    FILE_PATH_PNG_STINKYSKUNK,
    FILE_PATH_PNG_TIMBERWOLF,
    FILE_PATH_PNG_TINYCHICK,
};
const std::vector<const wchar_t*> FILE_PATH_TBL_IMAGES_KEYBOARD = {
    FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE,
    FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE,
    FILE_PATH_PNG_KEYBOARD_Q_OUTLINE,
    FILE_PATH_PNG_KEYBOARD_R_OUTLINE,
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
const std::vector<const wchar_t*> FILE_PATH_TBL_SOUNDS = {
    FILE_PATH_MP3_BEEP,
    FILE_PATH_MP3_BGM,
    FILE_PATH_MP3_CONFIRM,
    FILE_PATH_MP3_DRAWCARD,
    FILE_PATH_MP3_ENEMYHURT,
    FILE_PATH_MP3_FAIL,
    FILE_PATH_MP3_MAGIC,
    FILE_PATH_MP3_PLAYERHURT,
    FILE_PATH_MP3_SELECT,
    FILE_PATH_MP3_SHUFFLE,
    FILE_PATH_MP3_WARNING,
    FILE_PATH_MP3_WIN,
};


//------------------------------------------------------------------------------------------------------------------
// 
// file path map 
// 


const std::unordered_map<std::wstring, const wchar_t*> FILE_PATH_DICTIONARY = { 
    { std::wstring( L"unifont-17.otf" ), FILE_PATH_OTF_UNIFONT_17 },
    { std::wstring( L"CluckingChicken.png" ), FILE_PATH_PNG_CLUCKINGCHICKEN },
    { std::wstring( L"CoralCrab.png" ), FILE_PATH_PNG_CORALCRAB },
    { std::wstring( L"CroakingToad.png" ), FILE_PATH_PNG_CROAKINGTOAD },
    { std::wstring( L"DaintyPig.png" ), FILE_PATH_PNG_DAINTYPIG },
    { std::wstring( L"HonkingGoose.png" ), FILE_PATH_PNG_HONKINGGOOSE },
    { std::wstring( L"LeapingFrog.png" ), FILE_PATH_PNG_LEAPINGFROG },
    { std::wstring( L"MadBoar.png" ), FILE_PATH_PNG_MADBOAR },
    { std::wstring( L"MeowingCat.png" ), FILE_PATH_PNG_MEOWINGCAT },
    { std::wstring( L"PasturingSheep.png" ), FILE_PATH_PNG_PASTURINGSHEEP },
    { std::wstring( L"SlowTurtle.png" ), FILE_PATH_PNG_SLOWTURTLE },
    { std::wstring( L"SnowFox.png" ), FILE_PATH_PNG_SNOWFOX },
    { std::wstring( L"SpikeyPorcupine.png" ), FILE_PATH_PNG_SPIKEYPORCUPINE },
    { std::wstring( L"StinkySkunk.png" ), FILE_PATH_PNG_STINKYSKUNK },
    { std::wstring( L"TimberWolf.png" ), FILE_PATH_PNG_TIMBERWOLF },
    { std::wstring( L"TinyChick.png" ), FILE_PATH_PNG_TINYCHICK },
    { std::wstring( L"keyboard_arrow_down_outline.png" ), FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE },
    { std::wstring( L"keyboard_arrow_up_outline.png" ), FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE },
    { std::wstring( L"keyboard_q_outline.png" ), FILE_PATH_PNG_KEYBOARD_Q_OUTLINE },
    { std::wstring( L"keyboard_r_outline.png" ), FILE_PATH_PNG_KEYBOARD_R_OUTLINE },
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
    { std::wstring( L"Beep.mp3" ), FILE_PATH_MP3_BEEP },
    { std::wstring( L"BGM.mp3" ), FILE_PATH_MP3_BGM },
    { std::wstring( L"Confirm.mp3" ), FILE_PATH_MP3_CONFIRM },
    { std::wstring( L"DrawCard.mp3" ), FILE_PATH_MP3_DRAWCARD },
    { std::wstring( L"EnemyHurt.mp3" ), FILE_PATH_MP3_ENEMYHURT },
    { std::wstring( L"Fail.mp3" ), FILE_PATH_MP3_FAIL },
    { std::wstring( L"Magic.mp3" ), FILE_PATH_MP3_MAGIC },
    { std::wstring( L"PlayerHurt.mp3" ), FILE_PATH_MP3_PLAYERHURT },
    { std::wstring( L"Select.mp3" ), FILE_PATH_MP3_SELECT },
    { std::wstring( L"Shuffle.mp3" ), FILE_PATH_MP3_SHUFFLE },
    { std::wstring( L"Warning.mp3" ), FILE_PATH_MP3_WARNING },
    { std::wstring( L"Win.mp3" ), FILE_PATH_MP3_WIN },
};


//------------------------------------------------------------------------------------------------------------------
// 
// file name 
// 


constexpr const wchar_t* FILE_NAME_OTF_UNIFONT_17 = L"unifont-17.otf"; 

constexpr const wchar_t* FILE_NAME_PNG_CLUCKINGCHICKEN = L"CluckingChicken.png"; 
constexpr const wchar_t* FILE_NAME_PNG_CORALCRAB = L"CoralCrab.png"; 
constexpr const wchar_t* FILE_NAME_PNG_CROAKINGTOAD = L"CroakingToad.png"; 
constexpr const wchar_t* FILE_NAME_PNG_DAINTYPIG = L"DaintyPig.png"; 
constexpr const wchar_t* FILE_NAME_PNG_HONKINGGOOSE = L"HonkingGoose.png"; 
constexpr const wchar_t* FILE_NAME_PNG_LEAPINGFROG = L"LeapingFrog.png"; 
constexpr const wchar_t* FILE_NAME_PNG_MADBOAR = L"MadBoar.png"; 
constexpr const wchar_t* FILE_NAME_PNG_MEOWINGCAT = L"MeowingCat.png"; 
constexpr const wchar_t* FILE_NAME_PNG_PASTURINGSHEEP = L"PasturingSheep.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SLOWTURTLE = L"SlowTurtle.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SNOWFOX = L"SnowFox.png"; 
constexpr const wchar_t* FILE_NAME_PNG_SPIKEYPORCUPINE = L"SpikeyPorcupine.png"; 
constexpr const wchar_t* FILE_NAME_PNG_STINKYSKUNK = L"StinkySkunk.png"; 
constexpr const wchar_t* FILE_NAME_PNG_TIMBERWOLF = L"TimberWolf.png"; 
constexpr const wchar_t* FILE_NAME_PNG_TINYCHICK = L"TinyChick.png"; 

constexpr const wchar_t* FILE_NAME_PNG_KEYBOARD_ARROW_DOWN_OUTLINE = L"keyboard_arrow_down_outline.png"; 
constexpr const wchar_t* FILE_NAME_PNG_KEYBOARD_ARROW_UP_OUTLINE = L"keyboard_arrow_up_outline.png"; 
constexpr const wchar_t* FILE_NAME_PNG_KEYBOARD_Q_OUTLINE = L"keyboard_q_outline.png"; 
constexpr const wchar_t* FILE_NAME_PNG_KEYBOARD_R_OUTLINE = L"keyboard_r_outline.png"; 
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

constexpr const wchar_t* FILE_NAME_MP3_BEEP = L"Beep.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_BGM = L"BGM.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_CONFIRM = L"Confirm.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_DRAWCARD = L"DrawCard.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_ENEMYHURT = L"EnemyHurt.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_FAIL = L"Fail.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_MAGIC = L"Magic.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_PLAYERHURT = L"PlayerHurt.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_SELECT = L"Select.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_SHUFFLE = L"Shuffle.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_WARNING = L"Warning.mp3"; 
constexpr const wchar_t* FILE_NAME_MP3_WIN = L"Win.mp3"; 



//------------------------------------------------------------------------------------------------------------------
// 
// file name table 
// 


const std::vector<const wchar_t*> FILE_NAME_TBL_FONTS = {
    FILE_NAME_OTF_UNIFONT_17,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_IMAGES_BASICANIMALS = {
    FILE_NAME_PNG_CLUCKINGCHICKEN,
    FILE_NAME_PNG_CORALCRAB,
    FILE_NAME_PNG_CROAKINGTOAD,
    FILE_NAME_PNG_DAINTYPIG,
    FILE_NAME_PNG_HONKINGGOOSE,
    FILE_NAME_PNG_LEAPINGFROG,
    FILE_NAME_PNG_MADBOAR,
    FILE_NAME_PNG_MEOWINGCAT,
    FILE_NAME_PNG_PASTURINGSHEEP,
    FILE_NAME_PNG_SLOWTURTLE,
    FILE_NAME_PNG_SNOWFOX,
    FILE_NAME_PNG_SPIKEYPORCUPINE,
    FILE_NAME_PNG_STINKYSKUNK,
    FILE_NAME_PNG_TIMBERWOLF,
    FILE_NAME_PNG_TINYCHICK,
};
const std::vector<const wchar_t*> FILE_NAME_TBL_IMAGES_KEYBOARD = {
    FILE_NAME_PNG_KEYBOARD_ARROW_DOWN_OUTLINE,
    FILE_NAME_PNG_KEYBOARD_ARROW_UP_OUTLINE,
    FILE_NAME_PNG_KEYBOARD_Q_OUTLINE,
    FILE_NAME_PNG_KEYBOARD_R_OUTLINE,
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
const std::vector<const wchar_t*> FILE_NAME_TBL_SOUNDS = {
    FILE_NAME_MP3_BEEP,
    FILE_NAME_MP3_BGM,
    FILE_NAME_MP3_CONFIRM,
    FILE_NAME_MP3_DRAWCARD,
    FILE_NAME_MP3_ENEMYHURT,
    FILE_NAME_MP3_FAIL,
    FILE_NAME_MP3_MAGIC,
    FILE_NAME_MP3_PLAYERHURT,
    FILE_NAME_MP3_SELECT,
    FILE_NAME_MP3_SHUFFLE,
    FILE_NAME_MP3_WARNING,
    FILE_NAME_MP3_WIN,
};


//------------------------------------------------------------------------------------------------------------------
// 
// font name 
// 


constexpr const wchar_t* FONT_NAME_UNIFONT = L"Unifont"; 


//------------------------------------------------------------------------------------------------------------------
// 
// font name table 
// 


const std::vector<const wchar_t*> FONT_NAME_TABLE = { 
     FONT_NAME_UNIFONT,
};


//------------------------------------------------------------------------------------------------------------------
// 
// font name to path map 
// 


const std::unordered_map<std::wstring, const wchar_t*> FONT_NAME_TO_PATH_DICTIONARY = { 
     { std::wstring( FONT_NAME_UNIFONT ), L"resource/Fonts/unifont-17.otf" },
};
