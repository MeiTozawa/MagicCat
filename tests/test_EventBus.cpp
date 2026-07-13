#include <gtest/gtest.h>

import EventBus;

namespace mc {
namespace {

    struct DummyEvent : public IEvent {
        int value;
        DummyEvent(int v) : value(v) {}
    };

    struct AnotherEvent : public IEvent {
        std::string message;
        AnotherEvent(std::string msg) : message(std::move(msg)) {}
    };

    class EventBusTest : public ::testing::Test {
    protected:
        void SetUp() override {
            // Clear EventBus state before each test to prevent cross-test leakage
            EventBus::Clear();
        }
        void TearDown() override {
            // Clear EventBus state after each test to prevent cross-test leakage
            EventBus::Clear();
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

        auto handle = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) {
            callCount++;
        });

        EventBus::Publish<DummyEvent>({1});
        EXPECT_EQ(callCount, 1);

        EventBus::Unsubscribe(handle);

        EventBus::Publish<DummyEvent>({2});
        EXPECT_EQ(callCount, 1);
    }

    TEST_F(EventBusTest, MultipleSubscribers_AllReceiveEvent) {
        int callCountA = 0;
        int callCountB = 0;

        auto handleA = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) { callCountA++; });
        auto handleB = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) { callCountB++; });

        EventBus::Publish<DummyEvent>({99});

        EXPECT_EQ(callCountA, 1);
        EXPECT_EQ(callCountB, 1);

        EventBus::Unsubscribe(handleA);
        EventBus::Unsubscribe(handleB);
    }

    TEST_F(EventBusTest, Unsubscribe_InvalidHandle_DoesNotCrash) {
        auto handle = EventBus::Subscribe<DummyEvent>([](const DummyEvent&) {});
        EventBus::Unsubscribe(handle);

        EXPECT_NO_FATAL_FAILURE(EventBus::Unsubscribe(handle));
    }

    // ========== Task 3.3: EventBus Ownership and Callback Safety Hardening Tests ==========

    TEST_F(EventBusTest, TypedDelivery_OnlyTargetedSubscribersReceive) {
        int dummyCount = 0;
        int anotherCount = 0;

        auto handleDummy = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) { dummyCount++; });
        auto handleAnother = EventBus::Subscribe<AnotherEvent>([&](const AnotherEvent&) { anotherCount++; });

        EventBus::Publish<DummyEvent>({1});
        EXPECT_EQ(dummyCount, 1);
        EXPECT_EQ(anotherCount, 0);

        EventBus::Publish<AnotherEvent>({"test"});
        EXPECT_EQ(dummyCount, 1);
        EXPECT_EQ(anotherCount, 1);

        EventBus::Unsubscribe(handleDummy);
        EventBus::Unsubscribe(handleAnother);
    }

    TEST_F(EventBusTest, HandleUniqueness_EachSubscribeReturnsUniqueHandle) {
        auto handle1 = EventBus::Subscribe<DummyEvent>([](const DummyEvent&) {});
        auto handle2 = EventBus::Subscribe<DummyEvent>([](const DummyEvent&) {});
        auto handle3 = EventBus::Subscribe<AnotherEvent>([](const AnotherEvent&) {});

        EXPECT_NE(handle1, handle2);
        EXPECT_NE(handle1, handle3);
        EXPECT_NE(handle2, handle3);

        EventBus::Unsubscribe(handle1);
        EventBus::Unsubscribe(handle2);
        EventBus::Unsubscribe(handle3);
    }

    TEST_F(EventBusTest, PublishWithNoSubscribers_DoesNotCrash) {
        EXPECT_NO_FATAL_FAILURE(EventBus::Publish<DummyEvent>({42}));
        EXPECT_NO_FATAL_FAILURE(EventBus::Publish<AnotherEvent>({"test"}));
    }

    TEST_F(EventBusTest, IdempotentUnsubscribe_MultipleUnsubscribesDoNotCrash) {
        auto handle = EventBus::Subscribe<DummyEvent>([](const DummyEvent&) {});

        EventBus::Unsubscribe(handle);
        EXPECT_NO_FATAL_FAILURE(EventBus::Unsubscribe(handle));
        EXPECT_NO_FATAL_FAILURE(EventBus::Unsubscribe(handle));
        EXPECT_NO_FATAL_FAILURE(EventBus::Unsubscribe(handle));
    }

    TEST_F(EventBusTest, UnsubscribeDuringPublish_SafeAndDeterministic) {
        int callCount = 0;
        EventHandle handleToUnsubscribe = 0;

        // Subscriber A: unsubscribes handleToUnsubscribe during callback
        auto handleA = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) {
            callCount++;
            EventBus::Unsubscribe(handleToUnsubscribe);
        });

        // Subscriber B: the handle that will be unsubscribed by A
        handleToUnsubscribe = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) {
            callCount++;
        });

        // Subscriber C: should still receive the event
        auto handleC = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) {
            callCount++;
        });

        // Publish: all 3 subscribers should receive the event (snapshot before iteration)
        EventBus::Publish<DummyEvent>({1});
        EXPECT_EQ(callCount, 3);

        // Publish again: only A and C should receive (B was unsubscribed)
        EventBus::Publish<DummyEvent>({2});
        EXPECT_EQ(callCount, 5);

        EventBus::Unsubscribe(handleA);
        EventBus::Unsubscribe(handleC);
    }

    TEST_F(EventBusTest, Clear_RemovesAllSubscriptions) {
        int callCount = 0;

        auto handle1 = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) { callCount++; });
        auto handle2 = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) { callCount++; });
        auto handle3 = EventBus::Subscribe<AnotherEvent>([&](const AnotherEvent&) { callCount++; });

        EventBus::Publish<DummyEvent>({1});
        EXPECT_EQ(callCount, 2);

        EventBus::Clear();

        EventBus::Publish<DummyEvent>({2});
        EventBus::Publish<AnotherEvent>({"test"});
        EXPECT_EQ(callCount, 2); // No new callbacks fired after Clear
    }

    TEST_F(EventBusTest, NoCallbackAfterOwnerTeardown_ManualPattern) {
        int callCount = 0;

        // Simulate an owner object that stores a handle and unsubscribes in destructor
        class MockOwner {
        public:
            MockOwner(int& counter) : counter(counter) {
                handle = EventBus::Subscribe<DummyEvent>([&counter](const DummyEvent&) {
                    counter++;
                });
            }
            ~MockOwner() {
                EventBus::Unsubscribe(handle);
            }
        private:
            int& counter;
            EventHandle handle;
        };

        {
            MockOwner owner(callCount);
            EventBus::Publish<DummyEvent>({1});
            EXPECT_EQ(callCount, 1);
        } // owner destroyed, should unsubscribe

        EventBus::Publish<DummyEvent>({2});
        EXPECT_EQ(callCount, 1); // No callback after owner destruction
    }

    TEST_F(EventBusTest, ExplicitThisCapturePattern_SafeWhenOwnerOutlivesSubscription) {
        int memberValue = 0;

        class MockService {
        public:
            MockService(int& external) : externalCounter(external), memberData(0) {
                handle = EventBus::Subscribe<DummyEvent>([this](const DummyEvent& e) {
                    // Explicit [this] capture is safe here because destructor unsubscribes
                    memberData = e.value;
                    externalCounter++;
                });
            }
            ~MockService() {
                EventBus::Unsubscribe(handle);
            }
            int GetMemberData() const { return memberData; }
        private:
            int& externalCounter;
            int memberData;
            EventHandle handle;
        };

        {
            MockService service(memberValue);
            EventBus::Publish<DummyEvent>({42});
            EXPECT_EQ(service.GetMemberData(), 42);
            EXPECT_EQ(memberValue, 1);
        } // service destroyed, unsubscribed

        EventBus::Publish<DummyEvent>({99});
        EXPECT_EQ(memberValue, 1); // No callback after destruction
    }

    // ========== Additional Ownership Pattern Tests ==========

    TEST_F(EventBusTest, MultipleHandlesInVector_AllUnsubscribed) {
        int callCount = 0;
        std::vector<EventHandle> handles;

        // Subscribe multiple times
        for (int i = 0; i < 5; ++i) {
            handles.push_back(EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) {
                callCount++;
            }));
        }

        EventBus::Publish<DummyEvent>({1});
        EXPECT_EQ(callCount, 5);

        // Unsubscribe all
        for (auto handle : handles) {
            EventBus::Unsubscribe(handle);
        }

        EventBus::Publish<DummyEvent>({2});
        EXPECT_EQ(callCount, 5); // No new callbacks
    }

    TEST_F(EventBusTest, ClearBetweenTests_PreventsLeakage) {
        int callCount = 0;

        auto handle = EventBus::Subscribe<DummyEvent>([&](const DummyEvent&) {
            callCount++;
        });

        EventBus::Publish<DummyEvent>({1});
        EXPECT_EQ(callCount, 1);

        EventBus::Clear();

        // After Clear, no callbacks should fire
        EventBus::Publish<DummyEvent>({2});
        EXPECT_EQ(callCount, 1); // Should not increment
    }

} // namespace
} // namespace mc
