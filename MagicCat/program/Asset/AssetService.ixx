module;

#include <dxe.h>
#include <vector>
#include <string>

export module AssetService;

namespace mc
{


    export enum class EFont
    {
        ARK_PIXEL_16PX_JP
    };

    export enum class EImage
    {
        Rock = 0, Scissors = 1, Paper = 2, Magic = 3,

        KB_Q = 100, KB_R, KB_SPACE, KB_UP, KB_DOWN
    };

    export enum class ESound
    {
        Confirm, DrawCard, EnemyHurt, 
        Fail, PlayerHurt, Select, 
        Shuffle, Warning, Win
    };

    export enum class ESprite
    {
        Bunny, Wolf
    };

    export class IAssetService
    {
    public:
        virtual ~IAssetService() = default;
        virtual const int GetImageHandle(EImage) = 0;
        virtual const int GetFontHandle(EFont) = 0;
        virtual const int GetSpriteHandle(ESprite) = 0;
        virtual const int GetSoundHandle(ESound) = 0;

    };

    export Shared<IAssetService> CreateAssetService();
} // namespace mc
