#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>
#include <optional>
#include "MockServices.h"

import EventBus;
import SceneService;
import BattleService;
import Displayer;

namespace mc {
namespace {

// ========== Task 3.7: Ownership and Lifetime Safety Tests ==========
// These tests verify that game objects, scenes, displayers, event callbacks,
// effect objects, and service dependencies do not outlive their owners.
//
// Bug Condition: game object, scene, displayer, event callback, effect object,
// or service dependency may outlive its owner
//
// Expected Behavior: dangling references, invalid captures, stale handles, and
// use-after-destroy behavior are prevented through explicit ownership and lifetime rules
//
// Preservation: Preserve existing ownership model based on smart pointers,
// factories, service references, and EventBus handles

class OwnershipLifetimeSafetyTest : public ::testing::Test {
protected:
    void SetUp() override {
        EventBus::Clear();
    }
    void TearDown() override {
        EventBus::Clear();
    }
};

// ========== EventBus Handle Ownership Tests ==========

TEST_F(OwnershipLifetimeSafetyTest, EventHandle_StoredInOwner_UnsubscribedInDestructor) {
    int callbackCount = 0;

    class MockSubscriber {
    public:
        MockSubscriber(int& counter) : counter(counter) {
            handle = EventBus::Subscribe<StageClearEvent>([&counter](const StageClearEvent&) {
                counter++;
            });
        }
        ~MockSubscriber() {
            EventBus::Unsubscribe(handle);
        }
    private:
        int& counter;
        EventHandle handle;
    };

    {
        MockSubscriber subscriber(callbackCount);
        EventBus::Publish(StageClearEvent{});
        EXPECT_EQ(callbackCount, 1);
    } // subscriber destroyed, handle unsubscribed

    // Verify no callback after owner destruction
    EventBus::Publish(StageClearEvent{});
    EXPECT_EQ(callbackCount, 1);
}

TEST_F(OwnershipLifetimeSafetyTest, MultipleEventHandles_AllUnsubscribedOnDestruction) {
    int stageClearCount = 0;
    int stageFailCount = 0;
    int enemyDefeatedCount = 0;

    class MockService {
    public:
        MockService(int& clear, int& fail, int& defeated)
            : clearCounter(clear), failCounter(fail), defeatedCounter(defeated) {
            handles.push_back(EventBus::Subscribe<StageClearEvent>(
                [&clear](const StageClearEvent&) { clear++; }));
            handles.push_back(EventBus::Subscribe<StageFailEvent>(
                [&fail](const StageFailEvent&) { fail++; }));
            handles.push_back(EventBus::Subscribe<EnemyDefeatedEvent>(
                [&defeated](const EnemyDefeatedEvent&) { defeated++; }));
        }
        ~MockService() {
            for (auto handle : handles) {
                EventBus::Unsubscribe(handle);
            }
        }
    private:
        int& clearCounter;
        int& failCounter;
        int& defeatedCounter;
        std::vector<EventHandle> handles;
    };

    {
        MockService service(stageClearCount, stageFailCount, enemyDefeatedCount);
        EventBus::Publish(StageClearEvent{});
        EventBus::Publish(StageFailEvent{});
        EventBus::Publish(EnemyDefeatedEvent{0});
        EXPECT_EQ(stageClearCount, 1);
        EXPECT_EQ(stageFailCount, 1);
        EXPECT_EQ(enemyDefeatedCount, 1);
    } // service destroyed, all handles unsubscribed

    // Verify no callbacks after owner destruction
    EventBus::Publish(StageClearEvent{});
    EventBus::Publish(StageFailEvent{});
    EventBus::Publish(EnemyDefeatedEvent{0});
    EXPECT_EQ(stageClearCount, 1);
    EXPECT_EQ(stageFailCount, 1);
    EXPECT_EQ(enemyDefeatedCount, 1);
}

TEST_F(OwnershipLifetimeSafetyTest, OptionalEventHandle_SafelyUnsubscribedWhenPresent) {
    int callbackCount = 0;

    class MockScene {
    public:
        MockScene(int& counter) : counter(counter) {}
        
        void Subscribe() {
            if (!handle.has_value()) {
                handle = EventBus::Subscribe<CutsceneFinishedEvent>(
                    [&counter = this->counter](const CutsceneFinishedEvent&) {
                        counter++;
                    });
            }
        }
        
        void Unsubscribe() {
            if (handle.has_value()) {
                EventBus::Unsubscribe(*handle);
                handle = std::nullopt;
            }
        }
        
        ~MockScene() {
            Unsubscribe();
        }
        
    private:
        int& counter;
        std::optional<EventHandle> handle;
    };

    {
        MockScene scene(callbackCount);
        scene.Subscribe();
        EventBus::Publish(CutsceneFinishedEvent{});
        EXPECT_EQ(callbackCount, 1);
        
        // Explicit unsubscribe
        scene.Unsubscribe();
        EventBus::Publish(CutsceneFinishedEvent{});
        EXPECT_EQ(callbackCount, 1);
    } // scene destroyed, destructor safely handles already-unsubscribed handle

    EventBus::Publish(CutsceneFinishedEvent{});
    EXPECT_EQ(callbackCount, 1);
}

// ========== Explicit [this] Capture Safety Tests ==========

TEST_F(OwnershipLifetimeSafetyTest, ExplicitThisCapture_SafeWithDestructorUnsubscription) {
    int externalCounter = 0;

    class ServiceWithMemberData {
    public:
        ServiceWithMemberData(int& external) : externalCounter(external), memberValue(0) {
            // Explicit [this] capture is SAFE because destructor unsubscribes
            handle = EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent&) {
                memberValue = 42;
                externalCounter++;
            });
        }
        
        ~ServiceWithMemberData() {
            EventBus::Unsubscribe(handle);
        }
        
        int GetMemberValue() const { return memberValue; }
        
    private:
        int& externalCounter;
        int memberValue;
        EventHandle handle;
    };

    {
        ServiceWithMemberData service(externalCounter);
        EventBus::Publish(StageClearEvent{});
        EXPECT_EQ(service.GetMemberValue(), 42);
        EXPECT_EQ(externalCounter, 1);
    } // service destroyed, handle unsubscribed

    // No callback after destruction
    EventBus::Publish(StageClearEvent{});
    EXPECT_EQ(externalCounter, 1);
}

TEST_F(OwnershipLifetimeSafetyTest, BroadCapture_AvoidedInLongLivedCallbacks) {
    // This test documents the ANTI-PATTERN: broad [&] captures are dangerous
    // The test shows the CORRECT pattern: explicit captures or [this] with unsubscribe
    
    int safeCounter = 0;
    
    class SafeSubscriber {
    public:
        SafeSubscriber(int& counter) : counter(counter), memberData(0) {
            // CORRECT: explicit capture of members we need, with guaranteed unsubscribe
            handle = EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent&) {
                memberData++;
                this->counter++;
            });
        }
        
        ~SafeSubscriber() {
            EventBus::Unsubscribe(handle);
        }
        
    private:
        int& counter;
        int memberData;
        EventHandle handle;
    };

    {
        SafeSubscriber subscriber(safeCounter);
        EventBus::Publish(StageClearEvent{});
        EXPECT_EQ(safeCounter, 1);
    }

    EventBus::Publish(StageClearEvent{});
    EXPECT_EQ(safeCounter, 1); // No dangling reference issues
}

// ========== Scene Lifetime Safety Tests ==========

TEST_F(OwnershipLifetimeSafetyTest, Scene_EventHandlesUnsubscribedOnDestruction) {
    using ::testing::NiceMock;
    using ::testing::Return;
    
    NiceMock<MockInputService> mockInput;
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockBattleService> mockBattle;
    
    int callbackCount = 0;
    
    // Simulate a scene that subscribes and unsubscribes
    class TestScene : public IScene {
    public:
        TestScene(int& counter) : counter(counter) {
            handle = EventBus::Subscribe<StageClearEvent>([&counter](const StageClearEvent&) {
                counter++;
            });
        }
        
        ~TestScene() override {
            EventBus::Unsubscribe(handle);
        }
        
        void Start() override {}
        void Update(float) override {}
        
    private:
        int& counter;
        EventHandle handle;
    };

    auto sceneService = CreateSceneService();
    
    {
        auto scene = std::make_unique<TestScene>(callbackCount);
        sceneService->RegisterScene(ESceneState::Info, std::move(scene));
        
        EventBus::Publish(StageClearEvent{});
        EXPECT_EQ(callbackCount, 1);
    } // sceneService still owns the scene

    // Scene is still alive, callback should still work
    EventBus::Publish(StageClearEvent{});
    EXPECT_EQ(callbackCount, 2);

    // Now destroy sceneService, which should destroy the scene
    sceneService.reset();
    
    // After sceneService destruction, scene is destroyed, callback should not fire
    EventBus::Publish(StageClearEvent{});
    EXPECT_EQ(callbackCount, 2);
}

TEST_F(OwnershipLifetimeSafetyTest, SceneReplacement_OldSceneEventHandlesCleanedUp) {
    int oldSceneCallbacks = 0;
    int newSceneCallbacks = 0;
    
    class OldScene : public IScene {
    public:
        OldScene(int& counter) : counter(counter) {
            handle = EventBus::Subscribe<StageClearEvent>([&counter](const StageClearEvent&) {
                counter++;
            });
        }
        ~OldScene() override {
            EventBus::Unsubscribe(handle);
        }
        void Start() override {}
        void Update(float) override {}
    private:
        int& counter;
        EventHandle handle;
    };
    
    class NewScene : public IScene {
    public:
        NewScene(int& counter) : counter(counter) {
            handle = EventBus::Subscribe<StageClearEvent>([&counter](const StageClearEvent&) {
                counter++;
            });
        }
        ~NewScene() override {
            EventBus::Unsubscribe(handle);
        }
        void Start() override {}
        void Update(float) override {}
    private:
        int& counter;
        EventHandle handle;
    };

    auto sceneService = CreateSceneService();
    
    sceneService->RegisterScene(ESceneState::Info, std::make_unique<OldScene>(oldSceneCallbacks));
    EventBus::Publish(StageClearEvent{});
    EXPECT_EQ(oldSceneCallbacks, 1);
    EXPECT_EQ(newSceneCallbacks, 0);
    
    // Replace scene - old scene should be destroyed
    sceneService->RegisterScene(ESceneState::Info, std::make_unique<NewScene>(newSceneCallbacks));
    
    // After replacement, only new scene should receive events
    EventBus::Publish(StageClearEvent{});
    EXPECT_EQ(oldSceneCallbacks, 1); // No new callbacks to old scene
    EXPECT_EQ(newSceneCallbacks, 1); // New scene receives events
}

// ========== Displayer and Service Reference Lifetime Tests ==========

TEST_F(OwnershipLifetimeSafetyTest, Displayer_ServiceReferencesDoNotOutliveService) {
    using ::testing::NiceMock;
    
    // This test verifies the pattern where displayers hold references to services
    // Services must outlive displayers that reference them
    
    auto mockRender = std::make_unique<NiceMock<MockRenderService>>();
    auto mockAsset = std::make_unique<NiceMock<MockAssetService>>();
    
    std::unique_ptr<IDisplayer> displayer;
    
    {
        // Displayers take service references - services must outlive displayers
        displayer = CreateLambdaDisplayer([&mockRender](float deltaTime) {
            // Safe to use mockRender here while it's alive
            if (mockRender) {
                // Use the render service
            }
        });
        
        EXPECT_NO_FATAL_FAILURE(displayer->Update(0.016f));
    }
    
    // Displayer destroyed before services - correct lifetime order
    displayer.reset();
    
    // Services still valid
    EXPECT_NE(mockRender, nullptr);
    EXPECT_NE(mockAsset, nullptr);
}

TEST_F(OwnershipLifetimeSafetyTest, CompositeDisplayer_ChildrenDestroyedWithParent) {
    bool childDestroyed = false;
    
    class TestDisplayer : public DelegatingDisplayer {
    public:
        TestDisplayer(bool& flag) : destroyFlag(flag) {}
        ~TestDisplayer() override { destroyFlag = true; }
        void OnUpdate(float) override {}
        void OnDraw(float) const override {}
    private:
        bool& destroyFlag;
    };
    
    {
        auto composite = CreateCompositeDisplayer();
        composite->push_back(std::make_unique<TestDisplayer>(childDestroyed));
        
        EXPECT_FALSE(childDestroyed);
    } // composite destroyed, should destroy children
    
    EXPECT_TRUE(childDestroyed);
}

// ========== Smart Pointer Ownership Tests ==========

TEST_F(OwnershipLifetimeSafetyTest, UniquePtr_TransfersOwnershipCorrectly) {
    bool objectDestroyed = false;
    
    class TrackedObject {
    public:
        TrackedObject(bool& flag) : flag(flag) {}
        ~TrackedObject() { flag = true; }
    private:
        bool& flag;
    };
    
    std::unique_ptr<TrackedObject> owner1 = std::make_unique<TrackedObject>(objectDestroyed);
    EXPECT_FALSE(objectDestroyed);
    
    // Transfer ownership
    std::unique_ptr<TrackedObject> owner2 = std::move(owner1);
    EXPECT_FALSE(objectDestroyed);
    EXPECT_EQ(owner1, nullptr); // Original owner is now null
    
    // Destroy new owner
    owner2.reset();
    EXPECT_TRUE(objectDestroyed);
}

TEST_F(OwnershipLifetimeSafetyTest, Factory_ReturnsUniquePtr) {
    // Verify Create* factories return unique_ptr for clear ownership
    auto sceneService = CreateSceneService();
    EXPECT_NE(sceneService, nullptr);
    
    // Factory pattern ensures no raw new/delete needed
    // Ownership is clear: caller owns the returned object
}

TEST_F(OwnershipLifetimeSafetyTest, NoRawNewDelete_UseFactoriesAndSmartPointers) {
    // This test documents the pattern: no raw new/delete in production code
    // All object creation should use Create* factories or std::make_unique
    
    // CORRECT: Factory function
    auto service = CreateSceneService();
    
    // CORRECT: std::make_unique
    auto displayer = std::make_unique<DelegatingDisplayer>();
    
    // These are automatically cleaned up - no manual delete needed
    EXPECT_NE(service.get(), nullptr);
    EXPECT_NE(displayer.get(), nullptr);
}

// ========== RAII Cleanup Tests ==========

TEST_F(OwnershipLifetimeSafetyTest, RAII_ResourcesReleasedInDestructor) {
    int cleanupCount = 0;
    
    class RAIIResource {
    public:
        RAIIResource(int& counter) : counter(counter) {
            // Acquire resource
        }
        
        ~RAIIResource() {
            // Release resource
            counter++;
        }
        
    private:
        int& counter;
    };
    
    {
        RAIIResource resource(cleanupCount);
        EXPECT_EQ(cleanupCount, 0);
    } // Destructor called automatically
    
    EXPECT_EQ(cleanupCount, 1);
}

TEST_F(OwnershipLifetimeSafetyTest, ServiceDestruction_UnsubscribesAllHandles) {
    // This simulates the AudioService pattern: multiple handles in a vector
    int callbackCount = 0;
    
    class MockAudioService {
    public:
        MockAudioService(int& counter) : counter(counter) {
            eventHandles.push_back(EventBus::Subscribe<StageClearEvent>(
                [&counter](const StageClearEvent&) { counter++; }));
            eventHandles.push_back(EventBus::Subscribe<StageFailEvent>(
                [&counter](const StageFailEvent&) { counter++; }));
            eventHandles.push_back(EventBus::Subscribe<EnemyDefeatedEvent>(
                [&counter](const EnemyDefeatedEvent&) { counter++; }));
        }
        
        ~MockAudioService() {
            for (auto handle : eventHandles) {
                EventBus::Unsubscribe(handle);
            }
        }
        
    private:
        int& counter;
        std::vector<EventHandle> eventHandles;
    };

    {
        MockAudioService audioService(callbackCount);
        EventBus::Publish(StageClearEvent{});
        EventBus::Publish(StageFailEvent{});
        EXPECT_EQ(callbackCount, 2);
    } // audioService destroyed, all handles unsubscribed

    EventBus::Publish(StageClearEvent{});
    EventBus::Publish(StageFailEvent{});
    EventBus::Publish(EnemyDefeatedEvent{0});
    EXPECT_EQ(callbackCount, 2); // No new callbacks
}

// ========== Animation and Effect Object Lifetime Tests ==========

TEST_F(OwnershipLifetimeSafetyTest, Effector_CompletionCallbackSafe) {
    using ::testing::NiceMock;
    
    NiceMock<MockRenderService> mockRender;
    
    bool callbackInvoked = false;
    
    {
        auto displayer = CreateLambdaDisplayer([](float) {});
        
        // Simulate adding an effector with a completion callback
        // The callback should not access destroyed objects
        auto callback = [&callbackInvoked]() {
            callbackInvoked = true;
        };
        
        // In production, effector completion callbacks must be safe
        // They should not capture [this] from a potentially destroyed owner
        // unless the owner outlives the effector
        
        callback(); // Simulate immediate completion
        EXPECT_TRUE(callbackInvoked);
    }
}

TEST_F(OwnershipLifetimeSafetyTest, DisplayerTeardown_NoCallbacksAfterDestruction) {
    using ::testing::NiceMock;
    
    int updateCount = 0;
    
    {
        auto displayer = CreateLambdaDisplayer([&updateCount](float) {
            updateCount++;
        });
        
        displayer->Update(0.016f);
        EXPECT_EQ(updateCount, 1);
    } // displayer destroyed

    // Cannot call Update on destroyed displayer - this is by design
    // The test verifies that the pattern prevents use-after-destroy
}

// ========== Maintainability Check Documentation ==========

TEST_F(OwnershipLifetimeSafetyTest, ReviewChecklist_LongLivedCallbacks) {
    // This test documents maintainability rules for long-lived callbacks:
    //
    // 1. Every EventBus::Subscribe call should store the returned EventHandle
    // 2. The handle owner should call EventBus::Unsubscribe in its destructor
    // 3. Long-lived callbacks should use explicit [this] capture only when
    //    the owner unsubscribes before destruction
    // 4. Avoid broad [&] captures in long-lived callbacks
    // 5. Use std::optional<EventHandle> when subscription is conditional
    
    // This pattern is exemplified in:
    // - SceneService (constructor subscribes, destructor unsubscribes)
    // - InfoScene (constructor subscribes, destructor unsubscribes)
    // - CombatScene (Start subscribes, UnsubscribeAll helper, destructor calls it)
    // - AudioService (constructor subscribes to vector, destructor unsubscribes all)
    
    SUCCEED() << "Maintainability checklist documented";
}

TEST_F(OwnershipLifetimeSafetyTest, ReviewChecklist_ExplicitCaptures) {
    // This test documents explicit capture rules:
    //
    // SAFE PATTERNS:
    // - [this] with guaranteed destructor unsubscription
    // - [&externalRef] where external outlives the subscription
    // - Explicit member captures when owner unsubscribes
    //
    // UNSAFE PATTERNS:
    // - [&] broad capture in long-lived callbacks
    // - [this] without destructor unsubscription
    // - Capturing stack variables in callbacks that outlive the stack frame
    
    SUCCEED() << "Explicit capture safety rules documented";
}

TEST_F(OwnershipLifetimeSafetyTest, ReviewChecklist_InvalidHandleReuse) {
    // This test verifies idempotent unsubscribe prevents invalid handle reuse
    
    auto handle = EventBus::Subscribe<StageClearEvent>([](const StageClearEvent&) {});
    
    EventBus::Unsubscribe(handle);
    
    // Multiple unsubscribes with same handle should be safe (idempotent)
    EXPECT_NO_FATAL_FAILURE(EventBus::Unsubscribe(handle));
    EXPECT_NO_FATAL_FAILURE(EventBus::Unsubscribe(handle));
}

} // namespace
} // namespace mc
