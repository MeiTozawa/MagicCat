#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockServices.h"

import CardService;
import ConfigService;
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
        }

        void TearDown() override {
        }
    };

    TEST_F(CardServiceTest, Start_InitializesPiles) {
        auto cardService = CreateCardService(mockConfig.get());
        cardService->Start();

        auto drawCards = cardService->GetDrawCards();
        EXPECT_EQ(drawCards.size(), 4);
        EXPECT_EQ(cardService->GetHandCards().size(), 0);
        EXPECT_EQ(cardService->GetDiscardCards().size(), 0);
    }

    TEST_F(CardServiceTest, DrawCard_MovesCardToHand) {
        auto cardService = CreateCardService(mockConfig.get());
        cardService->Start();

        cardService->DrawCard();

        EXPECT_EQ(cardService->GetDrawCards().size(), 3);
        EXPECT_EQ(cardService->GetHandCards().size(), 1);
    }

    TEST_F(CardServiceTest, DrawCard_WhenEmpty_ShufflesDiscardPile) {
        auto cardService = CreateCardService(mockConfig.get());
        cardService->Start();

        cardService->DrawCard();
        cardService->DrawCard();
        cardService->DrawCard();
        cardService->DrawCard();

        EXPECT_EQ(cardService->GetHandCards().size(), 4);
        EXPECT_EQ(cardService->GetDiscardCards().size(), 0);

        cardService->DiscardHand();

        EXPECT_EQ(cardService->GetHandCards().size(), 0);
        EXPECT_EQ(cardService->GetDiscardCards().size(), 4);

        cardService->DrawCard();

        EXPECT_EQ(cardService->GetDrawCards().size(), 3);
        EXPECT_EQ(cardService->GetHandCards().size(), 1);
    }

} // namespace
} // namespace mc
