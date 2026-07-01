// Unit and property-based tests for ViewEnumMapper::ToImage.
// Verifies all ECardType and EAttackType enumerators map to the documented EImage,
// and that out-of-range casts return EImage::Null.
//
// Feature: code-review-fixes, Property 7: ToImage covers all enumerators
// Validates: Requirements 12.1, 12.2, 12.3

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

// ---------------------------------------------------------------------------
// Compile-time tables mapping every defined enumerator to its expected EImage.
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Req 12.1 — ToImage(ECardType) maps every enumerator to the correct EImage.
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Req 12.2 — ToImage(EAttackType) maps every enumerator to the correct EImage.
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Req 12.3 — Out-of-range casts must return EImage::Null.
//
// The ViewEnumMapper implementation contains assert(false) in the default branch
// (debug aid) followed by return EImage::Null.
//   - Debug builds: the assert fires — EXPECT_DEATH validates this intended behaviour.
//   - Release builds (NDEBUG): the assert is compiled out, and EImage::Null is returned.
// ---------------------------------------------------------------------------

#ifdef NDEBUG

TEST(ToImage_ECardType, OutOfRangeReturnsNull)
{
    // In release builds the assert is compiled out; the function returns EImage::Null.
    EXPECT_EQ(ToImage(static_cast<ECardType>(9999)), EImage::Null);
}

TEST(ToImage_EAttackType, OutOfRangeReturnsNull)
{
    // In release builds the assert is compiled out; the function returns EImage::Null.
    EXPECT_EQ(ToImage(static_cast<EAttackType>(9999)), EImage::Null);
}

#else // Debug build — assert fires before the return

TEST(ToImage_ECardType, OutOfRangeTriggersAssert)
{
    // The default branch has assert(false) — the process terminates in debug builds.
    // This validates that invalid values are caught early during development.
    EXPECT_DEATH(ToImage(static_cast<ECardType>(9999)), "");
}

TEST(ToImage_EAttackType, OutOfRangeTriggersAssert)
{
    // The default branch has assert(false) — the process terminates in debug builds.
    EXPECT_DEATH(ToImage(static_cast<EAttackType>(9999)), "");
}

#endif // NDEBUG

// ---------------------------------------------------------------------------
// Property 7: ToImage(ECardType) and ToImage(EAttackType) cover all defined
// enumerators — verified via property-based test over table indices.
//
// Feature: code-review-fixes, Property 7: ToImage covers all enumerators
// Validates: Requirements 12.1, 12.2, 12.3
// ---------------------------------------------------------------------------

RC_GTEST_PROP(ToImage_Property, ECardType_AllTableEntriesMapCorrectly, ())
{
    constexpr int tableSize = static_cast<int>(std::size(kCardTypeMappings));

    // Generate an arbitrary valid index into the ECardType mapping table.
    const int idx = *rc::gen::inRange(0, tableSize);

    const auto& m = kCardTypeMappings[idx];
    RC_ASSERT(ToImage(m.cardType) == m.expected);
}

RC_GTEST_PROP(ToImage_Property, EAttackType_AllTableEntriesMapCorrectly, ())
{
    constexpr int tableSize = static_cast<int>(std::size(kAttackTypeMappings));

    // Generate an arbitrary valid index into the EAttackType mapping table.
    const int idx = *rc::gen::inRange(0, tableSize);

    const auto& m = kAttackTypeMappings[idx];
    RC_ASSERT(ToImage(m.attackType) == m.expected);
}

} // namespace
} // namespace mc
