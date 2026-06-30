module;

#include <dxe.h>
#include <memory>
#include <unordered_map>
#include <ResourceConstantHedder.h>
#include <vector>
#include <windows.h>
#include <cassert>

module AssetService;

namespace {
    // ESprite の整数値がこの閾値未満のスプライトは 32×32 のスプライトシートを使用する。
    // この閾値以上のスプライトは 16×16 のスプライトシートを使用する。
    constexpr int LARGE_SPRITE_ENUM_THRESHOLD = 100;
}

namespace mc {
    class AssetService : public IAssetService
    {
    public:
        AssetService()
        {
            LoadFonts();
            LoadImages();
            LoadSounds();
        }

        int GetFontHandle(EFont e) override
        {
            if (e == EFont::Null) return -1;
            if (fontMappings.contains(e))
                return fontMappings.at(e);
            assert(false && "未登録のフォントにアクセスしようとしています");
            return -1;
        }


        int GetImageHandle(EImage e) override
        {
            if (e == EImage::Null) return -1;
            if (imageMappings.contains(e))
                return imageMappings.at(e);
            assert(false && "未登録の画像にアクセスしようとしています");
            return -1;
        }

        int GetSpriteHandle(ESprite e) override
        {
            if (e == ESprite::Null) return -1;
            if (spriteMappings.contains(e))
                return spriteMappings[e];
            assert(false && "未登録のスプライトにアクセスしようとしています");
            return -1;
        }

        SpriteInfo GetSpriteInfo(ESprite e) override
        {
            if (static_cast<int>(e) < LARGE_SPRITE_ENUM_THRESHOLD)
            {
                return {{32, 32}, 4};
            }
            return {{16, 16}, 4};
        }

        int GetSoundHandle(ESound e) override
        {
            if (e == ESound::Null) return -1;
            if (soundMappings.contains(e))
                return soundMappings.at(e);
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
        std::unordered_map<EImage, int> imageMappings = {};
        std::unordered_map<ESprite, int> spriteMappings = {};
        std::unordered_map<EFont, int> fontMappings = {};
        std::unordered_map<ESound, int> soundMappings = {};

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
                struct ImageData
                {
                    EImage id;
                    const wchar_t* path;
                };
                ImageData images[] = {
                    {EImage::Rock, FILE_PATH_PNG_STONE},
                    {EImage::Scissors, FILE_PATH_PNG_SCISSORS},
                    {EImage::Paper, FILE_PATH_PNG_PAPER},
                    {EImage::Magic, FILE_PATH_PNG_POINT},
                    {EImage::KB_Q, FILE_PATH_PNG_KEYBOARD_Q_OUTLINE},
                    {EImage::KB_R, FILE_PATH_PNG_KEYBOARD_R_OUTLINE},
                    {EImage::KB_SPACE, FILE_PATH_PNG_KEYBOARD_SPACE_OUTLINE},
                    {EImage::KB_UP, FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE},
                    {EImage::KB_DOWN, FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE}
                };

                for (const auto& img : images)
                {
                    int handle = LoadGraph(img.path);
                    if (handle == -1)
                        printfDx(L"%sの読み込みに失敗", img.path);
                    else
                        imageMappings.insert({img.id, handle});
                }

                struct SpriteData
                {
                    ESprite id;
                    const wchar_t* path;
                };
                SpriteData sprites[] = {
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
                    auto resource = LoadGraph(spr.path);
                    if (resource == -1)
                        printfDx(L"%sの読み込みに失敗", spr.path);
                    else
                        spriteMappings.insert({spr.id, resource});
                }
            }
            catch (const std::exception&)
            {
                printfDx(L"画像の読み込みに失敗");
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
                    {ESound::EnemyHurt, FILE_PATH_MP3_ENEMYHURT},
                    {ESound::Fail, FILE_PATH_MP3_FAIL},
                    {ESound::PlayerHurt, FILE_PATH_MP3_PLAYERHURT},
                    {ESound::Select, FILE_PATH_MP3_SELECT},
                    {ESound::Shuffle, FILE_PATH_MP3_SHUFFLE},
                    {ESound::Warning, FILE_PATH_MP3_WARNING},
                    {ESound::Win, FILE_PATH_MP3_WIN},
                    {ESound::Beep, FILE_PATH_MP3_BEEP},
                    {ESound::Magic, FILE_PATH_MP3_MAGIC},
                    {ESound::BGM, FILE_PATH_MP3_BGM}
                };

                for (const auto& snd : sounds)
                {
                    int handle = LoadSoundMem(snd.path);
                    if (handle == -1)
                        printfDx(L"%sの読み込みに失敗", snd.path);
                    else
                        soundMappings.insert({snd.id, handle});
                }
            }
            catch (const std::exception&)
            {
                printfDx(L"音声の読み込みに失敗");
            }
        }
    };

    std::unique_ptr<IAssetService> CreateAssetService()
    {
        return std::make_unique<AssetService>();
    }
} // namespace mc
