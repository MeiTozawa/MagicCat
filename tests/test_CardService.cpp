#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockServices.h"

import CardService;
import ConfigService;
import ServiceLocator;
import EventBus;
import Character;
import Player;

namespace mc {
namespace {

    using ::testing::ReturnRef;

    class CardServiceTest : public ::testing::Test {
    protected:
        std::shared_ptr<MockConfigService> mockConfig;
        std::vector<CardConfig> mockCardConfigs;

        void SetUp() override {
            mockConfig = std::make_shared<MockConfigService>();
            
            mockCardConfigs = {
                {0, 2}, // Rock 2
                {1, 3}, // Scissors 3
                {2, 4}, // Paper 4
                {3, 5}  // Magic 5
            };
            
            EXPECT_CALL(*mockConfig, GetCardConfigs())
                .WillRepeatedly(ReturnRef(mockCardConfigs));
                
            ServiceLocator::RegisterSingleton<IConfigService, MockConfigService>(mockConfig);
        }

        void TearDown() override {
            ServiceLocator::RegisterSingleton<IConfigService, MockConfigService>(nullptr);
        }
    };

    TEST_F(CardServiceTest, Start_InitializesPiles) {
        auto cardService = CreateCardService();
        cardService->Start();

        auto drawCards = cardService->GetDrawCards();
        EXPECT_EQ(drawCards.size(), 4);
        EXPECT_EQ(cardService->GetHandCards().size(), 0);
        EXPECT_EQ(cardService->GetDiscardCards().size(), 0);
    }

    TEST_F(CardServiceTest, DrawCard_MovesCardToHand) {
        auto cardService = CreateCardService();
        cardService->Start();

        cardService->DrawCard();

        EXPECT_EQ(cardService->GetDrawCards().size(), 3);
        EXPECT_EQ(cardService->GetHandCards().size(), 1);
    }

    TEST_F(CardServiceTest, DrawCard_WhenEmpty_ShufflesDiscardPile) {
        auto cardService = CreateCardService();
        cardService->Start();

        // Draw all 4 cards
        for (int i = 0; i < 4; ++i) {
            cardService->DrawCard();
        }
        
        EXPECT_EQ(cardService->GetDrawCards().size(), 0);
        EXPECT_EQ(cardService->GetHandCards().size(), 4);

        // Discard hand via combat event
        EventBus::Publish<CombatEvent>({EAttackType::Rock, EAttackType::Scissors, 1, 1});

        EXPECT_EQ(cardService->GetHandCards().size(), 0);
        EXPECT_EQ(cardService->GetDiscardCards().size(), 4);

        // Draw again. Should shuffle discard pile into draw pile
        cardService->DrawCard();

        EXPECT_EQ(cardService->GetDiscardCards().size(), 0);
        EXPECT_EQ(cardService->GetDrawCards().size(), 3);
        EXPECT_EQ(cardService->GetHandCards().size(), 1);
    }

} // namespace
} // namespace mc
