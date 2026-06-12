module;

#include <dxe.h>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <ResourceConstantHedder.h>
#include <sstream>
#include <vector>

module AssetService;
import ServiceLocator;

namespace mc
{
    class AssetService : public IAssetService
    {
    public:

        AssetService()
        {
            LoadFonts();
            LoadImages();
            LoadGameConfig();
        }
        
        const int GetFontHandle(EFont font) override
        {
            return fontMappings.at(font);
        }

        const std::vector<CardConfig>& GetCardConfigs() const override
        {
            return cardConfigs;
        }

        const std::vector<EnemyConfig>& GetEnemyConfigs() const override
        {
            return enemyConfigs;
        }

        const int GetImage(EImage e) override
        {
            if (imageMappings.contains(e))
                return imageMappings.at(e);
            return -1;
        }

        const int GetSpriteHandle(ESprite e) override
        {
            return spriteMappings[e];
        }

    private:
        std::unordered_map<EImage, int> imageMappings = {};
        std::unordered_map<ESprite, int> spriteMappings = {};
        std::unordered_map<EFont, int> fontMappings = {};
        std::vector<CardConfig> cardConfigs;
        std::vector<EnemyConfig> enemyConfigs;

        void LoadFonts()
        {
            ChangeFontType(DX_FONTTYPE_NORMAL); 
            int handle = LoadFontDataToHandle(FILE_PATH_TTF_ARK_PIXEL_16PX_PROPORTIONAL_JA, 16);
            fontMappings.insert({EFont::ARK_PIXEL_16PX_JP, handle});
        }
        
        void LoadGameConfig()
        {
            {
                std::ifstream ifs("resource/json/card_config.json");
                if (ifs.is_open())
                {
                    std::stringstream ss;
                    ss << ifs.rdbuf();
                    std::string err;
                    auto json = json11::Json::parse(ss.str(), err);
                    if (err.empty())
                    {
                        cardConfigs.clear();
                        for (auto& item : json.array_items())
                        {
                            CardConfig c;
                            c.type = item["type"].int_value();
                            c.value = item["value"].int_value();
                            cardConfigs.push_back(c);
                        }
                    }
                }
            }
            {
                std::ifstream ifs("resource/json/enemy_config.json");
                if (ifs.is_open())
                {
                    std::stringstream ss;
                    ss << ifs.rdbuf();
                    std::string err;
                    auto json = json11::Json::parse(ss.str(), err);
                    if (err.empty())
                    {
                        enemyConfigs.clear();
                        for (auto& item : json.array_items())
                        {
                            EnemyConfig e;
                            e.baseWeight = item["baseWeight"].int_value();
                            e.rockDamage = item["rockDamage"].int_value();
                            e.scissorsDamage = item["scissorsDamage"].int_value();
                            e.paperDamage = item["paperDamage"].int_value();
                            std::string s = item["name"].string_value();
                            size_t len = mbstowcs(nullptr, s.c_str(), 0);
                            if (len != static_cast<size_t>(-1))
                            {
                                std::vector<wchar_t> buf(len + 1);
                                mbstowcs(buf.data(), s.c_str(), len + 1);
                                e.name = buf.data();
                            }
                            enemyConfigs.push_back(e);
                        }
                    }
                }
            }
        }

        void LoadImages()
        {
            try
            {
                struct ImageData {
                    EImage id;
                    const wchar_t* path;
                };
                ImageData images[] = {
                    {EImage::Rock, FILE_PATH_PNG_STONE},
                    {EImage::Scissors, FILE_PATH_PNG_SCISSORS},
                    {EImage::Paper, FILE_PATH_PNG_PAPER},
                    {EImage::Magic, FILE_PATH_PNG_POINT},
                    {EImage::KB_Q, FILE_PATH_PNG_KEYBOARD_Q_OUTLINE},
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

                struct SpriteData {
                    ESprite id;
                    const wchar_t* path;
                };
                SpriteData sprites[] = {
                    {ESprite::Bunny, FILE_PATH_PNG_MINIBUNNY},
                    {ESprite::Wolf, FILE_PATH_PNG_MINIWOLF}
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
    };

    Shared<IAssetService> CreateAssetService()
    {
        return std::make_shared<AssetService>();
    }
} // namespace mc
