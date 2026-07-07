module;

#include <string>
#include <vector>
#include <memory>
#include "json11.hpp"

export module ConfigService;

namespace mc {
    /// @brief 敵キャラクターの初期ステータスを設定する構造体
    export struct EnemyConfig
    {
        int hp;
        int baseWeight;
        int rockDamage;
        int scissorsDamage;
        int paperDamage;
        std::wstring name;
        std::string spriteName;
    };

    /// @brief カードの初期設定（種類と数値）を保持する構造体
    export struct CardConfig
    {
        int type;
        int value;
    };

    /// @brief プレイヤーキャラクターの初期パラメータと魔法のバランス調整用数値を保持する構造体
    export struct PlayerConfig
    {
        int initialHp;
        int maxMp;
        int rockDamage;
        int scissorsDamage;
        int paperDamage;
        std::string spriteName;
        int clairvoyanceMpCost;
        int powerBoostMpCost;
        int powerBoostDamageOffset;
        int healMpCost;
        int healAmount;
        int maxHealUses;
    };

    /// @brief ゲーム進行に関わる全体的な設定を保持する構造体
    export struct GameConfig
    {
        int battleCount;
    };

    /// @brief 各種設定データ（カード、敵、プレイヤー、ゲーム設定）のロードおよび管理を担当するインターフェース
    export class IConfigService
    {
    public:
        virtual ~IConfigService() = default;

        /// @brief ロードされたカード設定のリストを取得する
        /// @return カード設定のリスト
        virtual const std::vector<CardConfig>& GetCardConfigs() const = 0;

        /// @brief ロードされた敵設定のリストを取得する
        /// @return 敵設定のリスト
        virtual const std::vector<EnemyConfig>& GetEnemyConfigs() const = 0;

        /// @brief プレイヤーの初期パラメータや魔法の設定を取得する
        /// @return プレイヤーの設定
        virtual const PlayerConfig& GetPlayerConfig() const = 0;

        /// @brief ゲーム進行に関する全体設定を取得する
        /// @return ゲーム設定
        virtual const GameConfig& GetGameConfig() const = 0;

        /// @brief サウンド設定を resource/sound_settings.json からロードする
        /// @return パース済みの json11::Json オブジェクト。ファイルが存在しないか不正な場合は json11::Json::NUL を返す（例外なし）
        virtual json11::Json LoadSoundSettings() = 0;

        /// @brief サウンド設定を resource/sound_settings.json に保存する
        /// @param data 保存する json11::Json オブジェクト
        /// @return 成功時 true、I/O エラー時 false（例外なし）
        virtual bool SaveSoundSettings(const json11::Json& data) = 0;
    };

    export std::unique_ptr<IConfigService> CreateConfigService(const std::string& cardConfigPath,
                                                               const std::string& enemyConfigPath,
                                                               const std::string& gameConfigPath);
} // namespace mc
