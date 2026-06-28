module;

#include <vector>
#include <memory>
#include <cassert>
#include <algorithm>
#include <ranges>
#include <string>
#include <unordered_map>
#include <RandomUtils.h>

module BattleService;

import HealthComponent;
import Character;
import AssetService;

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

    class BattleService : public IBattleService
    {
    public:
        BattleService(
            IConfigService& configService,
            ICardService& cardService
        )
            : configService(configService)
            , cardService(cardService)
        {
            deathHandle = EventBus::Subscribe<DeathEvent>([this](const DeathEvent& e)
            {
                OnDeathEvent(e);
            });
        }

        ~BattleService() override
        {
            EventBus::Unsubscribe(deathHandle);
        }

        void StartStage() override
        {
            auto& pool = configService.GetEnemyConfigs();
            assert(!pool.empty());

            sequence.clear();

            if (pool.size() >= 3)
            {
                std::vector<EnemyConfig> copy(pool.begin(), pool.end());
                Random::Shuffle(copy);
                sequence.assign(copy.begin(), copy.begin() + 3);
            }
            else
            {
                // pool is small — pick with replacement using a mutable copy
                std::vector<EnemyConfig> mutablePool(pool.begin(), pool.end());
                while (sequence.size() < 3)
                {
                    sequence.push_back(Random::Choice(mutablePool));
                }
            }

            currentIndex = 0;

            // Create a fresh Player, then load the first enemy of the sequence.
            currentPlayer = std::make_unique<Player>();
            LoadEnemy(sequence[0]);
            cardService.Start();
            EventBus::Publish(StageStartedEvent{});
        }

        int GetCurrentEnemyIndex() const override
        {
            return currentIndex;
        }

        const std::vector<EnemyConfig>& GetSequence() const override
        {
            return sequence;
        }

        void LoadEnemy(const EnemyConfig& config) override
        {
            ESprite sprite = ParseSprite(config.spriteName);
            if (currentEnemy)
            {
                // Overwrite the existing enemy's data in place rather than
                // destroying and recreating it, so references held elsewhere
                // (HealthComponent owner, views, etc.) remain valid.
                currentEnemy->Reset(
                    config.baseWeight, config.rockDamage, config.scissorsDamage, config.paperDamage,
                    config.name, sprite, config.hp
                );
            }
            else
            {
                currentEnemy = std::make_unique<Enemy>(
                    config.baseWeight, config.rockDamage, config.scissorsDamage, config.paperDamage,
                    config.name, sprite, config.hp
                );
            }
        }

        Enemy& GetEnemy() override
        {
            return *currentEnemy;
        }

        Player& GetPlayer() override
        {
            return *currentPlayer;
        }

    private:
        void OnDeathEvent(const DeathEvent& e)
        {
            if (e.Victim == nullptr) return;

            const auto& tags = e.Victim->GetTags();
            bool isEnemy  = std::ranges::contains(tags, ETag::Enemy);
            bool isPlayer = std::ranges::contains(tags, ETag::Player);

            if (isEnemy)
            {
                if (currentIndex < 2)
                {
                    int prevIndex = currentIndex;
                    currentIndex++;
                    // Load the next enemy first so listeners of EnemyDefeatedEvent
                    // (e.g. the combat view) observe the updated enemy data.
                    LoadEnemy(sequence[currentIndex]);
                    EventBus::Publish(EnemyDefeatedEvent(prevIndex));
                }
                else // currentIndex == 2
                {
                    EventBus::Publish(StageClearEvent{});
                    sequence.clear();
                    currentIndex = 0;
                }
            }
            else if (isPlayer)
            {
                EventBus::Publish(StageFailEvent{});
                sequence.clear();
                currentIndex = 0;
            }
        }

        IConfigService&    configService;
        ICardService&      cardService;

        std::vector<EnemyConfig> sequence;
        int currentIndex = 0;
        EventHandle deathHandle;

        std::unique_ptr<Enemy>  currentEnemy;
        std::unique_ptr<Player> currentPlayer;
    };

    std::unique_ptr<IBattleService> CreateBattleService(
        IConfigService& configService,
        ICardService& cardService
    )
    {
        return std::make_unique<BattleService>(
            configService, cardService);
    }

} // namespace mc
