module;

module AudioService;
import EventBus;
import HealthComponent;
import Character;
import AssetService;
import CardService;
import SceneService;
import CharacterService;
import Player;
namespace mc
{
    class AudioService : public IAudioService
    {
        IAssetService* assetService;
        ICharacterService* characterService;
        std::vector<EventHandle> eventHandles;

    public:
        AudioService(IAssetService* asset, ICharacterService* character)
            : assetService(asset), characterService(character)
        {

            eventHandles.push_back(EventBus::Subscribe<HealthChangedEvent>([this](const HealthChangedEvent& e)
            {
                auto tags = e.Victim->GetTags();
                if (std::ranges::find(tags, ETag::Player) != tags.end())
                {
                    PlaySoundMem(assetService->GetSoundHandle(ESound::PlayerHurt), DX_PLAYTYPE_BACK);

                    if (characterService)
                    {
                        const auto& player = characterService->GetPlayer();
                        const auto& healthComp = player.GetHealthComponent();
                        if (e.CurrentHealth > 0 && e.CurrentHealth <= healthComp.GetMaxHealth() * 0.3f)
                        {
                            PlaySoundMem(assetService->GetSoundHandle(ESound::Warning), DX_PLAYTYPE_BACK);
                        }
                    }
                }
                else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                {
                    PlaySoundMem(assetService->GetSoundHandle(ESound::EnemyHurt),DX_PLAYTYPE_BACK);
                }
            }));

            eventHandles.push_back(EventBus::Subscribe<DrawCardEvent>([this](const DrawCardEvent& e)
            {
                PlaySoundMem(assetService->GetSoundHandle(ESound::DrawCard),DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<ShuffleEvent>([this](const ShuffleEvent& e)
            {
                PlaySoundMem(assetService->GetSoundHandle(ESound::Shuffle), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<ActionSelectionEvent>([this](const ActionSelectionEvent& e)
            {
                PlaySoundMem(assetService->GetSoundHandle(ESound::Select), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<CombatEvent>([this](const CombatEvent& e)
            {
                PlaySoundMem(assetService->GetSoundHandle(ESound::Confirm), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<MagicEvent>([this](const MagicEvent& e)
            {
                PlaySoundMem(assetService->GetSoundHandle(ESound::Magic), DX_PLAYTYPE_BACK);
            }));
            
            eventHandles.push_back(EventBus::Subscribe<LackOfMpEvent>([this](const LackOfMpEvent& e)
            {
                PlaySoundMem(assetService->GetSoundHandle(ESound::Beep), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<DeathEvent>([this](const DeathEvent& e)
            {
                auto tags = e.Victim->GetTags();
                if (std::ranges::find(tags, ETag::Player) != tags.end())
                {
                    PlaySoundMem(assetService->GetSoundHandle(ESound::Fail), DX_PLAYTYPE_BACK);
                }
                else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                {
                    PlaySoundMem(assetService->GetSoundHandle(ESound::Win), DX_PLAYTYPE_BACK);
                }
            }));
        }

        ~AudioService() override
        {
            for (auto handle : eventHandles)
            {
                EventBus::Unsubscribe(handle);
            }
        }
    };

    Shared<IAudioService> CreateAudioService(IAssetService* assetService, ICharacterService* characterService)
    {
        return std::make_shared<AudioService>(assetService, characterService);
    }
} // namespace mc
