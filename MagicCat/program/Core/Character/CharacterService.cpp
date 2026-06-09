module;

#include <vector>
#include <memory>

module CharacterService;

import ServiceLocator;
import Enemy;

constexpr auto ENEMY_NUT = std::make_tuple(5, 3, 1, 1, L"ナッツ");
constexpr auto ENEMY_CABBAGE = std::make_tuple(5, 1, 1, 3, L"キャベツ");
constexpr auto ENEMY_KNIFE = std::make_tuple(3, 1, 5, 1, L"包丁");

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
        SpawnEnemies(ENEMY_NUT, ENEMY_CABBAGE, ENEMY_KNIFE);

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
    
    template <typename... Tuples>
    void SpawnEnemies(Tuples&&... tuples) {
        (std::apply([&enemies = this->enemies]<typename... T>(T&&... args) {
            enemies.push_back(std::make_unique<Enemy>(std::forward<T>(args)...));
        }, std::forward<Tuples>(tuples)), ...);
    }
};

static struct RegisterCharacterService
{
    RegisterCharacterService()
    {
        ServiceLocator::RegisterSingleton<ICharacterService, CharacterService>(std::make_shared<CharacterService>());
    }
} autoRegister_CharacterService;
