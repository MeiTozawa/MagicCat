module;

#include <dxe.h>

export module AssetService;

export enum class EAsset
{
    Rock = 0, Scissors = 1, Paper = 2, 
};

export class IAssetService
{
public:
    virtual ~IAssetService() = default;
    virtual void LoadAssets() = 0;
    virtual const Shared<dxe::Sprite> GetImage(EAsset) = 0;
};