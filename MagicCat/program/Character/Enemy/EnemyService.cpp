module;

#include <vector>
#include <memory>
#include <string>

module EnemyService;

import ServiceLocator;
import Enemy;

const Enemy ENEMY_NUT = Enemy{5, 3, 1, 1, L"ナッツ"};
const Enemy ENEMY_CABBAGE = Enemy{5, 1, 1, 3, L"キャベツ"};
const Enemy ENEMY_KNIFE = Enemy{3, 1, 5, 1, L"包丁"};

class EnemyPool: public IEnemyPool
{
public:
    EnemyPool(){
        enemies.push_back(ENEMY_NUT);
        enemies.push_back(ENEMY_CABBAGE);
        enemies.push_back(ENEMY_KNIFE);
    }
    
    Enemy GetEnemy() override
    {
        if(enemies.empty()) return Enemy{};
        auto em = enemies.back();
        enemies.pop_back();
        return em;
    }
    
private:
    std::vector<Enemy> enemies;
};

static struct RegisterEnemyPool {
    RegisterEnemyPool() {
        ServiceLocator::RegisterSingleton<IEnemyPool, EnemyPool>(std::make_shared<EnemyPool>());
    }
} autoRegister_EnemyPool;