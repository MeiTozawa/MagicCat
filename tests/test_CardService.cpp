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
        auto cardService = CreateCardService(*mockConfig);
        cardService->Start();

        auto drawCards = cardService->GetDrawCards();
        EXPECT_EQ(drawCards.size(), 4);
        EXPECT_EQ(cardService->GetHandCards().size(), 0);
        EXPECT_EQ(cardService->GetDiscardCards().size(), 0);
    }

    TEST_F(CardServiceTest, DrawCard_MovesCardToHand) {
        auto cardService = CreateCardService(*mockConfig);
        cardService->Start();

        cardService->DrawCard();

        EXPECT_EQ(cardService->GetDrawCards().size(), 3);
        EXPECT_EQ(cardService->GetHandCards().size(), 1);
    }

    TEST_F(CardServiceTest, DrawCard_WhenEmpty_ShufflesDiscardPile) {
        auto cardService = CreateCardService(*mockConfig);
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

    TEST_F(CardServiceTest, DrawCard_WhenHandFull_ReturnsNullCard) {
        auto cardService = CreateCardService(*mockConfig);
        cardService->Start();

        // Fill the hand to HAND_SIZE_MAX (4)
        for (int i = 0; i < HAND_SIZE_MAX; i++) {
            cardService->DrawCard();
        }
        ASSERT_EQ(static_cast<int>(cardService->GetHandCards().size()), HAND_SIZE_MAX);

        Card extra = cardService->DrawCard();

        EXPECT_EQ(extra.CardType, ECardType::Null);
        // Hand size must not exceed the maximum
        EXPECT_EQ(static_cast<int>(cardService->GetHandCards().size()), HAND_SIZE_MAX);
    }

    TEST_F(CardServiceTest, DrawCard_WhenBothPilesEmpty_ReturnsNullCard) {
        auto cardService = CreateCardService(*mockConfig);
        cardService->Start();

        // Fill hand, discard, then fill hand again to exhaust draw pile
        for (int i = 0; i < HAND_SIZE_MAX; i++) cardService->DrawCard();
        cardService->DiscardHand();
        // Draw pile is empty; drawing reshuffles from discard
        for (int i = 0; i < HAND_SIZE_MAX; i++) cardService->DrawCard();
        // Now hand is full, draw pile empty, discard empty
        ASSERT_EQ(cardService->GetDiscardCards().size(), 0uz);
        ASSERT_EQ(cardService->GetDrawCards().size(),    0uz);

        Card result = cardService->DrawCard(); // hand is full — must return Null
        EXPECT_EQ(result.CardType, ECardType::Null);
    }

    TEST_F(CardServiceTest, DrawCard_FiresHandUpdatedEvent) {
        auto cardService = CreateCardService(*mockConfig);
        cardService->Start();

        bool eventFired = false;
        auto handle = EventBus::Subscribe<HandUpdatedEvent>([&](const HandUpdatedEvent&) {
            eventFired = true;
        });

        cardService->DrawCard();

        EXPECT_TRUE(eventFired);
        EventBus::Unsubscribe(handle);
    }

    TEST_F(CardServiceTest, DiscardHand_FiresHandUpdatedEvent) {
        auto cardService = CreateCardService(*mockConfig);
        cardService->Start();
        cardService->DrawCard(); // put a card in hand first

        bool eventFired = false;
        auto handle = EventBus::Subscribe<HandUpdatedEvent>([&](const HandUpdatedEvent&) {
            eventFired = true;
        });

        cardService->DiscardHand();

        EXPECT_TRUE(eventFired);
        EventBus::Unsubscribe(handle);
    }

} // namespace
} // namespace mc
