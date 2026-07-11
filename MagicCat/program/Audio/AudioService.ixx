module;

export module AudioService;

import AssetService;
import BattleService;
import ConfigService;
import PersistenceService;

namespace mc {
    
    export constexpr int VOLUME_LEVEL_COUNT = 5;
    export constexpr int VOLUME_ROW_COUNT = 3;
    /// @brief current を target に向けて最大 step だけ近づける。絶対に target を超えない。
    export float StepTowards(float current, float target, float step);

    /// @brief ゲーム内BGMおよび効果音（SE）の再生・音量制御（フェードイン/フェードアウトなど）を担当するインターフェース
    export class IAudioService
    {
    public:
        virtual ~IAudioService() = default;

        /// @brief 毎フレームの更新処理。BGM の音量フェード状態を更新する。
        /// @param deltaTime 前のフレームからの経過時間（秒）
        virtual void Update(float deltaTime) = 0;

        /// @brief マスター音量レベル（0–4）を返す
        virtual int GetMasterVolume() const = 0;

        /// @brief マスター音量レベル（0–4）を設定する。範囲外はクランプ。
        /// @param level 設定する音量レベル
        virtual void SetMasterVolume(int level) = 0;

        /// @brief BGM 音量レベル（0–4）を返す
        virtual int GetBgmVolume() const = 0;

        /// @brief BGM 音量レベル（0–4）を設定する。範囲外はクランプ。
        /// @param level 設定する音量レベル
        virtual void SetBgmVolume(int level) = 0;

        /// @brief SFX 音量レベル（0–4）を返す
        virtual int GetSfxVolume() const = 0;

        /// @brief SFX 音量レベル（0–4）を設定する。範囲外はクランプ。
        /// @param level 設定する音量レベル
        virtual void SetSfxVolume(int level) = 0;
    };

    export std::unique_ptr<IAudioService> CreateAudioService(IPersistenceService& persistenceService,
                                                             IAssetService& assetService,
                                                             IBattleService& characterService);
} // namespace mc
