module;

#include <dxe.h>
#include <vector>
#include <string>

export module AssetService;

namespace mc {
    export enum class EFont
    {
        Null = -1,
        UNIFONT_17
    };

    export enum class EImage
    {
        Null = -1,
        Rock = 0, Scissors = 1, Paper = 2, Magic = 3,

        KB_Q = 100, KB_R, KB_SPACE, KB_UP, KB_DOWN
    };

    export enum class ESound
    {
        Null = -1,
        Confirm = 0, DrawCard, EnemyHurt,
        Fail, PlayerHurt, Select,
        Shuffle, Warning, Win,
        Beep, Magic
    };

    export enum class ESprite
    {
        Null = -1,
        Bunny, Wolf,

        CluckingChicken = 100,
        CoralCrab,
        CroakingToad,
        DaintyPig,
        HonkingGoose,
        LeapingFrog,
        MadBoar,
        MeowingCat,
        PasturingSheep,
        SlowTurtle,
        SnowFox,
        SpikeyPorcupine,
        StinkySkunk,
        TimberWolf,
        TinyChick
    };

    export struct SpriteInfo
    {
        tnl::Vector2i size;
        size_t frame;
    };

    export class IAssetService
    {
    public:
        virtual ~IAssetService() = default;
        virtual const int GetImageHandle(EImage) = 0;
        virtual const int GetFontHandle(EFont) = 0;
        virtual const int GetSpriteHandle(ESprite) = 0;
        virtual const SpriteInfo GetSpriteInfo(ESprite) = 0;
        virtual const int GetSoundHandle(ESound) = 0;
        /// @brief スプライト名の文字列を ESprite 列挙値に変換する。未知の名前は ESprite::Null を返す。
        virtual ESprite ParseSprite(const std::string& name) const = 0;
    };

    export std::unique_ptr<IAssetService> CreateAssetService();
} // namespace mc
