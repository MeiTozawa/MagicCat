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

    class AudioService : public IAudioService
    {
    public:
        AudioService(IConfigService& config, IAssetService& asset, IBattleService& character)
            : configService(config), assetService(asset), characterService(character)
        {
            if (!configService.LoadSoundSettings(masterLevel, bgmLevel, sfxLevel))
            {
                masterLevel = 4;
                bgmLevel = 4;
                sfxLevel = 4;
                configService.SaveSoundSettings(4, 4, 4);
            }
            masterLevel = std::clamp(masterLevel, 0, 4);
            bgmLevel = std::clamp(bgmLevel, 0, 4);
            sfxLevel = std::clamp(sfxLevel, 0, 4);

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
        void PersistVolumeSettings() const
        {
            configService.SaveSoundSettings(masterLevel, bgmLevel, sfxLevel);
        }

        /// @brief BGM の DxLib 音量値を計算する（bgmLevel × masterLevel のスケール）
        int CalcBgmDxVolume() const
        {
            return VOLUME_LEVEL_MAP[bgmLevel] * (masterLevel + 1) / 5.f;
        }

        /// @brief SFX の DxLib 音量値を計算する（sfxLevel × masterLevel のスケール）
        int CalcSfxDxVolume() const
        {
            return VOLUME_LEVEL_MAP[sfxLevel] * (masterLevel + 1) / 5.f;
        }

        /// @brief SFX ハンドルを現在の音量で再生する
        void PlaySfx(int handle) const
        {
            ChangeVolumeSoundMem(CalcSfxDxVolume(), handle);
            PlaySoundMem(handle, DX_PLAYTYPE_BACK);
        }

        void ApplyVolumeToDxLib()
        {
            // BGM チャンネルに適用
            if (bgmHandle != -1)
            {
                const int vol = CalcBgmDxVolume();
                bgmTarget = static_cast<float>(vol);
                bgmVolume = static_cast<float>(vol);
                ChangeVolumeSoundMem(vol, bgmHandle);
            }

            // SFX チャンネル（全サウンド）に適用
            const int sfxVol = CalcSfxDxVolume();
            constexpr ESound sfxSounds[] = {
                ESound::Confirm, ESound::DrawCard, ESound::Select, ESound::Shuffle,
                ESound::Warning, ESound::CatAttack, ESound::SheepAttack, ESound::WolfAttack,
                ESound::PigAttack, ESound::Fail, ESound::WinTheBattle, ESound::WinTheGame,
                ESound::Beep, ESound::Magic
            };
            for (const ESound sound : sfxSounds)
            {
                const int handle = assetService.GetSoundHandle(sound);
                if (handle != -1)
                    ChangeVolumeSoundMem(sfxVol, handle);
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
                PlaySfx(assetService.GetSoundHandle(ESound::DrawCard));
            }));
            eventHandles.push_back(EventBus::Subscribe<ShuffleEvent>([this](const ShuffleEvent&)
            {
                PlaySfx(assetService.GetSoundHandle(ESound::Shuffle));
            }));
            eventHandles.push_back(EventBus::Subscribe<ActionSelectionEvent>([this](const ActionSelectionEvent& e)
            {
                if (!e.silent)
                    PlaySfx(assetService.GetSoundHandle(ESound::Select));
            }));
            eventHandles.push_back(EventBus::Subscribe<CombatEvent>([this](const CombatEvent&)
            {
                PlaySfx(assetService.GetSoundHandle(ESound::Confirm));
            }));
            eventHandles.push_back(EventBus::Subscribe<MagicEvent>([this](const MagicEvent&)
            {
                PlaySfx(assetService.GetSoundHandle(ESound::Magic));
            }));
            eventHandles.push_back(EventBus::Subscribe<LackOfMpEvent>([this](const LackOfMpEvent&)
            {
                PlaySfx(assetService.GetSoundHandle(ESound::Beep));
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
                PlaySfx(assetService.GetSoundHandle(ESound::WinTheBattle));
            }));
            eventHandles.push_back(EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent&)
            {
                StartBgmFadeOut();
                PlaySfx(assetService.GetSoundHandle(ESound::WinTheGame));
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
                PlaySfx(assetService.GetSoundHandle(soundId));
                const auto& player = characterService.GetPlayer();
                const auto& healthComp = player.GetHealthComponent();
                if (e.CurrentHealth > 0 && e.CurrentHealth <= healthComp.GetMaxHealth() * 0.3f)
                    PlaySfx(assetService.GetSoundHandle(ESound::Warning));
            }
            else if (std::ranges::find(tags, ETag::Enemy) != tags.end())
            {
                ESound soundId = GetAttackSound(characterService.GetPlayer().GetSprite());
                PlaySfx(assetService.GetSoundHandle(soundId));
            }
        }

        void OnDeath(const DeathEvent& e) const
        {
            auto tags = e.Victim->GetTags();
            if (std::ranges::find(tags, ETag::Player) != tags.end())
                PlaySfx(assetService.GetSoundHandle(ESound::Fail));
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
            SetBgmTarget(static_cast<float>(CalcBgmDxVolume()));
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
        int bgmLevel = 4; ///< BGM 音量レベル（0–4）
        int sfxLevel = 4; ///< SFX 音量レベル（0–4）

        int bgmHandle = -1;
        float bgmVolume = 0.f; ///< 現在の実音量（0–255）
        float bgmTarget = 0.f; ///< 目標音量
        float bgmFadeSpeed = 0.f; ///< 1秒あたりの変化量

    private:
        static constexpr int BGM_VOLUME_MAX = 255;
        static constexpr float BGM_FADE_TIME = 1.5f;

        /// @brief 音量レベル（0–4）を DxLib の音量値（0–255）にマッピングする
        /// 仕様: level v → {0, 51, 102, 153, 204, 255}[v]
        /// 配列サイズ 6 は仕様の完全な表をそのまま持つため（有効インデックスは 0–4）
        static constexpr int VOLUME_LEVEL_MAP[6] = {
            0, 51, 102, 153, 204, 255
        };
    };

    std::unique_ptr<IAudioService> CreateAudioService(IConfigService& configService,
                                                      IAssetService& assetService,
                                                      IBattleService& characterService)
    {
        return std::make_unique<AudioService>(configService, assetService, characterService);
    }
} // namespace mc
