#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockServices.h"

import CombatController;
import BattleService;
import CardService;
import SceneService;
import EventBus;
import InputService;
import Character;
import Player;
import Enemy;
import ConfigService;
import HealthComponent;

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

namespace mc {
namespace {

    class CombatIntegrationTest : public ::testing::Test {
    protected:
        std::unique_ptr<MockInputService> mockInput;
        std::unique_ptr<MockConfigService> mockConfig;
        std::unique_ptr<IBattleService> characterService;
        std::unique_ptr<ICardService> cardService;
        std::unique_ptr<ISceneService> sceneService;
        std::unique_ptr<ICombatController> combatController;

        std::vector<EnemyConfig> enemyConfigs;
        std::vector<CardConfig> cardConfigs;

        void SetUp() override {
            mockInput = std::make_unique<NiceMock<MockInputService>>();
            mockConfig = std::make_unique<NiceMock<MockConfigService>>();
            
            enemyConfigs = { {10, 0, 10, 10, 10, L"TestEnemy", ""} };
            cardConfigs = {
                {3, 5}, {3, 5}, {3, 5}, {3, 5}
            };
            ON_CALL(*mockConfig, GetEnemyConfigs()).WillByDefault(testing::ReturnRef(enemyConfigs));
            ON_CALL(*mockConfig, GetCardConfigs()).WillByDefault(testing::ReturnRef(cardConfigs));

            cardService = CreateCardService(*mockConfig);
            sceneService = CreateSceneService();
            characterService = CreateBattleService(*mockConfig, *cardService, *sceneService);

            characterService->StartStage();
            cardService->Start();
            
            combatController = CreateCombatController(*mockInput, *characterService, *sceneService, *cardService);
        }

        void TearDown() override {
        }
    };

    TEST_F(CombatIntegrationTest, MagicAction_SetsEnemyExposed_IfSuccessful) {
        characterService->GetPlayer().ChangeMp(100);
        
        int frame = 0;
        ON_CALL(*mockInput, IsPressed(testing::_)).WillByDefault(testing::Invoke([&](InputAction action) {
            if (frame == 0 && action == InputAction::IgConfirm) return true;
            if (frame == 1 && action == InputAction::IgDown) return true;
            if (frame == 2 && action == InputAction::IgConfirm) return true;
            return false;
        }));
        
        combatController->Reset();
        
        combatController->Update(0.1f); frame++;
        combatController->Update(0.1f); frame++;
        combatController->Update(0.1f); frame++;
        
        EXPECT_TRUE(characterService->GetEnemy().IsExposed());
    }

    TEST_F(CombatIntegrationTest, DrawCard_UpdatesPiles_AndShufflesWhenEmpty) {
        ON_CALL(*mockInput, IsPressed(InputAction::IgDrawCard)).WillByDefault(Return(true));
        
        EXPECT_EQ(cardService->GetDrawCards().size(), 4);
        EXPECT_EQ(cardService->GetHandCards().size(), 0);
        
        for (int i = 0; i < 4; i++) {
            combatController->Update(0.1f);
        }
        
        EXPECT_EQ(cardService->GetDrawCards().size(), 0);
        EXPECT_EQ(cardService->GetHandCards().size(), 4);
        
        cardService->DiscardHand();
        EXPECT_EQ(cardService->GetDiscardCards().size(), 4);
        EXPECT_EQ(cardService->GetHandCards().size(), 0);
        
        combatController->Update(0.1f);
        
        EXPECT_EQ(cardService->GetHandCards().size(), 1);
        EXPECT_EQ(cardService->GetDrawCards().size(), 3);
        EXPECT_EQ(cardService->GetDiscardCards().size(), 0);
    }

    TEST_F(CombatIntegrationTest, Combat_PlayerDeath_FiresEvent) {
        bool deathFired = false;
        auto handle = EventBus::Subscribe<DeathEvent>([&](const DeathEvent&) {
            deathFired = true;
        });
        
        characterService->GetPlayer().ChangeMp(0);
        characterService->GetEnemy().TakeDamage(-100);
        
        ON_CALL(*mockInput, IsPressed(InputAction::IgDown)).WillByDefault(Return(true));
        combatController->Update(0.1f);
        
        ON_CALL(*mockInput, IsPressed(InputAction::IgDown)).WillByDefault(Return(false));
        ON_CALL(*mockInput, IsPressed(InputAction::IgConfirm)).WillByDefault(Return(true));
        
        characterService->GetPlayer().TakeDamage(1000); 
        
        EXPECT_TRUE(deathFired);
        
        EventBus::Unsubscribe(handle);
    }

} // namespace
} // namespace mc
