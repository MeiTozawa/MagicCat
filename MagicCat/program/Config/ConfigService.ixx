module;

#include <string>
#include <vector>

export module ConfigService;

import ServiceLocator;

namespace mc
{
    export struct EnemyConfig
    {
        int baseWeight;
        int rockDamage;
        int scissorsDamage;
        int paperDamage;
        std::wstring name;
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

    export Shared<IConfigService> CreateConfigService();
} // namespace mc
