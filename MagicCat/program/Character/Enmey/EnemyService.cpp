module;

#include <memory>
#include <vector>

#include "../../RandomUtils.h"

module EnemyService;

import Enmey;
import ServiceLocator;

class EnemyPool: public IEnemyPool
{
public:
    EnemyPool(){
        enemies.push_back(ENEMY_NUT);
        enemies.push_back(ENEMY_NUT);
        enemies.push_back(ENEMY_NUT);
        enemies.push_back(ENEMY_CABBAGE);
        enemies.push_back(ENEMY_CABBAGE);
        enemies.push_back(ENEMY_CABBAGE);
        enemies.push_back(ENEMY_KNIFE);
        enemies.push_back(ENEMY_KNIFE);
        
        Random::Shuffle(enemies);
    }
    
    Enemy GetEnemy() override
    {
        auto em = enemies.back();
        enemies.pop_back();
        return em;
    }
    
    private:
    std::vector<Enemy> enemies;
};

template <>
struct std::hash<Enemy>
{
    size_t operator()(const Enemy& b) const noexcept
    {
        const size_t h1 = std::hash<std::wstring>{}(b.name);
        const size_t h2 = std::hash<int>{}(b.baseWeight);
        const size_t h3 = std::hash<int>{}(b.rockDamage);
        const size_t h4 = std::hash<int>{}(b.scissorsDamage);
        const size_t h5 = std::hash<int>{}(b.paperDamage);

        size_t seed = h1;
        seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h5 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

static struct RegisterEnemyPool {
    RegisterEnemyPool() {
        ServiceLocator::RegisterSingleton<IEnemyPool, EnemyPool>(std::make_shared<EnemyPool>());
    }
} autoRegister_EnemyPool;
