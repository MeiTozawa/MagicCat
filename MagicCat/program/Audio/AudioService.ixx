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
    };

    export std::unique_ptr<IAudioService> CreateAudioService(IAssetService& assetService,
                                                             IBattleService& characterService);
} // namespace mc
