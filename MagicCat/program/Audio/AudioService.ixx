module;

#include <ResourceConstantHedder.h>

export module AudioService;

import AssetService;
import BattleService;

namespace mc {
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
    };

    export std::unique_ptr<IAudioService> CreateAudioService(IAssetService& assetService,
                                                             IBattleService& characterService);
} // namespace mc
