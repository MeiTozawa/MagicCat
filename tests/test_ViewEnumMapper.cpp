// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before gtest includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

import ViewEnumMapper;
import CardService;
import Character;
import AssetService;

namespace mc {
namespace {

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
