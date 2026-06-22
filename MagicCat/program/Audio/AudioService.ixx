module;

#include <ResourceConstantHedder.h>

export module AudioService;

import AssetService;
import CharacterService;

namespace mc
{
    export class IAudioService
    {
    public:
        virtual ~IAudioService() = default;
    };

    export std::unique_ptr<IAudioService> CreateAudioService(IAssetService& assetService, ICharacterService& characterService);
} // namespace mc
