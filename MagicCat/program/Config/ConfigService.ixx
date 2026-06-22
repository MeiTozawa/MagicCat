module;

#include <string>
#include <vector>

export module ConfigService;

namespace mc
{
    export struct EnemyConfig
    {
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

    export class IConfigService
    {
    public:
        virtual ~IConfigService() = default;
        virtual const std::vector<CardConfig>& GetCardConfigs() const = 0;
        virtual const std::vector<EnemyConfig>& GetEnemyConfigs() const = 0;
    };

    export std::unique_ptr<IConfigService> CreateConfigService(const std::string& cardConfigPath, const std::string& enemyConfigPath);
} // namespace mc
