module;

#include <vector>
#include <memory>
#include <string>

module CharacterService;

import ServiceLocator;
import Enemy;

const Enemy ENEMY_NUT = Enemy{5, 3, 1, 1, L"ナッツ"};
const Enemy ENEMY_CABBAGE = Enemy{5, 1, 1, 3, L"キャベツ"};
const Enemy ENEMY_KNIFE = Enemy{3, 1, 5, 1, L"包丁"};

class CharacterService : public ICharacterService
{
public:
    CharacterService()
    {
        //
    }

    const Enemy& GetEnemy() override
    {
        return currentEnemy;
    }

    const Player& GetPlayer() override
    {
        return currentPlayer;
    }

    void Reset() override
    {
        enemies.clear();
        enemies.push_back(ENEMY_NUT);
        enemies.push_back(ENEMY_CABBAGE);
        enemies.push_back(ENEMY_KNIFE);

        currentPlayer = Player{};

        NextEnemy();
    }

    bool NextEnemy() override
    {
        if (enemies.empty()) return false;
        currentEnemy = enemies.back();
        enemies.pop_back();
        return true;
    }

private:
    std::vector<Enemy> enemies;
    Enemy currentEnemy;
    Player currentPlayer;
};

static struct RegisterCharacterService
{
    RegisterCharacterService()
    {
        ServiceLocator::RegisterSingleton<ICharacterService, CharacterService>(std::make_shared<CharacterService>());
    }
} autoRegister_CharacterService;
