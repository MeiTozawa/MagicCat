module;

#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <windows.h>
#include "json11.hpp"

module ConfigService;

namespace mc {
    class ConfigService : public IConfigService
    {
    public:
        ConfigService(const std::string& cardConfigPath, const std::string& enemyConfigPath)
        {
            LoadGameConfig(cardConfigPath, enemyConfigPath);
        }

        const std::vector<CardConfig>& GetCardConfigs() const override  { return cardConfigs; }
        const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemyConfigs; }

    private:
        void LoadGameConfig(const std::string& cardConfigPath, const std::string& enemyConfigPath)
        {
            LoadCardConfig(cardConfigPath);
            LoadEnemyConfig(enemyConfigPath);
        }

        void LoadCardConfig(const std::string& path)
        {
            std::ifstream ifs(path);
            if (!ifs.is_open())
            {
                assert(false && "Card JSON ファイルが開けませんでした");
                return;
            }
            std::stringstream ss;
            ss << ifs.rdbuf();
            std::string err;
            auto json = json11::Json::parse(ss.str(), err);
            if (!err.empty())
            {
                assert(false && "Card JSON の解析に失敗しました");
                return;
            }
            cardConfigs.clear();
            for (auto& item : json.array_items())
            {
                CardConfig c;
                c.type  = item["type"].int_value();
                c.value = item["value"].int_value();
                cardConfigs.push_back(c);
            }
        }

        void LoadEnemyConfig(const std::string& path)
        {
            std::ifstream ifs(path);
            if (!ifs.is_open())
            {
                assert(false && "Enemy JSON ファイルが開けませんでした");
                return;
            }
            std::stringstream ss;
            ss << ifs.rdbuf();
            std::string err;
            auto json = json11::Json::parse(ss.str(), err);
            if (!err.empty())
            {
                assert(false && "Enemy JSON の解析に失敗しました");
                return;
            }
            enemyConfigs.clear();
            for (auto& item : json.array_items())
            {
                EnemyConfig e;
                e.hp             = item["hp"].int_value();
                e.baseWeight     = item["baseWeight"].int_value();
                e.rockDamage     = item["rockDamage"].int_value();
                e.scissorsDamage = item["scissorsDamage"].int_value();
                e.paperDamage    = item["paperDamage"].int_value();
                e.spriteName     = item["sprite"].string_value();
                std::string s    = item["name"].string_value();
                int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
                if (len > 0)
                {
                    std::vector<wchar_t> buf(len);
                    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buf.data(), len);
                    e.name = buf.data();
                }
                enemyConfigs.push_back(e);
            }
        }

        std::vector<CardConfig> cardConfigs;
        std::vector<EnemyConfig> enemyConfigs;
    };

    std::unique_ptr<IConfigService> CreateConfigService(const std::string& cardConfigPath,
                                                        const std::string& enemyConfigPath)
    {
        return std::make_unique<ConfigService>(cardConfigPath, enemyConfigPath);
    }
} // namespace mc
