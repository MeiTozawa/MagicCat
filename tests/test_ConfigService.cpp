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
        std::string gameConfigPath = "test_game_config.json";

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

            std::ofstream gameOut(gameConfigPath);
            gameOut << R"({
                "stage": {
                    "battleCount": 5
                },
                "player": {
                    "initialHp": 20,
                    "maxMp": 12,
                    "sprite": "HeroCat",
                    "damage": {
                        "rock": 3,
                        "scissors": 3,
                        "paper": 3
                    },
                    "magic": {
                        "clairvoyance": {
                            "mpCost": 8
                        },
                        "powerBoost": {
                            "mpCost": 6,
                            "damageOffset": 4
                        },
                        "heal": {
                            "mpCost": 4,
                            "healAmount": 5,
                            "maxUses": 2
                        }
                    }
                }
            })";
            gameOut.close();
        }

        void TearDown() override {
            std::filesystem::remove(cardConfigPath);
            std::filesystem::remove(enemyConfigPath);
            std::filesystem::remove(gameConfigPath);
        }
    };

    TEST_F(ConfigServiceTest, LoadGameConfig_ParsesJsonFiles) {
        auto configService = CreateConfigService(cardConfigPath, enemyConfigPath, gameConfigPath);

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

        auto& playerConfig = configService->GetPlayerConfig();
        EXPECT_EQ(playerConfig.initialHp, 20);
        EXPECT_EQ(playerConfig.maxMp, 12);
        EXPECT_EQ(playerConfig.spriteName, "HeroCat");
        EXPECT_EQ(playerConfig.rockDamage, 3);
        EXPECT_EQ(playerConfig.scissorsDamage, 3);
        EXPECT_EQ(playerConfig.paperDamage, 3);
        EXPECT_EQ(playerConfig.clairvoyanceMpCost, 8);
        EXPECT_EQ(playerConfig.powerBoostMpCost, 6);
        EXPECT_EQ(playerConfig.powerBoostDamageOffset, 4);
        EXPECT_EQ(playerConfig.healMpCost, 4);
        EXPECT_EQ(playerConfig.healAmount, 5);
        EXPECT_EQ(playerConfig.maxHealUses, 2);

        auto& gameConfig = configService->GetGameConfig();
        EXPECT_EQ(gameConfig.battleCount, 5);
    }

} // namespace
} // namespace mc
