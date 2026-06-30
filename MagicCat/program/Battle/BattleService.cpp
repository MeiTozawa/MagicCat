module;

#include <vector>
#include <memory>
#include <cassert>
#include <algorithm>
#include <ranges>
#include <string>
#include <RandomUtils.h>

module BattleService;

import HealthComponent;
import Character;
import AssetService;

namespace mc {
    class BattleService : public IBattleService
    {
    public:
        BattleService(
            IConfigService& configService,
            ICardService& cardService,
            IAssetService& assetService
        )
            : configService(configService)
              , cardService(cardService)
              , assetService(assetService)
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
                // プールが少ない場合は重複を許して補充する
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
            ESprite sprite = assetService.ParseSprite(config.spriteName);
            if (currentEnemy)
            {
                // 既存インスタンスのデータを上書きし、外部からの参照を維持する
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

        int GetTotalEnemyCount() const override
        {
            return static_cast<int>(sequence.size());
        }

    private:
        void OnDeathEvent(const DeathEvent& e)
        {
            if (e.Victim == nullptr) return;

            const auto& tags = e.Victim->GetTags();
            bool isEnemy = std::ranges::contains(tags, ETag::Enemy);
            bool isPlayer = std::ranges::contains(tags, ETag::Player);

            if (isEnemy)
            {
                if (currentIndex < 2)
                {
                    int prevIndex = currentIndex;
                    currentIndex++;
                    // 次の敵を先に読み込んでから EnemyDefeatedEvent を発行する（ビュー側が更新済みの敵データを参照できる）
                    LoadEnemy(sequence[currentIndex]);
                    EventBus::Publish(EnemyDefeatedEvent(prevIndex));
                }
                else
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

        IConfigService& configService;
        ICardService& cardService;
        IAssetService& assetService;

        std::vector<EnemyConfig> sequence;
        int currentIndex = 0;
        EventHandle deathHandle;

        std::unique_ptr<Enemy> currentEnemy;
        std::unique_ptr<Player> currentPlayer;
    };

    std::unique_ptr<IBattleService> CreateBattleService(
        IConfigService& configService,
        ICardService& cardService,
        IAssetService& assetService
    )
    {
        return std::make_unique<BattleService>(
            configService, cardService, assetService);
    }
} // namespace mc
