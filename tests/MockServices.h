#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>

import AssetService;
import AudioService;
import InputService;
import RenderService;
import ConfigService;
import PersistenceService;
import BattleService;
import CardService;
import OSService;
import Enemy;
import Player;

namespace mc {

/// @brief IConfigService のみをモックする（静的設定読み込み 4 メソッド）
class MockIConfigService : public IConfigService {
public:
    MOCK_METHOD(const std::vector<CardConfig>&, GetCardConfigs, (), (const, override));
    MOCK_METHOD(const std::vector<EnemyConfig>&, GetEnemyConfigs, (), (const, override));
    MOCK_METHOD(const PlayerConfig&, GetPlayerConfig, (), (const, override));
    MOCK_METHOD(const GameConfig&, GetGameConfig, (), (const, override));
    
    // Storage for configs to return by reference
    std::vector<CardConfig> cardConfigs;
    std::vector<EnemyConfig> enemyConfigs;
    PlayerConfig playerConfig;
    GameConfig gameConfig;
};

/// @brief IPersistenceService のみをモックする（音量設定・存档 R/W 5 メソッド）
class MockIPersistenceService : public IPersistenceService {
public:
    MOCK_METHOD(bool, LoadSoundSettings, (int& masterVolume, int& bgmVolume, int& sfxVolume), (override));
    MOCK_METHOD(bool, SaveSoundSettings, (int masterVolume, int bgmVolume, int sfxVolume), (override));
    MOCK_METHOD(bool, SaveGame, (int slot, const GameState& state), (override));
    MOCK_METHOD(std::optional<GameState>, LoadGame, (int slot), (override));
    MOCK_METHOD(SaveMetadata, GetSaveMetadata, (int slot), (override));
};

/// @brief 両インターフェースを組み合わせた後方互換モック（既存テスト向け）
class MockConfigService : public MockIConfigService, public MockIPersistenceService {};

class MockAssetService : public IAssetService {
public:
    MOCK_METHOD(int, GetImageHandle, (EImage), (override));
    MOCK_METHOD(int, GetFontHandle, (EFont), (override));
    MOCK_METHOD(int, GetSpriteHandle, (ESprite), (override));
    MOCK_METHOD(SpriteInfo, GetSpriteInfo, (ESprite), (override));
    MOCK_METHOD(int, GetSoundHandle, (ESound), (override));
    MOCK_METHOD(ESprite, ParseSprite, (const std::string&), (const, override));
};

class MockAudioService : public IAudioService {
public:
    MOCK_METHOD(void, Update, (float), (override));
    MOCK_METHOD(int, GetMasterVolume, (), (const, override));
    MOCK_METHOD(void, SetMasterVolume, (int), (override));
    MOCK_METHOD(int, GetBgmVolume, (), (const, override));
    MOCK_METHOD(void, SetBgmVolume, (int), (override));
    MOCK_METHOD(int, GetSfxVolume, (), (const, override));
    MOCK_METHOD(void, SetSfxVolume, (int), (override));
};

class MockInputService : public IInputService {
public:
    MOCK_METHOD(void, Update, (), (override));
    MOCK_METHOD(bool, IsPressed, (InputAction), (const, override));
    MOCK_METHOD(bool, IsHolding, (InputAction), (const, override));
    MOCK_METHOD(bool, IsReleased, (InputAction), (const, override));
    MOCK_METHOD(Point<int>, OnMouseClick, (InputAction), (const, override));
    MOCK_METHOD(InputDevice, GetActiveDevice, (), (const, override));
    MOCK_METHOD(void, PushContext, (InputContext), (override));
    MOCK_METHOD(void, PopContext, (), (override));
    MOCK_METHOD(void, ClearAndSetContext, (InputContext), (override));
    MOCK_METHOD(Point<int>, GetMousePosition, (), (const, override));
};

class MockRenderService : public IRenderService {
public:
    MOCK_METHOD(int, GetFontSize, (), (override));
    MOCK_METHOD(void, DrawString, (int, int, const wchar_t*, uint32_t), (override));
    MOCK_METHOD(void, DrawBoxAA, (float, float, float, float, uint32_t, bool), (override));
    MOCK_METHOD(int, GetDrawStringWidth, (const wchar_t*), (override));
    MOCK_METHOD(void, DrawGraph, (int, int, int, bool), (override));
    MOCK_METHOD(void, DrawRotaGraphF, (float, float, double, double, int, bool), (override));
    MOCK_METHOD(void, DrawRectRotaGraph, (int, int, int, int, int, int, double, double, int, bool, bool), (override));
    MOCK_METHOD(void, SetDrawBlendMode, (BlendMode, int), (override));
    MOCK_METHOD(void, SetDrawBright, (int, int, int), (override));
    MOCK_METHOD(void, DrawRoundRectFrame, (int, int, int, int, int, int, uint32_t), (override));
    MOCK_METHOD(void, DrawCenterString, (int, int, const wchar_t*, uint32_t), (override));
    MOCK_METHOD(void, DrawLeftString, (int, int, const wchar_t*, uint32_t), (override));
    MOCK_METHOD(void, DrawRightString, (int, int, const wchar_t*, uint32_t), (override));
    MOCK_METHOD(void, DrawHollowBox, (int, int, int, int, int, uint32_t), (override));
    MOCK_METHOD(void, SetFontTypeNormal, (), (override));
    MOCK_METHOD(void, ChangeFont, (const wchar_t*), (override));
    MOCK_METHOD(void, SetFontSize, (int), (override));
    MOCK_METHOD(void, SetFontThickness, (int), (override));
    MOCK_METHOD(void, SetBackgroundColor, (int, int, int), (override));
    MOCK_METHOD(void, DrawFilledTriangle, (int, int, int, int, int, int, uint32_t), (override));
    MOCK_METHOD(void, DrawButton, (int, int, int, int, const wchar_t*, bool, uint32_t, uint32_t), (override));
    MOCK_METHOD(void, DrawOption, (Rect<int>, const wchar_t*, bool, uint32_t), (override));
    MOCK_METHOD(void, ExitApplication, (), (override));
};

class MockOSService : public IOSService {
public:
    MOCK_METHOD(void, SetCursorPointer, (), (override));
    MOCK_METHOD(void, SetCursorArrow, (), (override));
};

class MockBattleService : public IBattleService {
public:
    MOCK_METHOD(void, StartStage, (), (override));
    MOCK_METHOD(int, GetCurrentEnemyIndex, (), (const, override));
    MOCK_METHOD(const std::vector<EnemyConfig>&, GetSequence, (), (const, override));
    MOCK_METHOD(void, LoadEnemy, (const EnemyConfig&), (override));
    MOCK_METHOD(Enemy&, GetEnemy, (), (override));
    MOCK_METHOD(Player&, GetPlayer, (), (override));
    MOCK_METHOD(int, GetTotalEnemyCount, (), (const, override));
    MOCK_METHOD(void, SaveState, (int), (override));
    MOCK_METHOD(bool, LoadState, (int), (override));
};

class MockCardService : public ICardService {
public:
    MOCK_METHOD(void, Start, (), (override));
    MOCK_METHOD(Card, DrawCard, (), (override));
    MOCK_METHOD(void, DiscardHand, (), (override));
    MOCK_METHOD(std::vector<Card>, GetHandCards, (), (override));
    MOCK_METHOD(std::vector<Card>, GetDrawCards, (), (override));
    MOCK_METHOD(std::vector<Card>, GetDiscardCards, (), (override));
    MOCK_METHOD(std::vector<CardData>, GetHandForSave, (), (const, override));
    MOCK_METHOD(std::vector<CardData>, GetDrawPileForSave, (), (const, override));
    MOCK_METHOD(std::vector<CardData>, GetDiscardPileForSave, (), (const, override));
    MOCK_METHOD(void, SetHandFromLoad, (const std::vector<CardData>&), (override));
    MOCK_METHOD(void, SetDrawPileFromLoad, (const std::vector<CardData>&), (override));
    MOCK_METHOD(void, SetDiscardPileFromLoad, (const std::vector<CardData>&), (override));
};

} // namespace mc
