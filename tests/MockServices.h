#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>

import AssetService;
import AudioService;
import InputService;
import RenderService;
import ConfigService;
import BattleService;
import CardService;

namespace mc {

class MockConfigService : public IConfigService {
public:
    MOCK_METHOD(const std::vector<CardConfig>&, GetCardConfigs, (), (const, override));
    MOCK_METHOD(const std::vector<EnemyConfig>&, GetEnemyConfigs, (), (const, override));
};

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
    // IAudioService currently has no pure virtual methods except destructor
};

class MockInputService : public IInputService {
public:
    MOCK_METHOD(bool, IsPressed, (InputAction), (const, override));
    MOCK_METHOD(bool, IsHolding, (InputAction), (const, override));
    MOCK_METHOD(bool, IsReleased, (InputAction), (const, override));
    MOCK_METHOD(Point<int>, OnMouseClick, (InputAction), (const, override));
    MOCK_METHOD(InputDevice, GetActiveDevice, (), (const, override));
    MOCK_METHOD(void, PushContext, (InputContext), (override));
    MOCK_METHOD(void, PopContext, (), (override));
    MOCK_METHOD(void, ClearAndSetContext, (InputContext), (override));
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
    MOCK_METHOD(int, GetWindowWidth, (), (const, override));
    MOCK_METHOD(int, GetWindowHeight, (), (const, override));
    MOCK_METHOD(void, SetFontTypeNormal, (), (override));
    MOCK_METHOD(void, ChangeFont, (const wchar_t*), (override));
    MOCK_METHOD(void, SetFontSize, (int), (override));
    MOCK_METHOD(void, SetFontThickness, (int), (override));
    MOCK_METHOD(void, SetBackgroundColor, (int, int, int), (override));
    MOCK_METHOD(void, DrawFilledTriangle, (int, int, int, int, int, int, uint32_t), (override));
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
};

class MockCardService : public ICardService {
public:
    MOCK_METHOD(void, Start, (), (override));
    MOCK_METHOD(Card, DrawCard, (), (override));
    MOCK_METHOD(void, DiscardHand, (), (override));
    MOCK_METHOD(std::vector<Card>, GetHandCards, (), (override));
    MOCK_METHOD(std::vector<Card>, GetDrawCards, (), (override));
    MOCK_METHOD(std::vector<Card>, GetDiscardCards, (), (override));
};

} // namespace mc
