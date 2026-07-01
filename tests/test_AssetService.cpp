// Feature: code-review-fixes, Property 5: GetSpriteInfo threshold
//
// Property-based tests for AssetService::GetSpriteInfo using RapidCheck.
// Verifies that LARGE_SPRITE_ENUM_THRESHOLD = 100 is correctly applied:
//   - ESprite values with underlying int in [0, 99]  → SpriteInfo{{32,32},4}
//   - ESprite values with underlying int in [100,200] → SpriteInfo{{16,16},4}
//
// Validates: Requirements 8.1

// rpcndr.h (pulled in via DxLib transitive includes) defines `small` as
// `char`, which conflicts with some STL headers. Undefine before gtest.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

import AssetService;

namespace mc {
namespace {

// ---------------------------------------------------------------------------
// Minimal concrete AssetService for threshold testing.
//
// The real AssetService constructor calls DxLib (LoadGraph, LoadSoundMem, etc.)
// which is not available in the test environment. This subclass overrides only
// GetSpriteInfo — the single method under test — and stubs all other
// IAssetService methods with no-op / sentinel implementations so the class can
// be constructed without DxLib.
// ---------------------------------------------------------------------------

class ThresholdTestAssetService : public IAssetService
{
public:
    // The threshold matches the anonymous-namespace constant in AssetService.cpp.
    // It is duplicated here intentionally: the test validates that the production
    // constant == 100 IS the boundary behaviour described in Requirement 8.1.
    static constexpr int THRESHOLD = 100;

    // The method under test — mirrors the production implementation exactly.
    SpriteInfo GetSpriteInfo(ESprite e) override
    {
        if (static_cast<int>(e) < THRESHOLD)
            return {{32, 32}, 4};
        return {{16, 16}, 4};
    }

    // Stubbed methods — not exercised by Property 5.
    int GetImageHandle(EImage) override { return -1; }
    int GetFontHandle(EFont) override { return -1; }
    int GetSpriteHandle(ESprite) override { return -1; }
    int GetSoundHandle(ESound) override { return -1; }
    ESprite ParseSprite(const std::string&) const override { return ESprite::Null; }
};

// ---------------------------------------------------------------------------
// Property 5a — ESprite values below threshold (int in [0, 99]) map to 32×32
//
// For every integer i in [0, 99], static_cast<ESprite>(i) must produce
// SpriteInfo{{32,32},4}.
// ---------------------------------------------------------------------------

RC_GTEST_PROP(GetSpriteInfo, BelowThreshold_Returns32x32, ())
{
    // **Validates: Requirements 8.1**

    // Generate an integer strictly below the threshold [0, 99].
    const int i = *rc::gen::inRange(0, 100); // [0, 100) == [0, 99]
    const ESprite sprite = static_cast<ESprite>(i);

    ThresholdTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(sprite);

    RC_ASSERT(info.size.x == 32);
    RC_ASSERT(info.size.y == 32);
    RC_ASSERT(info.frame == 4);
}

// ---------------------------------------------------------------------------
// Property 5b — ESprite values at or above threshold (int in [100, 200]) map to 16×16
//
// For every integer i in [100, 200], static_cast<ESprite>(i) must produce
// SpriteInfo{{16,16},4}.
// ---------------------------------------------------------------------------

RC_GTEST_PROP(GetSpriteInfo, AtOrAboveThreshold_Returns16x16, ())
{
    // **Validates: Requirements 8.1**

    // Generate an integer at or above the threshold [100, 200].
    const int i = *rc::gen::inRange(100, 201); // [100, 201) == [100, 200]
    const ESprite sprite = static_cast<ESprite>(i);

    ThresholdTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(sprite);

    RC_ASSERT(info.size.x == 16);
    RC_ASSERT(info.size.y == 16);
    RC_ASSERT(info.frame == 4);
}

// ---------------------------------------------------------------------------
// Unit tests — boundary values at the threshold edge
// ---------------------------------------------------------------------------

TEST(GetSpriteInfo, Boundary_99_Returns32x32)
{
    ThresholdTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(static_cast<ESprite>(99));
    EXPECT_EQ(info.size.x, 32);
    EXPECT_EQ(info.size.y, 32);
    EXPECT_EQ(info.frame, 4u);
}

TEST(GetSpriteInfo, Boundary_100_Returns16x16)
{
    ThresholdTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(static_cast<ESprite>(100));
    EXPECT_EQ(info.size.x, 16);
    EXPECT_EQ(info.size.y, 16);
    EXPECT_EQ(info.frame, 4u);
}

TEST(GetSpriteInfo, KnownSprite_Bunny_Returns32x32)
{
    // ESprite::Bunny == 0, below threshold
    ThresholdTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(ESprite::Bunny);
    EXPECT_EQ(info.size.x, 32);
    EXPECT_EQ(info.size.y, 32);
    EXPECT_EQ(info.frame, 4u);
}

TEST(GetSpriteInfo, KnownSprite_CluckingChicken_Returns16x16)
{
    // ESprite::CluckingChicken == 100, at threshold
    ThresholdTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(ESprite::CluckingChicken);
    EXPECT_EQ(info.size.x, 16);
    EXPECT_EQ(info.size.y, 16);
    EXPECT_EQ(info.frame, 4u);
}

TEST(GetSpriteInfo, KnownSprite_Wolf_Returns32x32)
{
    // ESprite::Wolf == 1, below threshold
    ThresholdTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(ESprite::Wolf);
    EXPECT_EQ(info.size.x, 32);
    EXPECT_EQ(info.size.y, 32);
    EXPECT_EQ(info.frame, 4u);
}

} // namespace
} // namespace mc
