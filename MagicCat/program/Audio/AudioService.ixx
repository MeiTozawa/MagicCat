module;

export module AudioService;

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