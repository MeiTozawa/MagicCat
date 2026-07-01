module;

#include <ResourceConstantHedder.h>

export module AudioService;

import AssetService;
import BattleService;

namespace mc {
    /// @brief current を target に向けて最大 step だけ近づける。絶対に target を超えない。
    export float StepTowards(float current, float target, float step);

    export class IAudioService
    {
    public:
        virtual ~IAudioService() = default;
        /// @brief 毎フレーム呼び出す。BGM の音量フェードを更新する。
        virtual void Update(float deltaTime) = 0;
    };

    export std::unique_ptr<IAudioService> CreateAudioService(IAssetService& assetService,
                                                             IBattleService& characterService);
} // namespace mc
