module;

#include <dxe.h>
#include <vector>
#include <string>

export module AssetService;

namespace mc
{
    export struct EnemyConfig
    {
        int baseWeight;
        int rockDamage;
        int scissorsDamage;
        int paperDamage;
        std::wstring name;
    };


    export struct CardConfig
    {
        int type;
        int value;
    };
    
    export enum class EFont
    {
        ARK_PIXEL_16PX_JP
    };

    export enum class EImage
    {
        Rock = 0, Scissors = 1, Paper = 2, Magic = 3,

        KB_Q = 100, KB_SPACE, KB_UP, KB_DOWN
    };

    export enum class ESprite
    {
        Bunny, Wolf
    };

    export class IAssetService
    {
    public:
        virtual ~IAssetService() = default;
        virtual const int GetImage(EImage) = 0;
        virtual const int GetFontHandle(EFont) = 0;
        virtual const int GetSpriteHandle(ESprite) = 0;
        virtual const std::vector<CardConfig>& GetCardConfigs() const = 0;
        virtual const std::vector<EnemyConfig>& GetEnemyConfigs() const = 0;
    };

    export Shared<IAssetService> CreateAssetService();
} // namespace mc
