module;

#include <vector>
#include <memory>

export module BattleService;

import ConfigService;
import CardService;
import EventBus;
import Enemy;
import Player;

namespace mc {
    /// @brief 敵が撃破された際に発行されるイベント
    export struct EnemyDefeatedEvent : IEvent
    {
        int defeatedIndex;
        explicit EnemyDefeatedEvent(int index) : defeatedIndex(index) {}
    };

    /// @brief ステージ内の全敵を撃破した際に発行されるイベント
    export struct StageClearEvent : IEvent {};

    /// @brief プレイヤーが死亡してステージ失敗になった際に発行されるイベント
    export struct StageFailEvent : IEvent {};

    /// @brief ステージ開始時（敵 Sequence の準備完了後）に発行されるイベント
    export struct StageStartedEvent : IEvent {};

    /// @brief ステージの進行状態（Sequence、Current_Enemy_Index、ステージ結果）と
    /// キャラクター（プレイヤーおよび敵）の管理を行うサービス
    export class IBattleService
    {
    public:
        virtual ~IBattleService() = default;

        /// @brief 新しいステージを開始する。
        /// EnemyPool から 3 体を抽選し、プレイヤー・CardService を初期化する。
        virtual void StartStage() = 0;

        /// @brief 現在挑戦中の敵インデックスを返す（0–2）。
        virtual int GetCurrentEnemyIndex() const = 0;

        /// @brief 今ステージで選ばれた敵の Sequence を返す（長さ 3）。
        virtual const std::vector<EnemyConfig>& GetSequence() const = 0;

        /// @brief 指定した EnemyConfig に基づいて敵キャラクターを読み込む
        /// @param config 読み込む敵の設定
        virtual void LoadEnemy(const EnemyConfig& config) = 0;

        /// @brief 敵キャラクターのインスタンスを取得する
        /// @return 敵キャラクターの参照
        virtual Enemy& GetEnemy() = 0;

        /// @brief プレイヤーキャラクターのインスタンスを取得する
        /// @return プレイヤーキャラクターの参照
        virtual Player& GetPlayer() = 0;
    };

    export std::unique_ptr<IBattleService> CreateBattleService(
        IConfigService& configService,
        ICardService& cardService
    );
} // namespace mc
