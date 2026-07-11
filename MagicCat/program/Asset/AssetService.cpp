module;

#include <dxe.h>
#include <memory>
#include <unordered_map>
#include <ResourceConstantHedder.h>
#include <vector>
#include <windows.h>
#include <cassert>

module AssetService;

namespace mc {
    class AssetService : public IAssetService
    {
    public:
        static constexpr Point<int> LARGE_SPRITE_SIZE = {32, 32};
        static constexpr Point<int> SMALL_SPRITE_SIZE = {16, 16};
        static constexpr size_t SPRITE_FRAME_COUNT = 4;
        static const std::unordered_map<ESprite, SpriteInfo> SPRITE_INFO_MAP;

        AssetService()
        {
            LoadFonts();
            LoadImages();
            LoadSounds();
        }

        int GetFontHandle(EFont e) override
        {
            if (e == EFont::Null) return -1;
            if (fontMappings.contains(e)) return fontMappings.at(e);
            assert(false && "未登録のフォントにアクセスしようとしています");
            return -1;
        }

        int GetImageHandle(EImage e) override
        {
            if (e == EImage::Null) return -1;
            if (imageMappings.contains(e)) return imageMappings.at(e);
            assert(false && "未登録の画像にアクセスしようとしています");
            return -1;
        }

        int GetSpriteHandle(ESprite e) override
        {
            if (e == ESprite::Null) return -1;
            if (spriteMappings.contains(e)) return spriteMappings[e];
            assert(false && "未登録のスプライトにアクセスしようとしています");
            return -1;
        }

        SpriteInfo GetSpriteInfo(ESprite e) override
        {
            auto it = SPRITE_INFO_MAP.find(e);
            if (it == SPRITE_INFO_MAP.end())
            {
                printfDx(L"GetSpriteInfo: 未登録の ESprite 値 %d\n", static_cast<int>(e));
                return {{0, 0}, 0};
            }
            return it->second;
        }

        int GetSoundHandle(ESound e) override
        {
            if (e == ESound::Null) return -1;
            if (soundMappings.contains(e)) return soundMappings.at(e);
            assert(false && "未登録の音声にアクセスしようとしています");
            return -1;
        }

        ESprite ParseSprite(const std::string& name) const override
        {
            static const std::unordered_map<std::string, ESprite> spriteMap = {
                {"Bunny", ESprite::Bunny},
                {"Wolf", ESprite::Wolf},
                {"CluckingChicken", ESprite::CluckingChicken},
                {"CoralCrab", ESprite::CoralCrab},
                {"CroakingToad", ESprite::CroakingToad},
                {"DaintyPig", ESprite::DaintyPig},
                {"HonkingGoose", ESprite::HonkingGoose},
                {"LeapingFrog", ESprite::LeapingFrog},
                {"MadBoar", ESprite::MadBoar},
                {"MeowingCat", ESprite::MeowingCat},
                {"PasturingSheep", ESprite::PasturingSheep},
                {"SlowTurtle", ESprite::SlowTurtle},
                {"SnowFox", ESprite::SnowFox},
                {"SpikeyPorcupine", ESprite::SpikeyPorcupine},
                {"StinkySkunk", ESprite::StinkySkunk},
                {"TimberWolf", ESprite::TimberWolf},
                {"TinyChick", ESprite::TinyChick}
            };
            if (auto it = spriteMap.find(name); it != spriteMap.end())
                return it->second;
            return ESprite::Null;
        }

    private:
        void LoadFonts()
        {
            ChangeFontType(DX_FONTTYPE_NORMAL);
            int handle = CreateFontToHandle(FILE_NAME_OTF_UNIFONT_17, 16, -1, DX_FONTTYPE_NORMAL);
            if (handle == -1) printfDx(L"フォントの読み込みに失敗\n");
            fontMappings.insert({EFont::UNIFONT_17, handle});
        }

        void LoadImages()
        {
            try
            {
                LoadImageFiles();
                LoadSpriteFiles();
            }
            catch (const std::exception&) { printfDx(L"画像の読み込みに失敗"); }
        }

        void LoadImageFiles()
        {
            struct ImageData
            {
                EImage id;
                const wchar_t* path;
            };
            const ImageData images[] = {
                {EImage::Rock, FILE_PATH_PNG_STONE},
                {EImage::Scissors, FILE_PATH_PNG_SCISSORS},
                {EImage::Paper, FILE_PATH_PNG_PAPER},
                {EImage::Magic, FILE_PATH_PNG_POINT},
                {EImage::KB_Q, FILE_PATH_PNG_KEYBOARD_Q_OUTLINE},
                {EImage::KB_ESCAPE, FILE_PATH_PNG_KEYBOARD_ESCAPE_OUTLINE},
                {EImage::KB_SPACE, FILE_PATH_PNG_KEYBOARD_SPACE_OUTLINE},
                {EImage::KB_UP, FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE},
                {EImage::KB_DOWN, FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE},
                {EImage::KB_LEFT, FILE_PATH_PNG_KEYBOARD_ARROW_LEFT_OUTLINE},
                {EImage::KB_RIGHT, FILE_PATH_PNG_KEYBOARD_ARROW_RIGHT_OUTLINE},
                {EImage::MOUSE_MOVE, FILE_PATH_PNG_MOUSE_MOVE},
                {EImage::MOUSE_LEFT, FILE_PATH_PNG_MOUSE_LEFT_OUTLINE},
                {EImage::MOUSE_RIGHT, FILE_PATH_PNG_MOUSE_RIGHT_OUTLINE},
                {EImage::XBOX_A, FILE_PATH_PNG_XBOX_BUTTON_COLOR_A_OUTLINE},
                {EImage::XBOX_B, FILE_PATH_PNG_XBOX_BUTTON_COLOR_B_OUTLINE},
                {EImage::XBOX_X, FILE_PATH_PNG_XBOX_BUTTON_COLOR_X_OUTLINE},
                {EImage::XBOX_Y, FILE_PATH_PNG_XBOX_BUTTON_COLOR_Y_OUTLINE},
                {EImage::XBOX_DPAD_HORIZONTAL, FILE_PATH_PNG_XBOX_DPAD_ROUND_HORIZONTAL},
                {EImage::BUTTON_MENU, FILE_PATH_PNG_BUTTON_MENU_OUTLINE},
            };
            for (const auto& img : images)
            {
                LoadAndMapResource(
                    imageMappings, img.id, img.path, [](const wchar_t* p)
                    {
                        return LoadGraph(p);
                    });
            }
        }

        void LoadSpriteFiles()
        {
            struct SpriteData
            {
                ESprite id;
                const wchar_t* path;
            };
            const SpriteData sprites[] = {
                {ESprite::Bunny, FILE_PATH_PNG_MINIBUNNY},
                {ESprite::Wolf, FILE_PATH_PNG_MINIWOLF},
                {ESprite::CluckingChicken, FILE_PATH_PNG_CLUCKINGCHICKEN},
                {ESprite::CoralCrab, FILE_PATH_PNG_CORALCRAB},
                {ESprite::CroakingToad, FILE_PATH_PNG_CROAKINGTOAD},
                {ESprite::DaintyPig, FILE_PATH_PNG_DAINTYPIG},
                {ESprite::HonkingGoose, FILE_PATH_PNG_HONKINGGOOSE},
                {ESprite::LeapingFrog, FILE_PATH_PNG_LEAPINGFROG},
                {ESprite::MadBoar, FILE_PATH_PNG_MADBOAR},
                {ESprite::MeowingCat, FILE_PATH_PNG_MEOWINGCAT},
                {ESprite::PasturingSheep, FILE_PATH_PNG_PASTURINGSHEEP},
                {ESprite::SlowTurtle, FILE_PATH_PNG_SLOWTURTLE},
                {ESprite::SnowFox, FILE_PATH_PNG_SNOWFOX},
                {ESprite::SpikeyPorcupine, FILE_PATH_PNG_SPIKEYPORCUPINE},
                {ESprite::StinkySkunk, FILE_PATH_PNG_STINKYSKUNK},
                {ESprite::TimberWolf, FILE_PATH_PNG_TIMBERWOLF},
                {ESprite::TinyChick, FILE_PATH_PNG_TINYCHICK}
            };
            for (const auto& spr : sprites)
            {
                LoadAndMapResource(
                    spriteMappings, spr.id, spr.path, [](const wchar_t* p)
                    {
                        return LoadGraph(p);
                    });
            }
        }

        void LoadSounds()
        {
            try
            {
                struct SoundData
                {
                    ESound id;
                    const wchar_t* path;
                };
                SoundData sounds[] = {
                    {ESound::Confirm, FILE_PATH_MP3_CONFIRM},
                    {ESound::DrawCard, FILE_PATH_MP3_DRAWCARD},
                    {ESound::Fail, FILE_PATH_MP3_FAIL},
                    {ESound::Select, FILE_PATH_MP3_SELECT},
                    {ESound::Shuffle, FILE_PATH_MP3_SHUFFLE},
                    {ESound::Warning, FILE_PATH_MP3_WARNING},
                    {ESound::Beep, FILE_PATH_MP3_BEEP},
                    {ESound::Magic, FILE_PATH_MP3_MAGIC},
                    {ESound::BGM, FILE_PATH_MP3_BGM},
                    {ESound::CatAttack, FILE_PATH_MP3_CATATTACK},
                    {ESound::SheepAttack, FILE_PATH_MP3_SHEEPATTACK},
                    {ESound::WolfAttack, FILE_PATH_MP3_WOLFATTACK},
                    {ESound::PigAttack, FILE_PATH_MP3_PIGATTACK},
                    {ESound::WinTheBattle, FILE_PATH_MP3_WINTHEBATTLE},
                    {ESound::WinTheGame, FILE_PATH_MP3_WINTHEGAME}
                };

                for (const auto& snd : sounds)
                {
                    LoadAndMapResource(
                        soundMappings, snd.id, snd.path, [](const wchar_t* p)
                        {
                            return LoadSoundMem(p);
                        });
                }
            }
            catch (const std::exception&) { printfDx(L"音声の読み込みに失敗"); }
        }

        template <typename MapType, typename KeyType, typename LoadFunc>
        void LoadAndMapResource(MapType& mappings, KeyType id, const wchar_t* path, LoadFunc&& loadFunc)
        {
            int handle = loadFunc(path);
            if (handle == -1) printfDx(L"%sの読み込みに失敗\n", path);
            else mappings.insert({id, handle});
        }


        std::unordered_map<EImage, int> imageMappings = {};
        std::unordered_map<ESprite, int> spriteMappings = {};
        std::unordered_map<EFont, int> fontMappings = {};
        std::unordered_map<ESound, int> soundMappings = {};

    };

    const std::unordered_map<ESprite, SpriteInfo> AssetService::SPRITE_INFO_MAP = {
        // 32×32 スプライトシート（大型キャラクター）
        {ESprite::Bunny,           {LARGE_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::Wolf,            {LARGE_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        // 16×16 スプライトシート（小型キャラクター）
        {ESprite::CluckingChicken, {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::CoralCrab,       {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::CroakingToad,    {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::DaintyPig,       {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::HonkingGoose,    {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::LeapingFrog,     {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::MadBoar,         {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::MeowingCat,      {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::PasturingSheep,  {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::SlowTurtle,      {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::SnowFox,         {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::SpikeyPorcupine, {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::StinkySkunk,     {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::TimberWolf,      {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
        {ESprite::TinyChick,       {SMALL_SPRITE_SIZE, SPRITE_FRAME_COUNT}},
    };

    std::unique_ptr<IAssetService> CreateAssetService()
    {
        return std::make_unique<AssetService>();
    }
} // namespace mc
