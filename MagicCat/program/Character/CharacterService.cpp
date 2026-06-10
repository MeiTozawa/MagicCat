module;

#include <vector>
#include <memory>

module CharacterService;

import ServiceLocator;
import Enemy;
import AssetService;

namespace mc {

class CharacterService : public ICharacterService
{
public:
    CharacterService() {}

    Enemy& GetEnemy() override
    {
        return *currentEnemy;
    }

    Player& GetPlayer() override
    {
        return *currentPlayer;
    }

    void Reset() override
    {
        enemies.clear();
        auto& config = ServiceLocator::Get<IAssetService>()->GetEnemyConfigs();
        for (const auto& e : config)
        {
            enemies.push_back(std::make_unique<Enemy>(e.baseWeight, e.rockDamage, e.scissorsDamage, e.paperDamage, e.name.c_str()));
        }

        currentPlayer = std::make_unique<Player>();

        NextEnemy();
    }

    bool NextEnemy() override
    {
        if (enemies.empty()) return false;
        currentEnemy = std::move(enemies.back());
        enemies.pop_back();
        return true;
    }

private:
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::unique_ptr<Enemy> currentEnemy;
    std::unique_ptr<Player> currentPlayer;
};

Shared<ICharacterService> CreateCharacterService()
{
    return std::make_shared<CharacterService>();
}

} // namespace mc

