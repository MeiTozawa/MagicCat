module;

#include <vector>
#include <memory>
#include <RandomUtils.h>

module CharacterService;

import ServiceLocator;
import Enemy;
import AssetService;
import ConfigService;

namespace mc {

class CharacterService : public ICharacterService
{
public:
    CharacterService() {        
       enemies = ServiceLocator::Get<IConfigService>()->GetEnemyConfigs();

        currentPlayer = std::make_unique<Player>();
    }

    Enemy& GetEnemy() override
    {
        return *currentEnemy;
    }

    Player& GetPlayer() override
    {
        return *currentPlayer;
    }

    bool NextEnemy() override
    {
        if (enemies.empty()) return false;
        Random::Shuffle(enemies);
        auto e = enemies.front();
        currentEnemy = std::make_unique<Enemy>(e.baseWeight, e.rockDamage, e.scissorsDamage, e.paperDamage, e.name.c_str());
        enemies.pop_back();
        return true;
    }

private:
    std::vector<EnemyConfig> enemies;
    std::unique_ptr<Enemy> currentEnemy;
    std::unique_ptr<Player> currentPlayer;
};

Shared<ICharacterService> CreateCharacterService()
{
    return std::make_shared<CharacterService>();
}

} // namespace mc

