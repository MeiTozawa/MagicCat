module;

#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "json11.hpp"

module ConfigService;
import ServiceLocator;

namespace mc
{
    class ConfigService : public IConfigService
    {
        std::vector<CardConfig> cardConfigs;
        std::vector<EnemyConfig> enemyConfigs;

    public:
        ConfigService()
        {
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

    private:
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
    };

    Shared<IConfigService> CreateConfigService()
    {
        return std::make_shared<ConfigService>();
    }
} // namespace mc
