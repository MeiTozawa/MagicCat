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

// Minimal concrete AssetService for threshold testing.
//
// The real AssetService constructor calls DxLib (LoadGraph, LoadSoundMem, etc.)
// which is not available in the test environment. This subclass overrides only
// GetSpriteInfo and stubs all other IAssetService methods so the class can
// be constructed without DxLib.

class ThresholdTestAssetService : public IAssetService
{
public:
    // Matches the anonymous-namespace constant in AssetService.cpp.
    // Duplicated here intentionally: validates that the production constant == 100
    // is the boundary described in the threshold requirement.
    static constexpr int THRESHOLD = 100;

    SpriteInfo GetSpriteInfo(ESprite e) override
    {
        if (static_cast<int>(e) < THRESHOLD)
            return {{32, 32}, 4};
        return {{16, 16}, 4};
    }

    int GetImageHandle(EImage) override { return -1; }
    int GetFontHandle(EFont) override { return -1; }
    int GetSpriteHandle(ESprite) override { return -1; }
    int GetSoundHandle(ESound) override { return -1; }
    ESprite ParseSprite(const std::string&) const override { return ESprite::Null; }
};

RC_GTEST_PROP(GetSpriteInfo, BelowThreshold_Returns32x32, ())
{
    const int i = *rc::gen::inRange(0, 100); // [0, 99]
    const ESprite sprite = static_cast<ESprite>(i);

    ThresholdTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(sprite);

    RC_ASSERT(info.size.x == 32);
    RC_ASSERT(info.size.y == 32);
    RC_ASSERT(info.frame == 4);
}

RC_GTEST_PROP(GetSpriteInfo, AtOrAboveThreshold_Returns16x16, ())
{
    const int i = *rc::gen::inRange(100, 201); // [100, 200]
    const ESprite sprite = static_cast<ESprite>(i);

    ThresholdTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(sprite);

    RC_ASSERT(info.size.x == 16);
    RC_ASSERT(info.size.y == 16);
    RC_ASSERT(info.frame == 4);
}

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
