// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before gtest includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

import AssetEnumMapper;
import CardService;
import Character;
import AssetService;

namespace mc {
namespace {

struct SpriteToSoundMapping
{
    ESprite sprite;
    ESound  expected;
};

constexpr SpriteToSoundMapping kSpriteToSoundMappings[] = {
    // CatAttack group
    { ESprite::MeowingCat,       ESound::CatAttack   },
    // WolfAttack group
    { ESprite::Wolf,             ESound::WolfAttack  },
    { ESprite::TimberWolf,       ESound::WolfAttack  },
    { ESprite::SnowFox,          ESound::WolfAttack  },
    { ESprite::Bunny,            ESound::WolfAttack  },
    { ESprite::StinkySkunk,      ESound::WolfAttack  },
    // PigAttack group
    { ESprite::DaintyPig,        ESound::PigAttack   },
    { ESprite::MadBoar,          ESound::PigAttack   },
    { ESprite::SlowTurtle,       ESound::PigAttack   },
    { ESprite::SpikeyPorcupine,  ESound::PigAttack   },
    { ESprite::CoralCrab,        ESound::PigAttack   },
    // SheepAttack group
    { ESprite::PasturingSheep,   ESound::SheepAttack },
    { ESprite::CluckingChicken,  ESound::SheepAttack },
    { ESprite::TinyChick,        ESound::SheepAttack },
    { ESprite::HonkingGoose,     ESound::SheepAttack },
    { ESprite::CroakingToad,     ESound::SheepAttack },
    { ESprite::LeapingFrog,      ESound::SheepAttack },
};

// ---------------------------------------------------------------------------
// Unit tests — SpriteToAttackSound specific mapping groups
// ---------------------------------------------------------------------------

TEST(SpriteToAttackSound, MeowingCat_ReturnsCatAttack)
{
    EXPECT_EQ(SpriteToAttackSound(ESprite::MeowingCat), ESound::CatAttack);
}

TEST(SpriteToAttackSound, WolfGroup_ReturnsWolfAttack)
{
    EXPECT_EQ(SpriteToAttackSound(ESprite::Wolf),        ESound::WolfAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::TimberWolf),  ESound::WolfAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::SnowFox),     ESound::WolfAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::Bunny),       ESound::WolfAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::StinkySkunk), ESound::WolfAttack);
}

TEST(SpriteToAttackSound, PigGroup_ReturnsPigAttack)
{
    EXPECT_EQ(SpriteToAttackSound(ESprite::DaintyPig),       ESound::PigAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::MadBoar),         ESound::PigAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::SlowTurtle),      ESound::PigAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::SpikeyPorcupine), ESound::PigAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::CoralCrab),       ESound::PigAttack);
}

TEST(SpriteToAttackSound, SheepGroup_ReturnsSheepAttack)
{
    EXPECT_EQ(SpriteToAttackSound(ESprite::PasturingSheep),  ESound::SheepAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::CluckingChicken), ESound::SheepAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::TinyChick),       ESound::SheepAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::HonkingGoose),    ESound::SheepAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::CroakingToad),    ESound::SheepAttack);
    EXPECT_EQ(SpriteToAttackSound(ESprite::LeapingFrog),     ESound::SheepAttack);
}

// ---------------------------------------------------------------------------
// Fallback tests — ESprite::Null and unknown / out-of-range values
// ---------------------------------------------------------------------------

TEST(SpriteToAttackSound, Null_ReturnsCatAttack)
{
    EXPECT_EQ(SpriteToAttackSound(ESprite::Null), ESound::CatAttack);
}

TEST(SpriteToAttackSound, UnknownValue_ReturnsCatAttack)
{
    // Cast an arbitrary out-of-range integer; function must not assert or throw
    EXPECT_EQ(SpriteToAttackSound(static_cast<ESprite>(9999)), ESound::CatAttack);
    EXPECT_EQ(SpriteToAttackSound(static_cast<ESprite>(-1)),   ESound::CatAttack);
}

// ---------------------------------------------------------------------------
// Property test — Requirements 2.2, 2.3
//
// Feature: code-readability-refactor, Property 1:
// For any ESprite in the known mapping table, SpriteToAttackSound returns the
// expected ESound. Validates: Requirements 2.2, 2.3
// ---------------------------------------------------------------------------

RC_GTEST_PROP(SpriteToAttackSound_Property, AllKnownSpritesMapCorrectly, ())
{
    constexpr int tableSize = static_cast<int>(std::size(kSpriteToSoundMappings));
    const int idx = *rc::gen::inRange(0, tableSize);

    const auto& m = kSpriteToSoundMappings[idx];
    RC_ASSERT(SpriteToAttackSound(m.sprite) == m.expected);
}

struct CardTypeMapping
{
    ECardType  cardType;
    EImage     expected;
};

constexpr CardTypeMapping kCardTypeMappings[] = {
    { ECardType::Rock,     EImage::Rock     },
    { ECardType::Scissors, EImage::Scissors },
    { ECardType::Paper,    EImage::Paper    },
    { ECardType::Magic,    EImage::Magic    },
    { ECardType::Null,     EImage::Null     },
};

struct AttackTypeMapping
{
    EAttackType attackType;
    EImage      expected;
};

constexpr AttackTypeMapping kAttackTypeMappings[] = {
    { EAttackType::Rock,     EImage::Rock     },
    { EAttackType::Scissors, EImage::Scissors },
    { EAttackType::Paper,    EImage::Paper    },
};

TEST(ToImage_ECardType, AllEnumeratorsMappedCorrectly)
{
    for (const auto& m : kCardTypeMappings)
    {
        EXPECT_EQ(ToImage(m.cardType), m.expected)
            << "ECardType value "
            << static_cast<int>(m.cardType)
            << " did not map to the expected EImage";
    }
}

TEST(ToImage_EAttackType, AllEnumeratorsMappedCorrectly)
{
    for (const auto& m : kAttackTypeMappings)
    {
        EXPECT_EQ(ToImage(m.attackType), m.expected)
            << "EAttackType value "
            << static_cast<int>(m.attackType)
            << " did not map to the expected EImage";
    }
}

// Out-of-range behaviour differs between debug and release builds.
// Debug: assert(false) fires before the return → EXPECT_DEATH.
// Release (NDEBUG): assert compiled out, EImage::Null returned.
#ifdef NDEBUG

TEST(ToImage_ECardType, OutOfRangeReturnsNull)
{
    EXPECT_EQ(ToImage(static_cast<ECardType>(9999)), EImage::Null);
}

TEST(ToImage_EAttackType, OutOfRangeReturnsNull)
{
    EXPECT_EQ(ToImage(static_cast<EAttackType>(9999)), EImage::Null);
}

#else

TEST(ToImage_ECardType, OutOfRangeTriggersAssert)
{
    EXPECT_DEATH(ToImage(static_cast<ECardType>(9999)), "");
}

TEST(ToImage_EAttackType, OutOfRangeTriggersAssert)
{
    EXPECT_DEATH(ToImage(static_cast<EAttackType>(9999)), "");
}

#endif // NDEBUG

RC_GTEST_PROP(ToImage_Property, ECardType_AllTableEntriesMapCorrectly, ())
{
    constexpr int tableSize = static_cast<int>(std::size(kCardTypeMappings));
    const int idx = *rc::gen::inRange(0, tableSize);

    const auto& m = kCardTypeMappings[idx];
    RC_ASSERT(ToImage(m.cardType) == m.expected);
}

RC_GTEST_PROP(ToImage_Property, EAttackType_AllTableEntriesMapCorrectly, ())
{
    constexpr int tableSize = static_cast<int>(std::size(kAttackTypeMappings));
    const int idx = *rc::gen::inRange(0, tableSize);

    const auto& m = kAttackTypeMappings[idx];
    RC_ASSERT(ToImage(m.attackType) == m.expected);
}

} // namespace
} // namespace mc
