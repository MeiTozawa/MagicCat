module;

export module AudioService;

namespace mc {

enum class ESoundEffect
{
    PlayACard, EnterTheGame, 
};

export class IAudioService
{
public:
    virtual ~IAudioService() = default;
    
    virtual void PlaySoundEffect(ESoundEffect soundEffect);
};
} // namespace mc

