module;

#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>
#include <windows.h>
#include "json11.hpp"

module ConfigService;

namespace mc {
    class ConfigService : public IConfigService
    {
    public:
        ConfigService(const std::string& cardConfigPath,
                      const std::string& enemyConfigPath,
                      const std::string& gameConfigPath)
        {
            LoadGameConfig(cardConfigPath, enemyConfigPath, gameConfigPath);
        }

        const std::vector<CardConfig>& GetCardConfigs() const override { return cardConfigs; }
        const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemyConfigs; }
        const PlayerConfig& GetPlayerConfig() const override { return playerConfig; }
        const GameConfig& GetGameConfig() const override { return gameConfig; }

    private:
        void LoadGameConfig(const std::string& cardConfigPath,
                            const std::string& enemyConfigPath,
                            const std::string& gameConfigPath)
        {
            LoadCardConfig(cardConfigPath);
            LoadEnemyConfig(enemyConfigPath);
            LoadGameSettingsConfig(gameConfigPath);
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
                c.type = item["type"].int_value();
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
                e.hp = item["hp"].int_value();
                e.baseWeight = item["baseWeight"].int_value();
                e.rockDamage = item["rockDamage"].int_value();
                e.scissorsDamage = item["scissorsDamage"].int_value();
                e.paperDamage = item["paperDamage"].int_value();
                e.spriteName = item["sprite"].string_value();
                e.name = ParseUtf8Name(item["name"].string_value());
                enemyConfigs.push_back(e);
            }
        }

        void LoadGameSettingsConfig(const std::string& path)
        {
            std::ifstream ifs(path);
            if (!ifs.is_open())
            {
                assert(false && "Game Settings JSON ファイルが開けませんでした");
                return;
            }
            std::stringstream ss;
            ss << ifs.rdbuf();
            std::string err;
            auto json = json11::Json::parse(ss.str(), err);
            if (!err.empty())
            {
                assert(false && "Game Settings JSON の解析に失敗しました");
                return;
            }

            auto stageJson = json["stage"];
            gameConfig.battleCount = stageJson["battleCount"].int_value();

            auto playerJson = json["player"];
            playerConfig.initialHp = playerJson["initialHp"].int_value();
            playerConfig.maxMp = playerJson["maxMp"].int_value();
            playerConfig.spriteName = playerJson["sprite"].string_value();

            auto damageJson = playerJson["damage"];
            playerConfig.rockDamage = damageJson["rock"].int_value();
            playerConfig.scissorsDamage = damageJson["scissors"].int_value();
            playerConfig.paperDamage = damageJson["paper"].int_value();

            auto magicJson = playerJson["magic"];
            auto clairvoyanceJson = magicJson["clairvoyance"];
            playerConfig.clairvoyanceMpCost = clairvoyanceJson["mpCost"].int_value();

            auto powerBoostJson = magicJson["powerBoost"];
            playerConfig.powerBoostMpCost = powerBoostJson["mpCost"].int_value();
            playerConfig.powerBoostDamageOffset = powerBoostJson["damageOffset"].int_value();

            auto healJson = magicJson["heal"];
            playerConfig.healMpCost = healJson["mpCost"].int_value();
            playerConfig.healAmount = healJson["healAmount"].int_value();
            playerConfig.maxHealUses = healJson["maxUses"].int_value();
        }

        static std::wstring ParseUtf8Name(const std::string& s)
        {
            int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
            if (len <= 0) return {};
            std::vector<wchar_t> buf(len);
            MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buf.data(), len);
            return buf.data();
        }

        std::vector<CardConfig> cardConfigs;
        std::vector<EnemyConfig> enemyConfigs;
        PlayerConfig playerConfig;
        GameConfig gameConfig;
    };

    std::unique_ptr<IConfigService> CreateConfigService(
        const std::string& cardConfigPath, const std::string& enemyConfigPath, const std::string& gameConfigPath)
    {
        return std::make_unique<ConfigService>(cardConfigPath, enemyConfigPath, gameConfigPath);
    }
} // namespace mc
