module;

#include <string>
#include <vector>
#include <memory>
#include <optional>

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

    /// @brief セーブスロットの総数（スロット番号は [0, SAVE_SLOT_COUNT) の範囲）
    export constexpr int SAVE_SLOT_COUNT = 4;

    /// @brief カードのシリアライズ用データ（種類とパワー値を int で保持）
    export struct CardData
    {
        int type  = 0;
        int power = 0;
    };

    /// @brief セーブ・ロード時にバトル状態全体を転送するデータ構造体
    export struct GameState
    {
        // Player
        int playerHp                   = 0;
        int playerMaxHp                = 0;
        int playerMp                   = 0;
        int playerSprite               = -1;
        int playerHealUses             = 0;
        bool playerHasUsedClairvoyance = false;

        // Enemy
        int enemyHp             = 0;
        int enemyMaxHp          = 0;
        int enemySprite         = -1;
        int enemyRockOffset     = 0;
        int enemyScissorsOffset = 0;
        int enemyPaperOffset    = 0;

        // Battle progress
        int currentIndex = 0;
        std::vector<int> sequence;  // ESprite int values

        // Cards
        std::vector<CardData> hand;
        std::vector<CardData> drawPile;
        std::vector<CardData> discardPile;
    };

    /// @brief セーブスロットの概要情報をメニュー表示用に保持する構造体（フル状態のロードなし）
    export struct SaveMetadata
    {
        bool exists        = false;
        int  playerHp      = 0;
        int  playerMaxHp   = 0;
        int  playerSprite  = -1;  // ESprite の int キャスト値
        int  enemyHp       = 0;
        int  enemyMaxHp    = 0;
        int  enemySprite   = -1;  // ESprite の int キャスト値
        int  currentBattle = 0;   // 1-based display index (currentIndex + 1)
        int  totalBattles  = 0;
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
        /// @param masterVolume ロードされたマスター音量レベルの出力先
        /// @param bgmVolume ロードされた BGM 音量レベルの出力先
        /// @param sfxVolume ロードされた SFX 音量レベルの出力先
        /// @return 成功時 true、ファイルが存在しないか不正な場合は false（例外なし）
        virtual bool LoadSoundSettings(int& masterVolume, int& bgmVolume, int& sfxVolume) = 0;

        /// @brief サウンド設定を resource/sound_settings.json に保存する
        /// @param masterVolume 保存するマスター音量レベル
        /// @param bgmVolume 保存する BGM 音量レベル
        /// @param sfxVolume 保存する SFX 音量レベル
        /// @return 成功時 true、I/O エラー時 false（例外なし）
        virtual bool SaveSoundSettings(int masterVolume, int bgmVolume, int sfxVolume) = 0;

        /// @brief ゲーム状態を resource/saveX.json に JSON 形式で書き込む
        /// @param slot スロット番号 [0, SAVE_SLOT_COUNT - 1]（0 はオートセーブ）
        /// @param state 書き込む GameState データ
        /// @return 成功時 true、I/O エラー時 false（例外なし）
        virtual bool SaveGame(int slot, const GameState& state) = 0;

        /// @brief resource/saveX.json を読み込み GameState を返す
        /// @param slot スロット番号 [0, SAVE_SLOT_COUNT - 1]
        /// @return 解析済み GameState。ファイルが存在しないか不正な場合は std::nullopt（例外なし）
        virtual std::optional<GameState> LoadGame(int slot) = 0;

        /// @brief セーブスロットの概要情報をフル状態のロードなしで取得する
        /// @param slot スロット番号 [0, SAVE_SLOT_COUNT - 1]
        /// @return SaveMetadata。ファイルが存在しないか不正な場合は exists=false
        virtual SaveMetadata GetSaveMetadata(int slot) = 0;
    };

    export std::unique_ptr<IConfigService> CreateConfigService(const std::string& cardConfigPath,
                                                               const std::string& enemyConfigPath,
                                                               const std::string& gameConfigPath);
} // namespace mc
