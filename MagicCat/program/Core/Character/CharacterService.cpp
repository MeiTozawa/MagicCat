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
        return currentEnemy;
    }

    Player& GetPlayer() override
    {
        return currentPlayer;
    }

    void Reset() override
    {
        enemies.clear();
        SpawnEnemies(ENEMY_NUT, ENEMY_CABBAGE, ENEMY_KNIFE);

        currentPlayer = Player{};

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
    std::vector<Enemy> enemies;
    Enemy currentEnemy;
    Player currentPlayer;
    
    template <typename... Tuples>
    void SpawnEnemies(Tuples&&... tuples) {
        // ⚠️ 高能预警：C++17 逗号折叠表达式
        // 它会自动为你传入的每一个 tuple 展开并执行一遍 std::apply
        (std::apply([&enemies = this->enemies]<typename... T>(T&&... args) {
            enemies.emplace_back(std::forward<T>(args)...);
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
