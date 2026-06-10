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
        void AssetService()
        {
            LoadImages();
            LoadGameConfig();
        }

        const std::vector<CardConfig>& GetCardConfigs() const override
        {
            return cardConfigs;
        }

        const std::vector<EnemyConfig>& GetEnemyConfigs() const override
        {
            return enemyConfigs;
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
        std::vector<CardConfig> cardConfigs;
        std::vector<EnemyConfig> enemyConfigs;

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
                auto rock_resource = dxe::SpriteResouce::Create(FILE_PATH_PNG_STONE);
                if (!rock_resource)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_STONE);
                else
                    imageMappings.insert({EImage::Rock, dxe::Sprite::Create(rock_resource)});

                auto scissors_resource = dxe::SpriteResouce::Create(FILE_PATH_PNG_SCISSORS);
                if (!scissors_resource)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_SCISSORS);
                else
                    imageMappings.insert({EImage::Scissors, dxe::Sprite::Create(scissors_resource)});

                auto paper_resource = dxe::SpriteResouce::Create(FILE_PATH_PNG_PAPER);
                if (!paper_resource)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_PAPER);
                else
                    imageMappings.insert({EImage::Paper, dxe::Sprite::Create(paper_resource)});

                auto magic_resource = dxe::SpriteResouce::Create(FILE_PATH_PNG_POINT);
                if (!magic_resource)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_POINT);
                else
                    imageMappings.insert({EImage::Magic, dxe::Sprite::Create(magic_resource)});

                auto kb_q_resource = dxe::SpriteResouce::Create(FILE_PATH_PNG_KEYBOARD_Q_OUTLINE);
                if (!kb_q_resource)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_KEYBOARD_Q_OUTLINE);
                else
                    imageMappings.insert({EImage::KB_Q, dxe::Sprite::Create(kb_q_resource)});

                auto kb_space_resource = dxe::SpriteResouce::Create(FILE_PATH_PNG_KEYBOARD_SPACE_OUTLINE);
                if (!kb_space_resource)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_KEYBOARD_SPACE_OUTLINE);
                else
                    imageMappings.insert({EImage::KB_SPACE, dxe::Sprite::Create(kb_space_resource)});

                auto kb_up_resource = dxe::SpriteResouce::Create(FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE);
                if (!kb_up_resource)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_KEYBOARD_ARROW_UP_OUTLINE);
                else
                    imageMappings.insert({EImage::KB_UP, dxe::Sprite::Create(kb_up_resource)});

                auto kb_down_resource = dxe::SpriteResouce::Create(FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE);
                if (!kb_down_resource)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_KEYBOARD_ARROW_DOWN_OUTLINE);
                else
                    imageMappings.insert({EImage::KB_DOWN, dxe::Sprite::Create(kb_down_resource)});


                auto bunny_resource = LoadGraph(FILE_PATH_PNG_MINIBUNNY);
                if (bunny_resource == -1)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_MINIBUNNY);
                else
                    spriteMappings.insert({ESprite::Bunny, bunny_resource});

                auto wolf_resource = LoadGraph(FILE_PATH_PNG_MINIWOLF);
                if (wolf_resource == -1)
                    printfDx(L"%sの読み込みに失敗", FILE_PATH_PNG_MINIWOLF);
                else
                    spriteMappings.insert({ESprite::Wolf, wolf_resource});
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
