module;

#include <algorithm>

module AudioService;
import EventBus;
import HealthComponent;
import Character;
import AssetService;
import CardService;
import SceneService;
import BattleService;
import Player;
namespace mc {
    float StepTowards(float current, float target, float step)
    {
        if (current < target) return std::min(current + step, target);
        if (current > target) return std::max(current - step, target);
        return target;
    }

    static constexpr int   BGM_VOLUME_MAX   = 255;
    static constexpr float BGM_FADE_TIME    = 1.5f; ///< フェードイン/アウトの秒数

    class AudioService : public IAudioService
    {
        IAssetService& assetService;
        IBattleService& characterService;
        std::vector<EventHandle> eventHandles;

        int   bgmHandle     = -1;
        float bgmVolume     = 0.f;   ///< 現在の実音量（0–255）
        float bgmTarget     = 0.f;   ///< 目標音量
        float bgmFadeSpeed  = 0.f;   ///< 1秒あたりの変化量

        void SetBgmTarget(float target)
        {
            bgmTarget    = target;
            bgmFadeSpeed = BGM_VOLUME_MAX / BGM_FADE_TIME;
        }

        void StartBgmFadeIn()
        {
            if (bgmHandle == -1) return;
            if (CheckSoundMem(bgmHandle))
                StopSoundMem(bgmHandle);
            PlaySoundMem(bgmHandle, DX_PLAYTYPE_LOOP);
            bgmVolume = 0.f;
            ChangeVolumeSoundMem(0, bgmHandle);
            SetBgmTarget(static_cast<float>(BGM_VOLUME_MAX));
        }

        void StartBgmFadeOut()
        {
            if (bgmHandle == -1) return;
            SetBgmTarget(0.f);
        }

    public:
        AudioService(IAssetService& asset, IBattleService& character)
            : assetService(asset), characterService(character)
        {
            bgmHandle = assetService.GetSoundHandle(ESound::BGM);

            // --- SFX イベント ---
            eventHandles.push_back(EventBus::Subscribe<HealthChangedEvent>([this](const HealthChangedEvent& e)
            {
                auto tags = e.Victim->GetTags();
                if (std::ranges::find(tags, ETag::Player) != tags.end())
                {
                    PlaySoundMem(assetService.GetSoundHandle(ESound::PlayerHurt), DX_PLAYTYPE_BACK);

                    const auto& player = characterService.GetPlayer();
                    const auto& healthComp = player.GetHealthComponent();
                    if (e.CurrentHealth > 0 && e.CurrentHealth <= healthComp.GetMaxHealth() * 0.3f)
                    {
                        PlaySoundMem(assetService.GetSoundHandle(ESound::Warning), DX_PLAYTYPE_BACK);
                    }
                }
                else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                {
                    PlaySoundMem(assetService.GetSoundHandle(ESound::EnemyHurt), DX_PLAYTYPE_BACK);
                }
            }));

            eventHandles.push_back(EventBus::Subscribe<DrawCardEvent>([this](const DrawCardEvent&)
            {
                PlaySoundMem(assetService.GetSoundHandle(ESound::DrawCard), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<ShuffleEvent>([this](const ShuffleEvent&)
            {
                PlaySoundMem(assetService.GetSoundHandle(ESound::Shuffle), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<ActionSelectionEvent>([this](const ActionSelectionEvent& e)
            {
                if (!e.silent)
                    PlaySoundMem(assetService.GetSoundHandle(ESound::Select), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<CombatEvent>([this](const CombatEvent&)
            {
                PlaySoundMem(assetService.GetSoundHandle(ESound::Confirm), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<MagicEvent>([this](const MagicEvent&)
            {
                PlaySoundMem(assetService.GetSoundHandle(ESound::Magic), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<LackOfMpEvent>([this](const LackOfMpEvent&)
            {
                PlaySoundMem(assetService.GetSoundHandle(ESound::Beep), DX_PLAYTYPE_BACK);
            }));

            eventHandles.push_back(EventBus::Subscribe<DeathEvent>([this](const DeathEvent& e)
            {
                auto tags = e.Victim->GetTags();
                if (std::ranges::find(tags, ETag::Player) != tags.end())
                {
                    PlaySoundMem(assetService.GetSoundHandle(ESound::Fail), DX_PLAYTYPE_BACK);
                }
                else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
                {
                    PlaySoundMem(assetService.GetSoundHandle(ESound::Win), DX_PLAYTYPE_BACK);
                }
            }));

            // --- BGM フェードイン：戦闘開始 ---
            // CutsceneFinishedEvent でカットシーン → Combat へ遷移するタイミングで開始する
            eventHandles.push_back(EventBus::Subscribe<CutsceneFinishedEvent>([this](const CutsceneFinishedEvent&)
            {
                StartBgmFadeIn();
            }));

            // --- BGM フェードアウト：敵撃破（次の敵へ移行）／ゲーム終了 ---
            eventHandles.push_back(EventBus::Subscribe<EnemyDefeatedEvent>([this](const EnemyDefeatedEvent&)
            {
                StartBgmFadeOut();
            }));
            eventHandles.push_back(EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent&)
            {
                StartBgmFadeOut();
            }));
            eventHandles.push_back(EventBus::Subscribe<StageFailEvent>([this](const StageFailEvent&)
            {
                StartBgmFadeOut();
            }));
        }

        ~AudioService() override
        {
            if (bgmHandle != -1 && CheckSoundMem(bgmHandle))
                StopSoundMem(bgmHandle);

            for (auto handle : eventHandles)
                EventBus::Unsubscribe(handle);
        }

        void Update(float deltaTime) override
        {
            if (bgmHandle == -1) return;
            if (bgmVolume == bgmTarget) return;

            bgmVolume = StepTowards(bgmVolume, bgmTarget, bgmFadeSpeed * deltaTime);
            if (bgmVolume <= 0.f && bgmTarget == 0.f && CheckSoundMem(bgmHandle))
                StopSoundMem(bgmHandle);

            ChangeVolumeSoundMem(static_cast<int>(bgmVolume), bgmHandle);
        }
    };

    std::unique_ptr<IAudioService> CreateAudioService(IAssetService& assetService, IBattleService& characterService)
    {
        return std::make_unique<AudioService>(assetService, characterService);
    }
} // namespace mc
