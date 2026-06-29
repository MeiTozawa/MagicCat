#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>

import AssetService;
import AudioService;
import InputService;
import RenderService;
import ConfigService;

namespace mc {

class MockConfigService : public IConfigService {
public:
    MOCK_METHOD(const std::vector<CardConfig>&, GetCardConfigs, (), (const, override));
    MOCK_METHOD(const std::vector<EnemyConfig>&, GetEnemyConfigs, (), (const, override));
};

class MockAssetService : public IAssetService {
public:
    MOCK_METHOD(const int, GetImageHandle, (EImage), (override));
    MOCK_METHOD(const int, GetFontHandle, (EFont), (override));
    MOCK_METHOD(const int, GetSpriteHandle, (ESprite), (override));
    MOCK_METHOD(const SpriteInfo, GetSpriteInfo, (ESprite), (override));
    MOCK_METHOD(const int, GetSoundHandle, (ESound), (override));
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
    MOCK_METHOD(tnl::Vector2i, OnMouseClick, (InputAction), (const, override));
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
    MOCK_METHOD(void, DrawRectRotaGraph, (int, int, int, int, int, int, double, double, int, bool, bool), (override));
    MOCK_METHOD(void, SetDrawBlendMode, (int, int), (override));
    MOCK_METHOD(void, DrawRoundRectFrame, (int, int, int, int, int, int, uint32_t), (override));
};

} // namespace mc
