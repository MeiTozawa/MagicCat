module;

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <RandomUtils.h>

module CharacterService;

import ServiceLocator;
import Enemy;
import AssetService;
import ConfigService;

namespace mc
{
    ESprite ParseSprite(const std::string& name)
    {
        static const std::unordered_map<std::string, ESprite> spriteMap = {
            {"Bunny", ESprite::Bunny},
            {"Wolf", ESprite::Wolf},
            {"CluckingChicken", ESprite::CluckingChicken},
            {"CoralCrab", ESprite::CoralCrab},
            {"CroakingToad", ESprite::CroakingToad},
            {"DaintyPig", ESprite::DaintyPig},
            {"HonkingGoose", ESprite::HonkingGoose},
            {"LeapingFrog", ESprite::LeapingFrog},
            {"MadBoar", ESprite::MadBoar},
            {"MeowingCat", ESprite::MeowingCat},
            {"PasturingSheep", ESprite::PasturingSheep},
            {"SlowTurtle", ESprite::SlowTurtle},
            {"SnowFox", ESprite::SnowFox},
            {"SpikeyPorcupine", ESprite::SpikeyPorcupine},
            {"StinkySkunk", ESprite::StinkySkunk},
            {"TimberWolf", ESprite::TimberWolf},
            {"TinyChick", ESprite::TinyChick}
        };

        if (auto it = spriteMap.find(name); it != spriteMap.end())
        {
            return it->second;
        }
        return ESprite::Null;
    }

    class CharacterService : public ICharacterService
    {
    public:
        CharacterService()
        {
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
            ESprite sprite = ParseSprite(e.spriteName);
            currentEnemy = std::make_unique<Enemy>(
                e.baseWeight, e.rockDamage, e.scissorsDamage, e.paperDamage,
                e.name.c_str(), sprite
            );
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
