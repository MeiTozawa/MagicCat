module;

#include <dxe.h>

export module AssetService;

export enum class EImage
{
    Rock = 0, Scissors = 1, Paper = 2, 
};

export enum class ESprite
{
    Bunny, Wolf
};

export class IAssetService
{
public:
    virtual ~IAssetService() = default;
    virtual void LoadAssets() = 0;
    virtual const Shared<dxe::Sprite> GetImage(EImage) = 0;
    virtual const int GetSpriteHandle(ESprite) = 0;
};