#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

import ConfigService;

namespace mc {
namespace {

    class ConfigServiceTest : public ::testing::Test {
    protected:
        std::string cardConfigPath = "test_card_config.json";
        std::string enemyConfigPath = "test_enemy_config.json";

        void SetUp() override {
            std::ofstream cardOut(cardConfigPath);
            cardOut << R"([
                {"type": 0, "value": 2},
                {"type": 1, "value": 3}
            ])";
            cardOut.close();

            std::ofstream enemyOut(enemyConfigPath);
            enemyOut << R"([{
                "name": "Test Enemy",
                "sprite": "cat.png",
                "baseWeight": 10,
                "rockDamage": 1,
                "scissorsDamage": 2,
                "paperDamage": 3,
                "hp": 42
            }])";
            enemyOut.close();
        }

        void TearDown() override {
            std::filesystem::remove(cardConfigPath);
            std::filesystem::remove(enemyConfigPath);
        }
    };

    TEST_F(ConfigServiceTest, LoadGameConfig_ParsesJsonFiles) {
        auto configService = CreateConfigService(cardConfigPath, enemyConfigPath);

        auto& cardConfigs = configService->GetCardConfigs();
        EXPECT_EQ(cardConfigs.size(), 2);
        EXPECT_EQ(cardConfigs[0].type, 0);
        EXPECT_EQ(cardConfigs[0].value, 2);
        EXPECT_EQ(cardConfigs[1].type, 1);
        EXPECT_EQ(cardConfigs[1].value, 3);

        auto& enemyConfigs = configService->GetEnemyConfigs();
        EXPECT_EQ(enemyConfigs.size(), 1);
        EXPECT_EQ(enemyConfigs[0].hp, 42);
        EXPECT_EQ(enemyConfigs[0].baseWeight, 10);
        EXPECT_EQ(enemyConfigs[0].rockDamage, 1);
        EXPECT_EQ(enemyConfigs[0].scissorsDamage, 2);
        EXPECT_EQ(enemyConfigs[0].paperDamage, 3);
        EXPECT_EQ(enemyConfigs[0].spriteName, "cat.png");
        EXPECT_EQ(enemyConfigs[0].name, L"Test Enemy");
    }

} // namespace
} // namespace mc
