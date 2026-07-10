module;

#include <vector>
#include <memory>

export module BattleService;

import ConfigService;
import CardService;
import AssetService;
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

    /// @brief セーブステートが実行された際に発行されるイベント
    export struct SaveStateEvent : IEvent
    {
        bool success;
        int slot;
        explicit SaveStateEvent(bool success, int slot) : success(success), slot(slot) {}
    };

    /// @brief ステージの進行状態（Sequence、Current_Enemy_Index、ステージ結果）と
    /// キャラクター（プレイヤーおよび敵）の管理を行うサービス
    export class IBattleService
    {
    public:
        virtual ~IBattleService() = default;

        /// @brief 新しいステージを開始する。
        /// EnemyPool から敵を抽選し、プレイヤー・CardService を初期化する。
        virtual void StartStage() = 0;

        /// @brief 現在挑戦中の敵のインデックスを取得する。
        /// @return 敵のインデックス
        virtual int GetCurrentEnemyIndex() const = 0;

        /// @brief 今ステージで選ばれた敵の Sequence を取得する。
        /// @return 敵の構成情報のリスト
        virtual const std::vector<EnemyConfig>& GetSequence() const = 0;

        /// @brief 指定した EnemyConfig に基づいて敵キャラクターを読み込む、または再初期化する
        /// @param config 読み込む敵の設定
        virtual void LoadEnemy(const EnemyConfig& config) = 0;

        /// @brief 現在の敵キャラクターのインスタンスへの参照を取得する
        /// @return 敵キャラクターの参照
        virtual Enemy& GetEnemy() = 0;

        /// @brief プレイヤーキャラクターのインスタンスへの参照を取得する
        /// @return プレイヤーキャラクターの参照
        virtual Player& GetPlayer() = 0;

        /// @brief 現在のステージにおける敵の総数を取得する。
        /// @return 敵の総数
        virtual int GetTotalEnemyCount() const = 0;

        /// @brief 現在のバトル状態を指定スロットに保存する
        /// @param slot セーブスロット番号 [0, SAVE_SLOT_COUNT)
        virtual void SaveState(int slot) = 0;

        /// @brief 指定スロットからバトル状態を復元する
        /// @param slot セーブスロット番号 [0, SAVE_SLOT_COUNT)
        /// @return 成功時 true、ファイルが存在しないか不正な場合は false
        virtual bool LoadState(int slot) = 0;
    };

    export std::unique_ptr<IBattleService> CreateBattleService(
        IConfigService& configService,
        ICardService& cardService,
        IAssetService& assetService
    );
} // namespace mc
