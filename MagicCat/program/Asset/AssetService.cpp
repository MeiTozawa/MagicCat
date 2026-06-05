module;

#include <dxe.h>
#include <memory>
#include <unordered_map>

module AssetService;
import ServiceLocator;

class AssetService : public IAssetService
{
public:
    void LoadAssets() override
    {
        loadImages();
    }

    const Shared<dxe::Sprite> GetImage(EAsset e) override
    {
        return imageMappings[e];
    }

private:
    std::unordered_map<EAsset, Shared<dxe::Sprite>> imageMappings = {};

    void loadImages()
    {
        try
        {
            auto rock_resource = dxe::SpriteResouce::Create(L"resource/graphics/example/stone.png");
            if (!rock_resource)
                printfDx(L"石の画像の読み込みに失敗");
            else
                imageMappings.insert({EAsset::Rock, dxe::Sprite::Create(rock_resource)});

            auto scissors_resource = dxe::SpriteResouce::Create(L"resource/graphics/example/scissors.png");
            if (!scissors_resource)
                printfDx(L"ハサミの画像の読み込みに失敗");
            else
                imageMappings.insert({EAsset::Scissors, dxe::Sprite::Create(scissors_resource)});

            auto paper_resource = dxe::SpriteResouce::Create(L"resource/graphics/example/paper.png");
            if (!paper_resource)
                printfDx(L"紙の画像の読み込みに失敗");
            else
                imageMappings.insert({EAsset::Paper, dxe::Sprite::Create(paper_resource)});
        }
        catch (const std::exception&)
        {
            printfDx(L"画像の読み込みに失敗");
        }
    }


};

static struct RegisterAssetService
{
    RegisterAssetService()
    {
        ServiceLocator::RegisterSingleton<IAssetService, AssetService>(std::make_shared<AssetService>());
    }
} autoRegister_AssetService;
