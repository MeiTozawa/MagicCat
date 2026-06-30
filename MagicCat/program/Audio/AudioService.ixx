module;

#include <ResourceConstantHedder.h>

export module AudioService;

import AssetService;
import BattleService;

namespace mc {
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
