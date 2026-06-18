#include <gtest/gtest.h>

import EventBus;

namespace mc {
namespace {

    struct DummyEvent : public IEvent {
        int value;
        DummyEvent(int v) : value(v) {}
    };

    class EventBusTest : public ::testing::Test {
    protected:
        void SetUp() override {
            // No easy way to clear global EventBus state, so we use unique variables.
        }
        void TearDown() override {
        }
    };

    TEST_F(EventBusTest, SubscribeAndPublish_BasicFlow) {
        int receivedValue = 0;
        
        auto handle = EventBus::Subscribe<DummyEvent>([&](const DummyEvent& e) {
            receivedValue = e.value;
        });

        EventBus::Publish<DummyEvent>({42});

        EXPECT_EQ(receivedValue, 42);

        EventBus::Unsubscribe(handle);
    }

    TEST_F(EventBusTest, Unsubscribe_RemovesListener) {
        int callCount = 0;

        auto handle = EventBus::Subscribe<DummyEvent>([&](const DummyEvent& e) {
            callCount++;
        });

        EventBus::Publish<DummyEvent>({1});
        EXPECT_EQ(callCount, 1);

        EventBus::Unsubscribe(handle);

        EventBus::Publish<DummyEvent>({2});
        EXPECT_EQ(callCount, 1); // Should not increase
    }

} // namespace
} // namespace mc
