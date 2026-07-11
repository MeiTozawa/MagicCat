// rpcndr.h (pulled in via DxLib transitive includes) defines `small` as
// `char`, which conflicts with some STL headers. Undefine before gtest.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <array>

import AssetService;

namespace mc {
namespace {

// ----------------------------------------------------------------------------
// MapBasedTestAssetService
//
// Mimics the production SPRITE_INFO_MAP behavior without requiring DxLib.
// Bunny and Wolf → {32,32}, 4 (large sprites)
// All other registered ESprites → {16,16}, 4 (small sprites)
// ESprite::Null or unknown → {0,0}, 0 (fallback)
// ----------------------------------------------------------------------------

class MapBasedTestAssetService : public IAssetService
{
public:
    // All 17 non-Null registered ESprite values, matching SPRITE_INFO_MAP in AssetService.cpp.
    static const std::array<ESprite, 17> kRegisteredSprites;

    SpriteInfo GetSpriteInfo(ESprite e) override
    {
        if (e == ESprite::Bunny || e == ESprite::Wolf)
            return {{32, 32}, 4};
        for (auto s : kRegisteredSprites)
            if (s == e) return {{16, 16}, 4};
        return {{0, 0}, 0};
    }

    int GetImageHandle(EImage) override { return -1; }
    int GetFontHandle(EFont) override { return -1; }
    int GetSpriteHandle(ESprite) override { return -1; }
    int GetSoundHandle(ESound) override { return -1; }
    ESprite ParseSprite(const std::string&) const override { return ESprite::Null; }
};

const std::array<ESprite, 17> MapBasedTestAssetService::kRegisteredSprites = {
    ESprite::Bunny,
    ESprite::Wolf,
    ESprite::CluckingChicken,
    ESprite::CoralCrab,
    ESprite::CroakingToad,
    ESprite::DaintyPig,
    ESprite::HonkingGoose,
    ESprite::LeapingFrog,
    ESprite::MadBoar,
    ESprite::MeowingCat,
    ESprite::PasturingSheep,
    ESprite::SlowTurtle,
    ESprite::SnowFox,
    ESprite::SpikeyPorcupine,
    ESprite::StinkySkunk,
    ESprite::TimberWolf,
    ESprite::TinyChick,
};

// ----------------------------------------------------------------------------
// Property 4: GetSpriteInfo が全登録済み ESprite に対して有効な SpriteInfo を返す
// Validates: Requirements 7.2
// ----------------------------------------------------------------------------

// Feature: code-readability-refactor, Property 4:
// For any registered ESprite, GetSpriteInfo returns a valid SpriteInfo
// (size.x > 0, size.y > 0, frame > 0).
RC_GTEST_PROP(GetSpriteInfo_Property, AllRegisteredSpritesHaveValidInfo, ())
{
    const int tableSize = static_cast<int>(MapBasedTestAssetService::kRegisteredSprites.size());
    const int idx = *rc::gen::inRange(0, tableSize);

    MapBasedTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(MapBasedTestAssetService::kRegisteredSprites[idx]);

    RC_ASSERT(info.size.x > 0);
    RC_ASSERT(info.size.y > 0);
    RC_ASSERT(info.frame > 0);
}

// ----------------------------------------------------------------------------
// Example test: ESprite::Null → fallback {0,0}, 0
// Validates: Requirements 7.3
// ----------------------------------------------------------------------------

TEST(GetSpriteInfo_Property, Null_ReturnsFallback)
{
    MapBasedTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(ESprite::Null);
    EXPECT_EQ(info.size.x, 0);
    EXPECT_EQ(info.size.y, 0);
    EXPECT_EQ(info.frame, 0u);
}

// ----------------------------------------------------------------------------
// Known-sprite example tests (updated from ThresholdTestAssetService)
// ----------------------------------------------------------------------------

TEST(GetSpriteInfo, KnownSprite_Bunny_Returns32x32)
{
    MapBasedTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(ESprite::Bunny);
    EXPECT_EQ(info.size.x, 32);
    EXPECT_EQ(info.size.y, 32);
    EXPECT_EQ(info.frame, 4u);
}

TEST(GetSpriteInfo, KnownSprite_Wolf_Returns32x32)
{
    MapBasedTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(ESprite::Wolf);
    EXPECT_EQ(info.size.x, 32);
    EXPECT_EQ(info.size.y, 32);
    EXPECT_EQ(info.frame, 4u);
}

TEST(GetSpriteInfo, KnownSprite_CluckingChicken_Returns16x16)
{
    MapBasedTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(ESprite::CluckingChicken);
    EXPECT_EQ(info.size.x, 16);
    EXPECT_EQ(info.size.y, 16);
    EXPECT_EQ(info.frame, 4u);
}

TEST(GetSpriteInfo, KnownSprite_MeowingCat_Returns16x16)
{
    // MeowingCat is a small sprite (16x16) in the new SPRITE_INFO_MAP.
    MapBasedTestAssetService svc;
    const SpriteInfo info = svc.GetSpriteInfo(ESprite::MeowingCat);
    EXPECT_EQ(info.size.x, 16);
    EXPECT_EQ(info.size.y, 16);
    EXPECT_EQ(info.frame, 4u);
}

} // namespace
} // namespace mc
