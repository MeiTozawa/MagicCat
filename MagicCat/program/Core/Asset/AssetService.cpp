module;

#include <dxe.h>
#include <memory>
#include <unordered_map>
#include "FilePaths.h"

module AssetService;
import ServiceLocator;

class AssetService : public IAssetService
{
public:
    void LoadAssets() override
    {
        loadImages();
    }

    const Shared<dxe::Sprite> GetImage(EImage e) override
    {
        if (imageMappings.contains(e))
            return imageMappings.at(e);
        return nullptr;
    }
    
    const int GetSpriteHandle(ESprite e) override
    {
        return spriteMappings[e];
    }

private:
    std::unordered_map<EImage, Shared<dxe::Sprite>> imageMappings = {};
    std::unordered_map<ESprite, int> spriteMappings = {};

    void loadImages()
    {
        try
        {
            auto rock_resource = dxe::SpriteResouce::Create(FILE_ROCK);
            if (!rock_resource)
                printfDx(L"%sの読み込みに失敗", FILE_ROCK.c_str());
            else
                imageMappings.insert({EImage::Rock, dxe::Sprite::Create(rock_resource)});

            auto scissors_resource = dxe::SpriteResouce::Create(FILE_SCISSORS);
            if (!scissors_resource)
                printfDx(L"%sの読み込みに失敗", FILE_SCISSORS.c_str());
            else
                imageMappings.insert({EImage::Scissors, dxe::Sprite::Create(scissors_resource)});

            auto paper_resource = dxe::SpriteResouce::Create(FILE_PAPER);
            if (!paper_resource)
                printfDx(L"%sの読み込みに失敗", FILE_PAPER.c_str());
            else
                imageMappings.insert({EImage::Paper, dxe::Sprite::Create(paper_resource)});
            
            auto magic_resource = dxe::SpriteResouce::Create(FILE_MAGIC);
            if (!magic_resource)
                printfDx(L"%sの読み込みに失敗", FILE_MAGIC.c_str());
            else
                imageMappings.insert({EImage::Magic, dxe::Sprite::Create(magic_resource)});
            
            auto kb_q_resource = dxe::SpriteResouce::Create(FILE_KB_Q);
            if (!kb_q_resource)
                printfDx(L"%sの読み込みに失敗", FILE_KB_Q.c_str());
            else
                imageMappings.insert({EImage::KB_Q, dxe::Sprite::Create(kb_q_resource)});
            
            auto kb_space_resource = dxe::SpriteResouce::Create(FILE_KB_SPACE);
            if (!kb_space_resource)
                printfDx(L"%sの読み込みに失敗", FILE_KB_SPACE.c_str());
            else
                imageMappings.insert({EImage::KB_SPACE, dxe::Sprite::Create(kb_space_resource)});
            
            auto kb_up_resource = dxe::SpriteResouce::Create(FILE_KB_UP);
            if (!kb_up_resource)
                printfDx(L"%sの読み込みに失敗", FILE_KB_UP.c_str());
            else
                imageMappings.insert({EImage::KB_UP, dxe::Sprite::Create(kb_up_resource)});
            
            auto kb_down_resource = dxe::SpriteResouce::Create(FILE_KB_DOWN);
            if (!kb_down_resource)
                printfDx(L"%sの読み込みに失敗", FILE_KB_DOWN.c_str());
            else
                imageMappings.insert({EImage::KB_DOWN, dxe::Sprite::Create(kb_down_resource)});
        
            
            
            auto bunny_resource = LoadGraph(FILE_BUNNY_SHEET.c_str());
            if (!bunny_resource)
                printfDx(L"%sの読み込みに失敗", FILE_BUNNY_SHEET.c_str());
            else
                spriteMappings.insert({ESprite::Bunny, bunny_resource});
            
            auto wolf_resource = LoadGraph(FILE_WOLF_SHEET.c_str());
            if (!wolf_resource)
                printfDx(L"%sの読み込みに失敗", FILE_BUNNY_SHEET.c_str());
            else
                spriteMappings.insert({ESprite::Wolf, wolf_resource});
            
            
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
