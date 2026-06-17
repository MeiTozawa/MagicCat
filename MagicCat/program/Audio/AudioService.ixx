module;

#include <ResourceConstantHedder.h>

export module AudioService;

namespace mc
{
    export class IAudioService
    {
    public:
        virtual ~IAudioService() = default;
    };

    export Shared<IAudioService> CreateAudioService();
} // namespace mc
