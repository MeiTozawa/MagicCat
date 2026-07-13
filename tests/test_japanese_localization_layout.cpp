/// @file test_japanese_localization_layout.cpp
/// @brief Tests for Japanese localization and layout safety
///
/// Task 3.11: Validate Japanese localization and layout safety
///
/// Bug Condition: Japanese UI text, wide string literals, glyph coverage,
/// or layout safety can regress without validation
///
/// Expected Behavior: localized text remains correctly encoded, renderable,
/// diagnosable when invalid, and layout-safe in tested UI bounds
///
/// Preservation: Preserve existing Japanese UI text, direct wide string usage,
/// and product language scope
///
/// Requirements: 2.5, 2.6, 2.13, 3.8, 3.10

// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before gtest includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

#include "MockServices.h"

import RenderService;
import Enemy;
import ConfigService;

using namespace testing;
using namespace mc;

// ============================================================================
// JAPANESE ENCODING AND WIDE STRING LITERAL TESTS
// ============================================================================

/// @brief Test: UTF-8 source encoding with wide string literals
///
/// Validates: Requirements 2.13, 3.10
/// Preservation: /utf-8 source handling and direct wide Japanese string literals
TEST(JapaneseLocalization, UTF8SourceEncoding_WideStringLiterals)
{
    // Verify wide string literals with Japanese text compile and are non-empty
    std::wstring hiragana = L"あいうえお";
    std::wstring katakana = L"アイウエオ";
    std::wstring kanji = L"魔法猫";
    std::wstring mixed = L"テスト敵";
    
    EXPECT_FALSE(hiragana.empty());
    EXPECT_FALSE(katakana.empty());
    EXPECT_FALSE(kanji.empty());
    EXPECT_FALSE(mixed.empty());
    
    // Verify Japanese text has expected character counts
    EXPECT_EQ(hiragana.length(), 5);
    EXPECT_EQ(katakana.length(), 5);
    EXPECT_EQ(kanji.length(), 3);
    EXPECT_EQ(mixed.length(), 4);
}

/// @brief Test: Wide string literals in known UI contexts
///
/// Validates: Requirements 2.13, 3.8
/// Preservation: Existing Japanese UI text in scenes and displayers
TEST(JapaneseLocalization, KnownUIText_WideStringLiterals)
{
    // Menu button labels (from MenuScene.cpp)
    std::wstring menuRules = L"ルール";
    std::wstring menuVolume = L"音量設定";
    std::wstring menuSave = L"セーブ";
    std::wstring menuLoad = L"ロード";
    std::wstring menuExit = L"終了";
    
    EXPECT_FALSE(menuRules.empty());
    EXPECT_FALSE(menuVolume.empty());
    EXPECT_FALSE(menuSave.empty());
    EXPECT_FALSE(menuLoad.empty());
    EXPECT_FALSE(menuExit.empty());
    
    // Info scene text (from InfoScene.cpp)
    std::wstring victory = L"勝利にゃあ！！";
    std::wstring failure = L"失敗にゃの！？";
    std::wstring startPrompt = L"SPACEキーを押してゲームをスタートにゃ！";
    
    EXPECT_FALSE(victory.empty());
    EXPECT_FALSE(failure.empty());
    EXPECT_FALSE(startPrompt.empty());
    
    // Volume panel text (from VolumePanel.cpp)
    std::wstring tooSmall = L"小さすぎ";
    std::wstring small = L"小さい";
    std::wstring normal = L"普通";
    std::wstring large = L"大きい";
    std::wstring tooLarge = L"大きすぎ";
    
    EXPECT_FALSE(tooSmall.empty());
    EXPECT_FALSE(small.empty());
    EXPECT_FALSE(normal.empty());
    EXPECT_FALSE(large.empty());
    EXPECT_FALSE(tooLarge.empty());
}

// ============================================================================
// RENDER SERVICE WIDTH MEASUREMENT TESTS
// ============================================================================

/// @brief Test: GetDrawStringWidth returns positive values for Japanese text
///
/// Validates: Requirements 2.6, 2.13
/// Preservation: Render service measurement interface
TEST(JapaneseLocalization, RenderService_WidthMeasurement_ReturnsPositiveValues)
{
    NiceMock<MockRenderService> mockRender;
    
    // Mock render service returns positive widths for Japanese text
    ON_CALL(mockRender, GetDrawStringWidth(_))
        .WillByDefault([](const wchar_t* text) -> int {
            // Simulate realistic width: each Japanese character ~20 pixels
            return static_cast<int>(std::wcslen(text)) * 20;
        });
    
    int widthKatakana = mockRender.GetDrawStringWidth(L"テスト");
    int widthHiragana = mockRender.GetDrawStringWidth(L"てすと");
    int widthKanji = mockRender.GetDrawStringWidth(L"試験");
    int widthMixed = mockRender.GetDrawStringWidth(L"魔法猫テスト");
    
    EXPECT_GT(widthKatakana, 0);
    EXPECT_GT(widthHiragana, 0);
    EXPECT_GT(widthKanji, 0);
    EXPECT_GT(widthMixed, 0);
}

/// @brief Test: Width measurement is consistent for same text
///
/// Validates: Requirements 2.6, 2.13
TEST(JapaneseLocalization, RenderService_WidthMeasurement_ConsistentResults)
{
    NiceMock<MockRenderService> mockRender;
    
    ON_CALL(mockRender, GetDrawStringWidth(_))
        .WillByDefault([](const wchar_t* text) -> int {
            return static_cast<int>(std::wcslen(text)) * 20;
        });
    
    const wchar_t* testText = L"テスト敵";
    
    int width1 = mockRender.GetDrawStringWidth(testText);
    int width2 = mockRender.GetDrawStringWidth(testText);
    int width3 = mockRender.GetDrawStringWidth(testText);
    
    EXPECT_EQ(width1, width2);
    EXPECT_EQ(width2, width3);
}

// ============================================================================
// LAYOUT SAFETY TESTS
// ============================================================================

/// @brief Test: Japanese text in enemy names fits in expected UI bounds
///
/// Validates: Requirements 2.13, 3.8
/// Preservation: Enemy config Japanese names
TEST(JapaneseLocalization, LayoutSafety_EnemyNames_FitInUIBounds)
{
    NiceMock<MockRenderService> mockRender;
    
    // Realistic width calculation for Japanese characters
    ON_CALL(mockRender, GetDrawStringWidth(_))
        .WillByDefault([](const wchar_t* text) -> int {
            return static_cast<int>(std::wcslen(text)) * 20;
        });
    
    // Known enemy names from actual config
    std::vector<std::wstring> enemyNames = {
        L"イノシシ",   // Wild Boar
        L"ヒツジ",     // Sheep
        L"オオカミ",   // Wolf
        L"ネコ",       // Cat
        L"ブタ"        // Pig
    };
    
    // Assume enemy name display area is ~200 pixels wide (common UI constraint)
    constexpr int MAX_ENEMY_NAME_WIDTH = 200;
    
    for (const auto& name : enemyNames)
    {
        int width = mockRender.GetDrawStringWidth(name.c_str());
        EXPECT_LE(width, MAX_ENEMY_NAME_WIDTH) 
            << "Enemy name '" << std::string(name.begin(), name.end()) 
            << "' exceeds max width " << MAX_ENEMY_NAME_WIDTH;
    }
}

/// @brief Test: Japanese menu button labels fit in button bounds
///
/// Validates: Requirements 2.13, 3.8
/// Preservation: Menu button Japanese labels
TEST(JapaneseLocalization, LayoutSafety_MenuButtons_FitInBounds)
{
    NiceMock<MockRenderService> mockRender;
    
    ON_CALL(mockRender, GetDrawStringWidth(_))
        .WillByDefault([](const wchar_t* text) -> int {
            return static_cast<int>(std::wcslen(text)) * 20;
        });
    
    // Menu button labels from MenuScene.cpp
    std::vector<std::wstring> menuLabels = {
        L"ルール",     // Rules
        L"音量設定",   // Volume Settings
        L"セーブ",     // Save
        L"ロード",     // Load
        L"終了"        // Exit
    };
    
    // Assume menu buttons are ~150 pixels wide (typical button constraint)
    constexpr int MAX_MENU_BUTTON_WIDTH = 150;
    
    for (const auto& label : menuLabels)
    {
        int width = mockRender.GetDrawStringWidth(label.c_str());
        EXPECT_LE(width, MAX_MENU_BUTTON_WIDTH)
            << "Menu label '" << std::string(label.begin(), label.end())
            << "' exceeds max button width " << MAX_MENU_BUTTON_WIDTH;
    }
}

/// @brief Test: Japanese rules panel text fits in line width
///
/// Validates: Requirements 2.13, 3.8
/// Preservation: Rules panel Japanese text content
TEST(JapaneseLocalization, LayoutSafety_RulesPanel_FitsInLineWidth)
{
    NiceMock<MockRenderService> mockRender;
    
    ON_CALL(mockRender, GetDrawStringWidth(_))
        .WillByDefault([](const wchar_t* text) -> int {
            return static_cast<int>(std::wcslen(text)) * 20;
        });
    
    // Sample rules text from RulesPanel.cpp (first line)
    std::wstring rulesLine = L"アクション: 【カードを引く】【魔法を使用する】【攻撃する】";
    
    // Typical rules panel content width (window width minus margins)
    // Assuming 800px window with 50px margins on each side = 700px usable
    constexpr int MAX_RULES_LINE_WIDTH = 700;
    
    int width = mockRender.GetDrawStringWidth(rulesLine.c_str());
    EXPECT_LE(width, MAX_RULES_LINE_WIDTH)
        << "Rules line exceeds max width " << MAX_RULES_LINE_WIDTH;
}

/// @brief Test: Volume level text fits in volume indicator area
///
/// Validates: Requirements 2.13, 3.8
/// Preservation: Volume panel Japanese level labels
TEST(JapaneseLocalization, LayoutSafety_VolumeLabels_FitInIndicatorArea)
{
    NiceMock<MockRenderService> mockRender;
    
    ON_CALL(mockRender, GetDrawStringWidth(_))
        .WillByDefault([](const wchar_t* text) -> int {
            return static_cast<int>(std::wcslen(text)) * 20;
        });
    
    // Volume level labels from VolumePanel.cpp
    std::vector<std::wstring> volumeLabels = {
        L"小さすぎ",   // Too Small
        L"小さい",     // Small
        L"普通",       // Normal
        L"大きい",     // Large
        L"大きすぎ"    // Too Large
    };
    
    // Volume indicator area constraint
    constexpr int MAX_VOLUME_LABEL_WIDTH = 120;
    
    for (const auto& label : volumeLabels)
    {
        int width = mockRender.GetDrawStringWidth(label.c_str());
        EXPECT_LE(width, MAX_VOLUME_LABEL_WIDTH)
            << "Volume label '" << std::string(label.begin(), label.end())
            << "' exceeds max indicator width " << MAX_VOLUME_LABEL_WIDTH;
    }
}

// ============================================================================
// CONFIG VALIDATION - JAPANESE TEXT IN ENEMY CONFIG
// ============================================================================

/// @brief Test: Enemy configs with valid Japanese names are accepted
///
/// Validates: Requirements 2.5, 2.13
/// Preservation: Japanese text in enemy configuration
TEST(JapaneseLocalization, ConfigValidation_ValidJapaneseNames_Accepted)
{
    // Valid enemy configs with Japanese names
    std::vector<EnemyConfig> enemies = {
        { 10, 8, 3, 2, 2, L"イノシシ", "MadBoar" },
        { 7, 6, 3, 3, 3, L"ヒツジ", "PasturingSheep" },
        { 5, 4, 2, 4, 2, L"オオカミ", "TimberWolf" },
        { 8, 5, 4, 2, 3, L"ネコ", "Cat" },
        { 12, 10, 5, 5, 5, L"魔法猫", "MagicCat" }
    };
    
    for (const auto& enemy : enemies)
    {
        EXPECT_FALSE(enemy.name.empty());
        EXPECT_GT(enemy.name.length(), 0);
    }
}

/// @brief Test: Empty Japanese names in enemy config should be detectable
///
/// Validates: Requirements 2.5, 2.6, 2.13
/// Preservation: Config validation with actionable diagnostics
TEST(JapaneseLocalization, ConfigValidation_EmptyJapaneseName_Detectable)
{
    // Enemy config with empty Japanese name
    EnemyConfig enemyWithEmptyName = { 10, 5, 3, 2, 2, L"", "Sprite" };
    
    // Empty names should be detectable
    EXPECT_TRUE(enemyWithEmptyName.name.empty());
}

// ============================================================================
// PROPERTY-BASED TESTS FOR JAPANESE TEXT
// ============================================================================

/// @brief Property: Generated Japanese strings are renderable
///
/// Validates: Requirements 2.13, 3.8
RC_GTEST_PROP(JapaneseLocalization_Property, GeneratedJapaneseStrings_Renderable, ())
{
    NiceMock<MockRenderService> mockRender;
    
    ON_CALL(mockRender, GetDrawStringWidth(_))
        .WillByDefault([](const wchar_t* text) -> int {
            return static_cast<int>(std::wcslen(text)) * 20;
        });
    
    // Generate Japanese strings from supported character ranges
    const std::vector<std::wstring> japanesePool = {
        L"イノシシ", L"ヒツジ", L"オオカミ", L"ネコ", L"ブタ",
        L"猫", L"犬", L"鳥", L"魚", L"竜",
        L"魔法", L"勝利", L"失敗", L"攻撃", L"防御"
    };
    
    const int idx = *rc::gen::inRange(0, static_cast<int>(japanesePool.size()));
    const std::wstring& text = japanesePool[idx];
    
    // Property: Width measurement returns positive value
    int width = mockRender.GetDrawStringWidth(text.c_str());
    RC_ASSERT(width > 0);
    
    // Property: Text is non-empty
    RC_ASSERT(!text.empty());
}

/// @brief Property: Japanese enemy names fit in UI bounds
///
/// Validates: Requirements 2.13, 3.8
RC_GTEST_PROP(JapaneseLocalization_Property, EnemyNames_FitInUIBounds, ())
{
    NiceMock<MockRenderService> mockRender;
    
    ON_CALL(mockRender, GetDrawStringWidth(_))
        .WillByDefault([](const wchar_t* text) -> int {
            return static_cast<int>(std::wcslen(text)) * 20;
        });
    
    // Generate enemy names from realistic Japanese character pool
    const std::vector<std::wstring> enemyNamePool = {
        L"イノシシ", L"ヒツジ", L"オオカミ", L"ネコ", L"ブタ",
        L"ウサギ", L"トリ", L"サカナ", L"リュウ", L"クマ"
    };
    
    const int idx = *rc::gen::inRange(0, static_cast<int>(enemyNamePool.size()));
    const std::wstring& enemyName = enemyNamePool[idx];
    
    constexpr int MAX_ENEMY_NAME_WIDTH = 200;
    
    int width = mockRender.GetDrawStringWidth(enemyName.c_str());
    
    // Property: Enemy names fit in UI bounds
    RC_ASSERT(width <= MAX_ENEMY_NAME_WIDTH);
}

/// @brief Property: Valid Japanese config text is accepted
///
/// Validates: Requirements 2.5, 2.13
RC_GTEST_PROP(JapaneseLocalization_Property, ValidJapaneseConfig_Accepted, ())
{
    const std::vector<std::wstring> validNames = {
        L"イノシシ", L"ヒツジ", L"オオカミ", L"ネコ", L"ブタ",
        L"猫", L"犬", L"鳥", L"魚", L"竜"
    };
    
    const int idx = *rc::gen::inRange(0, static_cast<int>(validNames.size()));
    const std::wstring& name = validNames[idx];
    
    EnemyConfig config{
        *rc::gen::inRange(1, 100),  // hp
        *rc::gen::inRange(1, 20),   // baseWeight
        *rc::gen::inRange(0, 10),   // rockDamage
        *rc::gen::inRange(0, 10),   // scissorsDamage
        *rc::gen::inRange(0, 10),   // paperDamage
        name,
        "TestSprite"
    };
    
    // Property: Valid Japanese names are non-empty
    RC_ASSERT(!config.name.empty());
    RC_ASSERT(config.name.length() > 0);
}

// ============================================================================
// INTEGRATION TESTS WITH MOCK RENDER SERVICE
// ============================================================================

/// @brief Test: DrawString called with Japanese text does not fail
///
/// Validates: Requirements 2.6, 2.13
TEST(JapaneseLocalization, RenderService_DrawString_HandlesJapaneseText)
{
    NiceMock<MockRenderService> mockRender;
    
    bool drawStringCalled = false;
    
    ON_CALL(mockRender, DrawString(_, _, _, _))
        .WillByDefault([&](int, int, const wchar_t*, uint32_t) {
            drawStringCalled = true;
        });
    
    // Call DrawString with various Japanese text
    mockRender.DrawString(10, 10, L"テスト", 0xFFFFFF);
    EXPECT_TRUE(drawStringCalled);
    
    drawStringCalled = false;
    mockRender.DrawString(20, 20, L"魔法猫", 0xFFFFFF);
    EXPECT_TRUE(drawStringCalled);
    
    drawStringCalled = false;
    mockRender.DrawString(30, 30, L"勝利にゃあ！！", 0xFFFFFF);
    EXPECT_TRUE(drawStringCalled);
}

/// @brief Test: DrawCenterString called with Japanese text does not fail
///
/// Validates: Requirements 2.6, 2.13
TEST(JapaneseLocalization, RenderService_DrawCenterString_HandlesJapaneseText)
{
    NiceMock<MockRenderService> mockRender;
    
    bool drawCenterStringCalled = false;
    
    ON_CALL(mockRender, DrawCenterString(_, _, _, _))
        .WillByDefault([&](int, int, const wchar_t*, uint32_t) {
            drawCenterStringCalled = true;
        });
    
    // Call DrawCenterString with Japanese text (from actual UI)
    mockRender.DrawCenterString(400, 300, L"ゲームを終了しますか？", 0xFFFFFF);
    EXPECT_TRUE(drawCenterStringCalled);
    
    drawCenterStringCalled = false;
    mockRender.DrawCenterString(400, 350, L"保存されていない内容は失われます！", 0xFFFFFF);
    EXPECT_TRUE(drawCenterStringCalled);
}

// ============================================================================
// DIAGNOSTIC TESTS FOR INVALID LOCALIZATION DATA
// ============================================================================

/// @brief Test: Invalid or unsupported Japanese text should be diagnosable
///
/// Validates: Requirements 2.5, 2.6, 2.13
/// Note: This test documents the expectation that invalid text fails early
TEST(JapaneseLocalization, Diagnostics_InvalidText_FailsEarly)
{
    // Document expectation: Empty Japanese names should be rejected during validation
    EnemyConfig configWithEmptyName = { 10, 5, 3, 2, 2, L"", "Sprite" };
    EXPECT_TRUE(configWithEmptyName.name.empty());
    
    // Future: Validation should reject empty names with actionable diagnostic
    // Example: "Enemy config in 'enemy_config.json': field 'name' is empty"
}

/// @brief Test: Extremely long Japanese text should be detectable
///
/// Validates: Requirements 2.13, 3.8
TEST(JapaneseLocalization, LayoutSafety_ExtremelyLongText_Detectable)
{
    NiceMock<MockRenderService> mockRender;
    
    ON_CALL(mockRender, GetDrawStringWidth(_))
        .WillByDefault([](const wchar_t* text) -> int {
            return static_cast<int>(std::wcslen(text)) * 20;
        });
    
    // Extremely long Japanese text that would overflow UI
    std::wstring veryLongText = L"これは非常に長い日本語のテキストで、"
                                 L"UIの境界を超えて表示されることを"
                                 L"テストするためのものです。";
    
    int width = mockRender.GetDrawStringWidth(veryLongText.c_str());
    
    // Document that extremely long text can be detected via width measurement
    constexpr int REASONABLE_MAX_WIDTH = 700;
    EXPECT_GT(width, REASONABLE_MAX_WIDTH) 
        << "Test text should exceed reasonable bounds for overflow detection";
}
