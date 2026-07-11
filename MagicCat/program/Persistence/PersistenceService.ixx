module;

#include <vector>
#include <memory>
#include <optional>

export module PersistenceService;

namespace mc {
    /// @brief セーブスロットの総数
    export constexpr int SAVE_SLOT_COUNT = 4;

    /// @brief カードのシリアライズ用データ
    export struct CardData
    {
        int type = 0;
        int power = 0;
    };

    /// @brief セーブ・ロード時にバトル状態全体を転送するデータ構造体
    export struct GameState
    {
        int playerHp = 0;
        int playerMaxHp = 0;
        int playerMp = 0;
        int playerSprite = -1;
        int playerHealUses = 0;
        bool playerHasUsedClairvoyance = false;

        int enemyHp = 0;
        int enemyMaxHp = 0;
        int enemySprite = -1;
        int enemyRockOffset = 0;
        int enemyScissorsOffset = 0;
        int enemyPaperOffset = 0;

        int currentIndex = 0;
        std::vector<int> sequence;

        std::vector<CardData> hand;
        std::vector<CardData> drawPile;
        std::vector<CardData> discardPile;
    };

    /// @brief セーブスロットの概要情報をメニュー表示用に保持する構造体
    export struct SaveMetadata
    {
        bool exists = false;
        int playerHp = 0;
        int playerMaxHp = 0;
        int playerSprite = -1;
        int enemyHp = 0;
        int enemyMaxHp = 0;
        int enemySprite = -1;
        int currentBattle = 0;
        int totalBattles = 0;
    };

    /// @brief 音量設定とゲームセーブデータの永続化を担当するインターフェース
    export class IPersistenceService
    {
    public:
        virtual ~IPersistenceService() = default;

        virtual bool LoadSoundSettings(int& masterVolume, int& bgmVolume, int& sfxVolume) = 0;
        virtual bool SaveSoundSettings(int masterVolume, int bgmVolume, int sfxVolume) = 0;
        virtual bool SaveGame(int slot, const GameState& state) = 0;
        virtual std::optional<GameState> LoadGame(int slot) = 0;
        virtual SaveMetadata GetSaveMetadata(int slot) = 0;
    };

    export std::unique_ptr<IPersistenceService> CreatePersistenceService();
} // namespace mc
