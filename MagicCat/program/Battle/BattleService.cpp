module;

#include <vector>
#include <memory>
#include <cassert>
#include <algorithm>
#include <ranges>
#include <string>
#include <optional>
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
            IPersistenceService& persistenceService,
            ICardService& cardService,
            IAssetService& assetService
        )
            : configService(configService)
              , persistenceService(persistenceService)
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

            int battleCount = configService.GetGameConfig().battleCount;

            if (pool.size() >= battleCount)
            {
                std::vector<EnemyConfig> copy(pool.begin(), pool.end());
                Random::Shuffle(copy);
                sequence.assign(copy.begin(), copy.begin() + battleCount);
            }
            else
            {
                std::vector<EnemyConfig> mutablePool(pool.begin(), pool.end());
                while (sequence.size() < battleCount)
                    sequence.push_back(Random::Choice(mutablePool));
            }

            currentIndex = 0;
            const auto& playerConfig = configService.GetPlayerConfig();
            ESprite playerSprite = assetService.ParseSprite(playerConfig.spriteName);
            currentPlayer = std::make_unique<Player>(playerConfig, playerSprite);
            LoadEnemy(sequence[0]);
            cardService.Start();
            EventBus::Publish(StageStartedEvent{});
        }

        int GetCurrentEnemyIndex() const override { return currentIndex; }

        const std::vector<EnemyConfig>& GetSequence() const override { return sequence; }

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

        Enemy& GetEnemy() override { return *currentEnemy; }
        Player& GetPlayer() override { return *currentPlayer; }
        int GetTotalEnemyCount() const override { return static_cast<int>(sequence.size()); }

        void SaveState(int slot) override
        {
            assert(slot >= 0 && slot < SAVE_SLOT_COUNT);

            if (!currentPlayer || !currentEnemy || sequence.empty() || currentPlayer->GetHealthComponent().IsDead())
            {
                EventBus::Publish(SaveStateEvent(false, slot));
                return;
            }

            GameState state;
            state.currentIndex              = currentIndex;
            state.playerHp                  = currentPlayer->GetHealthComponent().GetHealth();
            state.playerMaxHp               = currentPlayer->GetHealthComponent().GetMaxHealth();
            state.playerMp                  = currentPlayer->GetMp();
            state.playerSprite              = static_cast<int>(currentPlayer->GetSprite());
            state.playerHealUses            = currentPlayer->GetHealUses();
            state.playerHasUsedClairvoyance = currentPlayer->GetHasUsedClairvoyance();

            state.enemyHp             = currentEnemy->GetHealthComponent().GetHealth();
            state.enemyMaxHp          = currentEnemy->GetHealthComponent().GetMaxHealth();
            state.enemySprite         = static_cast<int>(currentEnemy->GetSprite());
            state.enemyRockOffset     = currentEnemy->GetRockOffset();
            state.enemyScissorsOffset = currentEnemy->GetScissorsOffset();
            state.enemyPaperOffset    = currentEnemy->GetPaperOffset();

            state.sequence.reserve(sequence.size());
            for (const auto& cfg : sequence)
                state.sequence.push_back(static_cast<int>(assetService.ParseSprite(cfg.spriteName)));

            state.hand        = cardService.GetHandForSave();
            state.drawPile    = cardService.GetDrawPileForSave();
            state.discardPile = cardService.GetDiscardPileForSave();

            const bool success = persistenceService.SaveGame(slot, state);
            EventBus::Publish(SaveStateEvent(success, slot));
        }

        bool LoadState(int slot) override
        {
            assert(slot >= 0 && slot < SAVE_SLOT_COUNT);

            auto stateOpt = persistenceService.LoadGame(slot);
            if (!stateOpt.has_value())
            {
                EventBus::Publish(LoadStateEvent(false, slot));
                return false;
            }

            const GameState& state = *stateOpt;
            if (state.sequence.empty())
            {
                EventBus::Publish(LoadStateEvent(false, slot));
                return false;
            }

            // Rebuild sequence from sprite ints
            std::vector<EnemyConfig> newSequence;
            for (int spriteInt : state.sequence)
            {
                auto cfg = FindEnemyConfigBySprite(static_cast<ESprite>(spriteInt));
                if (!cfg.has_value())
                {
                    EventBus::Publish(LoadStateEvent(false, slot));
                    return false;
                }
                newSequence.push_back(*cfg);
            }
            sequence     = std::move(newSequence);
            currentIndex = state.currentIndex;

            // Restore player
            int maxHp = state.playerMaxHp;
            int hp    = std::max(0, std::min(state.playerHp, maxHp));
            if (!currentPlayer)
            {
                const auto& playerConfig = configService.GetPlayerConfig();
                currentPlayer = std::make_unique<Player>(playerConfig, static_cast<ESprite>(state.playerSprite));
            }
            currentPlayer->GetHealthComponent().SetMaxHealth(maxHp);
            currentPlayer->GetHealthComponent().SetHealth(hp);
            currentPlayer->SetMp(state.playerMp);
            currentPlayer->SetSprite(state.playerSprite);
            currentPlayer->SetHealUses(state.playerHealUses);
            currentPlayer->SetHasUsedClairvoyance(state.playerHasUsedClairvoyance);

            // Reinitialize enemy then override saved state
            LoadEnemy(sequence[currentIndex]);
            int enemyMaxHp = state.enemyMaxHp;
            int enemyHp    = std::max(0, std::min(state.enemyHp, enemyMaxHp));
            currentEnemy->GetHealthComponent().SetMaxHealth(enemyMaxHp);
            currentEnemy->GetHealthComponent().SetHealth(enemyHp);
            currentEnemy->SetRockOffset(state.enemyRockOffset);
            currentEnemy->SetScissorsOffset(state.enemyScissorsOffset);
            currentEnemy->SetPaperOffset(state.enemyPaperOffset);

            // Restore card piles
            cardService.SetHandFromLoad(state.hand);
            cardService.SetDrawPileFromLoad(state.drawPile);
            cardService.SetDiscardPileFromLoad(state.discardPile);

            EventBus::Publish(LoadStateEvent(true, slot));
            return true;
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
                currentPlayer->ResetBattleMagicUses();

                if (currentIndex < static_cast<int>(sequence.size()) - 1)
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

        /// @brief スプライト enum 値から EnemyConfig を検索する（LoadState 用ヘルパー）
        /// @param target 対象スプライト
        /// @return 一致する EnemyConfig。見つからない場合は std::nullopt
        std::optional<EnemyConfig> FindEnemyConfigBySprite(ESprite target) const
        {
            for (const auto& cfg : configService.GetEnemyConfigs())
            {
                if (assetService.ParseSprite(cfg.spriteName) == target)
                    return cfg;
            }
            return std::nullopt;
        }

        IConfigService& configService;
        IPersistenceService& persistenceService;  ///< 直接参照 — SaveGame/LoadGame に使用
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
        IPersistenceService& persistenceService,
        ICardService& cardService,
        IAssetService& assetService
    )
    {
        return std::make_unique<BattleService>(
            configService, persistenceService, cardService, assetService);
    }
} // namespace mc
