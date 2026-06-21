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

    export Shared<IAudioService> CreateAudioService(IAssetService* assetService, ICharacterService* characterService);
} // namespace mc
