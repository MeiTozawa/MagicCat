module;

#include <dxe.h>
#include <vector>
#include <string>
#include <RenderUtils.h>

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

        KB_Q = 100, KB_ESCAPE, KB_SPACE, KB_UP, KB_DOWN,
        KB_LEFT, KB_RIGHT,
        
        MOUSE_MOVE = 200, MOUSE_LEFT, MOUSE_RIGHT,

        XBOX_A = 300, XBOX_B, XBOX_X, XBOX_Y,
        XBOX_DPAD_HORIZONTAL,
        BUTTON_MENU
    };

    export enum class ESound
    {
        Null = -1,
        Confirm = 0, DrawCard, Select, Shuffle, Warning,
        CatAttack, SheepAttack, WolfAttack, PigAttack,
        Fail, WinTheBattle, WinTheGame,
        Beep, Magic, BGM
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

    /// @brief スプライト画像の詳細情報（サイズとフレーム数）を保持する構造体
    export struct SpriteInfo
    {
        Point<int> size;
        size_t frame;
    };

    /// @brief 画像、フォント、スプライト、音源などのアセットハンドル管理を担当するインターフェース
    export class IAssetService
    {
    public:
        virtual ~IAssetService() = default;

        /// @brief 指定した画像アセットのDxLibグラフィックハンドルを取得する
        /// @param image 画像アセットのID
        /// @return グラフィックハンドル
        virtual int GetImageHandle(EImage image) = 0;

        /// @brief 指定したフォントアセットのDxLibフォントハンドルを取得する
        /// @param font フォントアセットのID
        /// @return フォントハンドル
        virtual int GetFontHandle(EFont font) = 0;

        /// @brief 指定したスプライトアセットのDxLibグラフィックハンドルを取得する
        /// @param sprite スプライトアセットのID
        /// @return グラフィックハンドル
        virtual int GetSpriteHandle(ESprite sprite) = 0;

        /// @brief スプライトアセットの詳細情報（サイズやフレーム数）を取得する
        /// @param sprite スプライトアセットのID
        /// @return SpriteInfo構造体
        virtual SpriteInfo GetSpriteInfo(ESprite sprite) = 0;

        /// @brief 指定した音源アセットのDxLibサウンドハンドルを取得する
        /// @param sound 音源アセットのID
        /// @return サウンドハンドル
        virtual int GetSoundHandle(ESound sound) = 0;

        /// @brief スプライト名の文字列を ESprite 列挙値に変換する。未知の名前は ESprite::Null を返す。
        /// @param name スプライト名の文字列
        /// @return 対応するESprite列挙値
        virtual ESprite ParseSprite(const std::string& name) const = 0;
    };

    export std::unique_ptr<IAssetService> CreateAssetService();
} // namespace mc
