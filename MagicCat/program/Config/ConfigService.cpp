module;

#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>
#include <optional>
#include <windows.h>
#include "json11.hpp"

module ConfigService;

namespace mc {
    class ConfigService : public IConfigService
    {
    public:
        ConfigService(const std::string& cardConfigPath, const std::string& enemyConfigPath, const std::string& gameConfigPath)
        {
            LoadGameConfig(cardConfigPath, enemyConfigPath, gameConfigPath);
        }

        const std::vector<CardConfig>& GetCardConfigs() const override  { return cardConfigs; }
        const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemyConfigs; }
        const PlayerConfig& GetPlayerConfig() const override { return playerConfig; }
        const GameConfig& GetGameConfig() const override { return gameConfig; }

        bool LoadSoundSettings(int& masterVolume, int& bgmVolume, int& sfxVolume) override
        {
            std::ifstream ifs("resource/sound_settings.json");
            if (!ifs.is_open())
                return false;

            std::stringstream ss;
            ss << ifs.rdbuf();
            std::string err;
            auto json = json11::Json::parse(ss.str(), err);
            if (!err.empty())
                return false;

            masterVolume = json["masterVolume"].int_value();
            bgmVolume    = json["bgmVolume"].int_value();
            sfxVolume    = json["sfxVolume"].int_value();
            return true;
        }

        bool SaveSoundSettings(int masterVolume, int bgmVolume, int sfxVolume) override
        {
            json11::Json data = json11::Json::object{
                { "masterVolume", masterVolume },
                { "bgmVolume",    bgmVolume    },
                { "sfxVolume",    sfxVolume    }
            };
            std::ofstream ofs("resource/sound_settings.json");
            if (!ofs.is_open())
                return false;
            ofs << data.dump();
            return !ofs.fail();
        }

        bool SaveGame(int slot, const GameState& state) override
        {
            assert(slot >= 0 && slot < SAVE_SLOT_COUNT);

            auto toCardArray = [](const std::vector<CardData>& pile) -> json11::Json::array {
                json11::Json::array arr;
                for (const auto& cd : pile)
                    arr.push_back(json11::Json::object{ {"type", cd.type}, {"power", cd.power} });
                return arr;
            };

            json11::Json::array seqArr;
            for (int s : state.sequence)
                seqArr.push_back(s);

            json11::Json data = json11::Json::object{
                { "currentIndex", state.currentIndex },
                { "totalBattles", (int)state.sequence.size() },
                { "player", json11::Json::object{
                    { "hp",                  state.playerHp },
                    { "maxHp",               state.playerMaxHp },
                    { "mp",                  state.playerMp },
                    { "sprite",              state.playerSprite },
                    { "healUses",            state.playerHealUses },
                    { "hasUsedClairvoyance", state.playerHasUsedClairvoyance ? 1 : 0 }
                }},
                { "enemy", json11::Json::object{
                    { "hp",             state.enemyHp },
                    { "maxHp",          state.enemyMaxHp },
                    { "sprite",         state.enemySprite },
                    { "rockOffset",     state.enemyRockOffset },
                    { "scissorsOffset", state.enemyScissorsOffset },
                    { "paperOffset",    state.enemyPaperOffset }
                }},
                { "sequence", json11::Json(seqArr) },
                { "cards", json11::Json::object{
                    { "hand",        toCardArray(state.hand) },
                    { "drawPile",    toCardArray(state.drawPile) },
                    { "discardPile", toCardArray(state.discardPile) }
                }}
            };

            std::string path = "resource/save" + std::to_string(slot) + ".json";
            std::ofstream ofs(path);
            if (!ofs.is_open()) return false;
            ofs << data.dump();
            return !ofs.fail();
        }

        std::optional<GameState> LoadGame(int slot) override
        {
            assert(slot >= 0 && slot < SAVE_SLOT_COUNT);
            std::string path = "resource/save" + std::to_string(slot) + ".json";
            std::ifstream ifs(path);
            if (!ifs.is_open()) return std::nullopt;
            std::stringstream ss;
            ss << ifs.rdbuf();
            std::string err;
            auto json = json11::Json::parse(ss.str(), err);
            if (!err.empty()) return std::nullopt;

            GameState state;
            state.currentIndex = json["currentIndex"].int_value();

            const auto& p = json["player"];
            state.playerHp                  = p["hp"].int_value();
            state.playerMaxHp               = p["maxHp"].int_value();
            state.playerMp                  = p["mp"].int_value();
            state.playerSprite              = p["sprite"].int_value();
            state.playerHealUses            = p["healUses"].int_value();
            state.playerHasUsedClairvoyance = p["hasUsedClairvoyance"].int_value() != 0;

            const auto& e = json["enemy"];
            state.enemyHp             = e["hp"].int_value();
            state.enemyMaxHp          = e["maxHp"].int_value();
            state.enemySprite         = e["sprite"].int_value();
            state.enemyRockOffset     = e["rockOffset"].int_value();
            state.enemyScissorsOffset = e["scissorsOffset"].int_value();
            state.enemyPaperOffset    = e["paperOffset"].int_value();

            for (const auto& item : json["sequence"].array_items())
                state.sequence.push_back(item.int_value());

            auto fromCardArray = [](const json11::Json& arr) {
                std::vector<CardData> pile;
                for (const auto& item : arr.array_items())
                    pile.push_back({ item["type"].int_value(), item["power"].int_value() });
                return pile;
            };
            const auto& cards = json["cards"];
            state.hand        = fromCardArray(cards["hand"]);
            state.drawPile    = fromCardArray(cards["drawPile"]);
            state.discardPile = fromCardArray(cards["discardPile"]);

            return state;
        }

        SaveMetadata GetSaveMetadata(int slot) override
        {
            auto stateOpt = LoadGame(slot);
            if (!stateOpt.has_value())
                return SaveMetadata{ false };

            const auto& s = *stateOpt;
            SaveMetadata meta;
            meta.exists        = true;
            meta.currentBattle = s.currentIndex + 1;
            meta.totalBattles  = static_cast<int>(s.sequence.size());
            meta.playerHp      = s.playerHp;
            meta.playerMaxHp   = s.playerMaxHp;
            meta.playerSprite  = s.playerSprite;
            meta.enemyHp       = s.enemyHp;
            meta.enemyMaxHp    = s.enemyMaxHp;
            meta.enemySprite   = s.enemySprite;
            return meta;
        }

    private:
        void LoadGameConfig(const std::string& cardConfigPath, const std::string& enemyConfigPath, const std::string& gameConfigPath)
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
                e.name           = ParseUtf8Name(item["name"].string_value());
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

    std::unique_ptr<IConfigService> CreateConfigService(const std::string& cardConfigPath,
                                                        const std::string& enemyConfigPath,
                                                        const std::string& gameConfigPath)
    {
        return std::make_unique<ConfigService>(cardConfigPath, enemyConfigPath, gameConfigPath);
    }
} // namespace mc
