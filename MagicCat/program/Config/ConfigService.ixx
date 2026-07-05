module;

#include <string>
#include <vector>
#include <memory>

export module ConfigService;

namespace mc {
    export struct EnemyConfig
    {
        int hp;
        int baseWeight;
        int rockDamage;
        int scissorsDamage;
        int paperDamage;
        std::wstring name;
        std::string spriteName;
    };

    export struct CardConfig
    {
        int type;
        int value;
    };

    export struct PlayerConfig
    {
        int initialHp;
        int maxMp;
        int rockDamage;
        int scissorsDamage;
        int paperDamage;
        std::string spriteName;
        int clairvoyanceMpCost;
        int powerBoostMpCost;
        int powerBoostDamageOffset;
        int healMpCost;
        int healAmount;
        int maxHealUses;
    };

    export struct GameConfig
    {
        int battleCount;
    };

    export class IConfigService
    {
    public:
        virtual ~IConfigService() = default;
        virtual const std::vector<CardConfig>& GetCardConfigs() const = 0;
        virtual const std::vector<EnemyConfig>& GetEnemyConfigs() const = 0;
        virtual const PlayerConfig& GetPlayerConfig() const = 0;
        virtual const GameConfig& GetGameConfig() const = 0;
    };

    export std::unique_ptr<IConfigService> CreateConfigService(const std::string& cardConfigPath,
                                                               const std::string& enemyConfigPath,
                                                               const std::string& gameConfigPath);
} // namespace mc
