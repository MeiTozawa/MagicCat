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
import ConfigService;
import Player;
import Enemy;
namespace mc {
    float StepTowards(float current, float target, float step)
    {
        if (current < target) return std::min(current + step, target);
        if (current > target) return std::max(current - step, target);
        return target;
    }

    static ESound GetAttackSound(ESprite sprite)
    {
        switch (sprite)
        {
        case ESprite::MeowingCat:
            return ESound::CatAttack;
        case ESprite::Wolf:
        case ESprite::TimberWolf:
        case ESprite::SnowFox:
        case ESprite::Bunny:
        case ESprite::StinkySkunk:
            return ESound::WolfAttack;
        case ESprite::DaintyPig:
        case ESprite::MadBoar:
        case ESprite::SlowTurtle:
        case ESprite::SpikeyPorcupine:
        case ESprite::CoralCrab:
            return ESound::PigAttack;
        case ESprite::PasturingSheep:
        case ESprite::CluckingChicken:
        case ESprite::TinyChick:
        case ESprite::HonkingGoose:
        case ESprite::CroakingToad:
        case ESprite::LeapingFrog:
            return ESound::SheepAttack;
        default:
            return ESound::CatAttack;
        }
    }

    /// @brief 音量レベル（0–4）を DxLib の音量値（0–255）にマッピングする
    /// レベル 0=0, 1=51, 2=102, 3=153, 4=204
    static constexpr int VOLUME_LEVEL_MAP[5] = { 0, 51, 102, 153, 204 };

    static constexpr int BGM_VOLUME_MAX = 255 * 0.8;
    static constexpr float BGM_FADE_TIME = 1.5f; ///< フェードイン/アウトの秒数

    class AudioService : public IAudioService
    {
    public:
        AudioService(IConfigService& config, IAssetService& asset, IBattleService& character)
            : configService(config), assetService(asset), characterService(character)
        {
            // サウンド設定を IConfigService 経由でロードする（直接ファイルは開かない）
            if (!configService.LoadSoundSettings(masterLevel, bgmLevel, sfxLevel))
            {
                // ファイルが存在しない場合、デフォルト値 4 を使用し保存する
                masterLevel = 4;
                bgmLevel    = 4;
                sfxLevel    = 4;
                configService.SaveSoundSettings(masterLevel, bgmLevel, sfxLevel);
            }
            // ロード後クランプ（ファイルが外部で編集された場合の保護）
            masterLevel = std::clamp(masterLevel, 0, 4);
            bgmLevel    = std::clamp(bgmLevel,    0, 4);
            sfxLevel    = std::clamp(sfxLevel,    0, 4);

            bgmHandle = assetService.GetSoundHandle(ESound::BGM);
            SubscribeSfxEvents();
            SubscribeBgmEvents();
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

        // --- 音量 API ---

        int GetMasterVolume() const override { return masterLevel; }

        void SetMasterVolume(int level) override
        {
            masterLevel = std::clamp(level, 0, 4);
            PersistVolumeSettings();
            ApplyVolumeToDxLib();
        }

        int GetBgmVolume() const override { return bgmLevel; }

        void SetBgmVolume(int level) override
        {
            bgmLevel = std::clamp(level, 0, 4);
            PersistVolumeSettings();
            ApplyVolumeToDxLib();
        }

        int GetSfxVolume() const override { return sfxLevel; }

        void SetSfxVolume(int level) override
        {
            sfxLevel = std::clamp(level, 0, 4);
            PersistVolumeSettings();
            ApplyVolumeToDxLib();
        }

    private:
        /// @brief 現在の音量レベルを sound_settings.json に保存する（IConfigService 経由）
        void PersistVolumeSettings()
        {
            configService.SaveSoundSettings(masterLevel, bgmLevel, sfxLevel);
        }

        /// @brief 現在の音量レベルを DxLib チャンネルに適用する
        void ApplyVolumeToDxLib()
        {
            // BGM チャンネル: bgmLevel で BGM_VOLUME_MAX をスケール
            if (bgmHandle != -1)
            {
                int scaledBgm = BGM_VOLUME_MAX * bgmLevel / 4;
                bgmTarget = static_cast<float>(scaledBgm);
                bgmVolume = static_cast<float>(scaledBgm);
                ChangeVolumeSoundMem(scaledBgm, bgmHandle);
            }
        }

        void SubscribeSfxEvents()
        {
            eventHandles.push_back(EventBus::Subscribe<HealthChangedEvent>([this](const HealthChangedEvent& e)
            {
                OnHealthChanged(e);
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
                OnDeath(e);
            }));
        }

        void SubscribeBgmEvents()
        {
            eventHandles.push_back(EventBus::Subscribe<CutsceneFinishedEvent>([this](const CutsceneFinishedEvent&)
            {
                StartBgmFadeIn();
            }));
            eventHandles.push_back(EventBus::Subscribe<EnemyDefeatedEvent>([this](const EnemyDefeatedEvent&)
            {
                StartBgmFadeOut();
                PlaySoundMem(assetService.GetSoundHandle(ESound::WinTheBattle), DX_PLAYTYPE_BACK);
            }));
            eventHandles.push_back(EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent&)
            {
                StartBgmFadeOut();
                PlaySoundMem(assetService.GetSoundHandle(ESound::WinTheGame), DX_PLAYTYPE_BACK);
            }));
            eventHandles.push_back(EventBus::Subscribe<StageFailEvent>([this](const StageFailEvent&)
            {
                StartBgmFadeOut();
            }));
        }

        void OnHealthChanged(const HealthChangedEvent& e) const
        {
            auto tags = e.Victim->GetTags();
            if (std::ranges::find(tags, ETag::Player) != tags.end())
            {
                ESound soundId = GetAttackSound(characterService.GetEnemy().GetSprite());
                PlaySoundMem(assetService.GetSoundHandle(soundId), DX_PLAYTYPE_BACK);
                const auto& player = characterService.GetPlayer();
                const auto& healthComp = player.GetHealthComponent();
                if (e.CurrentHealth > 0 && e.CurrentHealth <= healthComp.GetMaxHealth() * 0.3f)
                    PlaySoundMem(assetService.GetSoundHandle(ESound::Warning), DX_PLAYTYPE_BACK);
            }
            else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
            {
                ESound soundId = GetAttackSound(characterService.GetPlayer().GetSprite());
                PlaySoundMem(assetService.GetSoundHandle(soundId), DX_PLAYTYPE_BACK);
            }
        }

        void OnDeath(const DeathEvent& e) const
        {
            auto tags = e.Victim->GetTags();
            if (std::ranges::find(tags, ETag::Player) != tags.end())
                PlaySoundMem(assetService.GetSoundHandle(ESound::Fail), DX_PLAYTYPE_BACK);
        }

        void SetBgmTarget(float target)
        {
            bgmTarget = target;
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

        IConfigService& configService;
        IAssetService& assetService;
        IBattleService& characterService;
        std::vector<EventHandle> eventHandles;

        int masterLevel = 4; ///< マスター音量レベル（0–4）
        int bgmLevel    = 4; ///< BGM 音量レベル（0–4）
        int sfxLevel    = 4; ///< SFX 音量レベル（0–4）

        int bgmHandle = -1;
        float bgmVolume = 0.f; ///< 現在の実音量（0–255）
        float bgmTarget = 0.f; ///< 目標音量
        float bgmFadeSpeed = 0.f; ///< 1秒あたりの変化量
    };

    std::unique_ptr<IAudioService> CreateAudioService(IConfigService& configService,
                                                      IAssetService& assetService,
                                                      IBattleService& characterService)
    {
        return std::make_unique<AudioService>(configService, assetService, characterService);
    }
} // namespace mc
