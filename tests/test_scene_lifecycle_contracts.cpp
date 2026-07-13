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

import SceneService;
import EventBus;
import InputService;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

namespace mc {
namespace {

// ============================================================
// Test Fixture and Helper Types
// ============================================================

/// @brief Instrumented scene that tracks lifecycle calls
class InstrumentedScene : public IScene {
public:
    int startCount = 0;
    int updateCount = 0;
    bool wasDestroyed = false;

    void Start() override { 
        startCount++; 
    }

    void Update(float deltaTime) override { 
        updateCount++; 
    }

    ~InstrumentedScene() override {
        wasDestroyed = true;
    }
};

class SceneLifecycleContractTest : public ::testing::Test {
protected:
    void SetUp() override {
        EventBus::Clear();
    }

    void TearDown() override {
        EventBus::Clear();
    }

    std::unique_ptr<ISceneService> CreateTestSceneService() {
        auto mockRender = std::make_unique<NiceMock<MockRenderService>>();
        auto mockInput = std::make_unique<NiceMock<MockInputService>>();
        auto mockOS = std::make_unique<NiceMock<MockOSService>>();

        renderPtr = mockRender.get();
        inputPtr = mockInput.get();
        osPtr = mockOS.get();

        // Store mocks for lifetime management
        renderMock = std::move(mockRender);
        inputMock = std::move(mockInput);
        osMock = std::move(mockOS);

        return CreateSceneService(renderPtr, inputPtr, osPtr);
    }

    MockRenderService* renderPtr = nullptr;
    MockInputService* inputPtr = nullptr;
    MockOSService* osPtr = nullptr;

private:
    std::unique_ptr<MockRenderService> renderMock;
    std::unique_ptr<MockInputService> inputMock;
    std::unique_ptr<MockOSService> osMock;
};

// ============================================================
// CONTRACT 1: Scene Start() is called exactly once per activation
// ============================================================

TEST_F(SceneLifecycleContractTest, Contract_StartCalledOnceOnSceneActivation) {
    auto service = CreateTestSceneService();
    
    auto* infoScene = new InstrumentedScene();
    service->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoScene));

    // First Update triggers lazy initialization and calls Start() once
    service->Update(0.016f);
    EXPECT_EQ(infoScene->startCount, 1) << "Start should be called once on first activation";

    // Subsequent updates should NOT call Start again
    service->Update(0.016f);
    service->Update(0.016f);
    EXPECT_EQ(infoScene->startCount, 1) << "Start should not be called again on subsequent updates";
}

TEST_F(SceneLifecycleContractTest, Contract_StartCalledOncePerPushScene) {
    auto service = CreateTestSceneService();
    
    auto* infoScene = new InstrumentedScene();
    auto* combatScene = new InstrumentedScene();
    service->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoScene));
    service->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatScene));

    // Initialize
    service->Update(0.016f);
    EXPECT_EQ(infoScene->startCount, 1);

    // Push combat scene
    service->PushScene(ESceneState::Combat);
    EXPECT_EQ(combatScene->startCount, 1) << "Start should be called once when scene is pushed";

    // Multiple updates don't call Start again
    service->Update(0.016f);
    service->Update(0.016f);
    EXPECT_EQ(combatScene->startCount, 1) << "Start should not be called on updates";
}

TEST_F(SceneLifecycleContractTest, Contract_StartCalledOncePerTransition) {
    auto service = CreateTestSceneService();
    
    auto* infoScene = new InstrumentedScene();
    auto* cutsceneScene = new InstrumentedScene();
    service->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoScene));
    service->RegisterScene(ESceneState::Cutscene, std::unique_ptr<IScene>(cutsceneScene));

    // Initialize to Info
    service->Update(0.016f);
    int infoStartCountBefore = infoScene->startCount;

    // Transition to Cutscene via event
    EventBus::Publish(StageStartedEvent{});
    
    // Wait for fade transition to complete (ApplyPendingTransition happens in callback)
    // Since we have no real render service, the transition applies immediately
    service->Update(0.016f);

    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Cutscene);
    EXPECT_EQ(cutsceneScene->startCount, 1) << "Start should be called once on transition";
}

// ============================================================
// CONTRACT 2: Only the active (top of stack) scene receives Update
// ============================================================

TEST_F(SceneLifecycleContractTest, Contract_OnlyActiveSceneUpdates) {
    auto service = CreateTestSceneService();
    
    auto* infoScene = new InstrumentedScene();
    auto* combatScene = new InstrumentedScene();
    service->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoScene));
    service->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatScene));

    // Initialize Info
    service->Update(0.016f);
    EXPECT_EQ(infoScene->updateCount, 1);
    EXPECT_EQ(combatScene->updateCount, 0);

    // Push Combat
    service->PushScene(ESceneState::Combat);
    int infoUpdateCount = infoScene->updateCount;

    // Update should only affect Combat (top of stack)
    service->Update(0.016f);
    service->Update(0.016f);
    
    EXPECT_EQ(infoScene->updateCount, infoUpdateCount) << "Inactive scene should not receive updates";
    EXPECT_EQ(combatScene->updateCount, 2) << "Active scene should receive updates";
}

TEST_F(SceneLifecycleContractTest, Contract_AfterPopActiveSceneUpdates) {
    auto service = CreateTestSceneService();
    
    auto* infoScene = new InstrumentedScene();
    auto* menuScene = new InstrumentedScene();
    service->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoScene));
    service->RegisterScene(ESceneState::Menu, std::unique_ptr<IScene>(menuScene));

    // Initialize and push menu
    service->Update(0.016f);
    service->PushScene(ESceneState::Menu);
    
    int infoUpdateBefore = infoScene->updateCount;
    service->Update(0.016f);
    EXPECT_EQ(menuScene->updateCount, 1);
    EXPECT_EQ(infoScene->updateCount, infoUpdateBefore) << "Inactive scene not updated";

    // Pop menu
    service->PopScene();
    
    // Now Info should update again
    service->Update(0.016f);
    EXPECT_EQ(infoScene->updateCount, infoUpdateBefore + 1) << "After pop, previous scene updates";
}

// ============================================================
// CONTRACT 3: Scene stack remains valid (never empty after init)
// ============================================================

TEST_F(SceneLifecycleContractTest, Contract_StackNeverEmptyAfterInitialization) {
    auto service = CreateTestSceneService();
    
    service->RegisterScene(ESceneState::Info, std::make_unique<InstrumentedScene>());
    
    // After initialization, current scene should be Info
    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
    
    // Attempting to pop the last scene should not crash or empty the stack
    service->PopScene();
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info) << "Stack should preserve at least one scene";
}

TEST_F(SceneLifecycleContractTest, Contract_StackValidAfterMultiplePushPop) {
    auto service = CreateTestSceneService();
    
    service->RegisterScene(ESceneState::Info, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Combat, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Menu, std::make_unique<InstrumentedScene>());

    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);

    // Push Combat
    service->PushScene(ESceneState::Combat);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Combat);

    // Push Menu
    service->PushScene(ESceneState::Menu);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Menu);

    // Pop back to Combat
    service->PopScene();
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Combat);

    // Pop back to Info
    service->PopScene();
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);

    // Stack should remain valid
    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
}

// ============================================================
// CONTRACT 4: Event-driven routing preserves deterministic flow
// ============================================================

TEST_F(SceneLifecycleContractTest, Contract_InfoToCutsceneOnStageStarted) {
    auto service = CreateTestSceneService();
    
    service->RegisterScene(ESceneState::Info, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Cutscene, std::make_unique<InstrumentedScene>());
    
    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);

    EventBus::Publish(StageStartedEvent{});
    service->Update(0.016f); // Process transition
    
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Cutscene);
}

TEST_F(SceneLifecycleContractTest, Contract_CutsceneToCombatOnCutsceneFinished) {
    auto service = CreateTestSceneService();
    
    service->RegisterScene(ESceneState::Cutscene, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Combat, std::make_unique<InstrumentedScene>());
    
    service->SetCurrentScene(ESceneState::Cutscene);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Cutscene);

    EventBus::Publish(CutsceneFinishedEvent{});
    service->Update(0.016f); // Process transition
    
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Combat);
}

TEST_F(SceneLifecycleContractTest, Contract_CombatToInfoOnStageClear) {
    auto service = CreateTestSceneService();
    
    service->RegisterScene(ESceneState::Combat, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Info, std::make_unique<InstrumentedScene>());
    
    service->SetCurrentScene(ESceneState::Combat);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Combat);

    EventBus::Publish(StageClearEvent{});
    service->Update(0.016f); // Process transition
    
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
}

TEST_F(SceneLifecycleContractTest, Contract_CombatToInfoOnStageFail) {
    auto service = CreateTestSceneService();
    
    service->RegisterScene(ESceneState::Combat, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Info, std::make_unique<InstrumentedScene>());
    
    service->SetCurrentScene(ESceneState::Combat);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Combat);

    EventBus::Publish(StageFailEvent{});
    service->Update(0.016f); // Process transition
    
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
}

TEST_F(SceneLifecycleContractTest, Contract_FullGameFlowSequence) {
    auto service = CreateTestSceneService();
    
    auto* infoScene = new InstrumentedScene();
    auto* cutsceneScene = new InstrumentedScene();
    auto* combatScene = new InstrumentedScene();
    
    service->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoScene));
    service->RegisterScene(ESceneState::Cutscene, std::unique_ptr<IScene>(cutsceneScene));
    service->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatScene));

    // Start at Info
    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
    EXPECT_EQ(infoScene->startCount, 1);

    // Info -> Cutscene (stage started)
    EventBus::Publish(StageStartedEvent{});
    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Cutscene);
    EXPECT_EQ(cutsceneScene->startCount, 1);

    // Cutscene -> Combat (cutscene finished)
    EventBus::Publish(CutsceneFinishedEvent{});
    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Combat);
    EXPECT_EQ(combatScene->startCount, 1);

    // Combat -> Cutscene (enemy defeated)
    EventBus::Publish(EnemyDefeatedEvent{0});
    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Cutscene);
    EXPECT_EQ(cutsceneScene->startCount, 2) << "Cutscene Start called again on re-entry";

    // Cutscene -> Combat (another fight)
    EventBus::Publish(CutsceneFinishedEvent{});
    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Combat);
    EXPECT_EQ(combatScene->startCount, 2) << "Combat Start called again on re-entry";

    // Combat -> Info (stage clear)
    EventBus::Publish(StageClearEvent{});
    service->Update(0.016f);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
    EXPECT_EQ(infoScene->startCount, 2) << "Info Start called again on return";
}

// ============================================================
// CONTRACT 5: Menu overlay preserves input context balance
// ============================================================

TEST_F(SceneLifecycleContractTest, Contract_MenuPushPopRestoresInputContext) {
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockInputService> mockInput;
    NiceMock<MockOSService> mockOS;

    auto service = CreateSceneService(&mockRender, &mockInput, &mockOS);
    
    service->RegisterScene(ESceneState::Info, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Menu, std::make_unique<InstrumentedScene>());

    // Initialize
    service->Update(0.016f);

    // Push Menu - no specific context push expected here
    service->PushScene(ESceneState::Menu);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Menu);

    // Pop Menu - should restore InGame context
    EXPECT_CALL(mockInput, ClearAndSetContext(InputContext::InGame)).Times(1);
    service->PopScene();
    
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
}

// ============================================================
// CONTRACT 6: SetCurrentScene clears stack and starts fresh
// ============================================================

TEST_F(SceneLifecycleContractTest, Contract_SetCurrentSceneClearsStack) {
    NiceMock<MockRenderService> mockRender;
    NiceMock<MockInputService> mockInput;
    NiceMock<MockOSService> mockOS;

    auto service = CreateSceneService(&mockRender, &mockInput, &mockOS);
    
    auto* infoScene = new InstrumentedScene();
    auto* combatScene = new InstrumentedScene();
    
    service->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoScene));
    service->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatScene));

    // Build a stack: Info -> Combat -> Menu
    service->Update(0.016f);
    service->PushScene(ESceneState::Combat);

    // SetCurrentScene should clear stack and set single scene
    EXPECT_CALL(mockInput, ClearAndSetContext(InputContext::InGame)).Times(1);
    service->SetCurrentScene(ESceneState::Info);
    
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
    EXPECT_EQ(infoScene->startCount, 2) << "Scene should be started fresh";

    // Pop should not go anywhere (stack has only one element)
    service->PopScene();
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
}

// ============================================================
// CONTRACT 7: Pending transitions are deterministic
// ============================================================

TEST_F(SceneLifecycleContractTest, Contract_PendingTransitionReplacesStack) {
    auto service = CreateTestSceneService();
    
    service->RegisterScene(ESceneState::Info, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Combat, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Menu, std::make_unique<InstrumentedScene>());

    // Initialize and build a multi-level stack
    service->Update(0.016f);
    service->PushScene(ESceneState::Combat);
    service->PushScene(ESceneState::Menu);
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Menu);

    // Trigger a transition event - should replace entire stack
    EventBus::Publish(StageClearEvent{});
    service->Update(0.016f);
    
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
    
    // Pop should not reveal Combat or Menu (they were cleared)
    service->PopScene();
    EXPECT_EQ(service->GetCurrentScene(), ESceneState::Info);
}

// ============================================================
// PROPERTY-BASED TESTS: Generated scene command sequences
// ============================================================

enum class SceneCommand {
    PushInfo,
    PushCombat,
    PushMenu,
    PushCutscene,
    Pop,
    PublishStageStarted,
    PublishCutsceneFinished,
    PublishEnemyDefeated,
    PublishStageClear,
    PublishStageFail,
    Update
};

std::string SceneCommandToString(SceneCommand cmd) {
    switch (cmd) {
        case SceneCommand::PushInfo: return "PushInfo";
        case SceneCommand::PushCombat: return "PushCombat";
        case SceneCommand::PushMenu: return "PushMenu";
        case SceneCommand::PushCutscene: return "PushCutscene";
        case SceneCommand::Pop: return "Pop";
        case SceneCommand::PublishStageStarted: return "PublishStageStarted";
        case SceneCommand::PublishCutsceneFinished: return "PublishCutsceneFinished";
        case SceneCommand::PublishEnemyDefeated: return "PublishEnemyDefeated";
        case SceneCommand::PublishStageClear: return "PublishStageClear";
        case SceneCommand::PublishStageFail: return "PublishStageFail";
        case SceneCommand::Update: return "Update";
        default: return "Unknown";
    }
}

void show(SceneCommand cmd, std::ostream& os) {
    os << SceneCommandToString(cmd);
}

} // anonymous namespace
} // namespace mc

namespace rc {
template<>
struct Arbitrary<mc::SceneCommand> {
    static Gen<mc::SceneCommand> arbitrary() {
        return gen::element(
            mc::SceneCommand::PushInfo,
            mc::SceneCommand::PushCombat,
            mc::SceneCommand::PushMenu,
            mc::SceneCommand::PushCutscene,
            mc::SceneCommand::Pop,
            mc::SceneCommand::PublishStageStarted,
            mc::SceneCommand::PublishCutsceneFinished,
            mc::SceneCommand::PublishEnemyDefeated,
            mc::SceneCommand::PublishStageClear,
            mc::SceneCommand::PublishStageFail,
            mc::SceneCommand::Update
        );
    }
};
} // namespace rc

namespace mc {
namespace {

// Helper to execute a command sequence
void ExecuteSceneCommand(ISceneService& service, SceneCommand cmd) {
    switch (cmd) {
        case SceneCommand::PushInfo:
            service.PushScene(ESceneState::Info);
            break;
        case SceneCommand::PushCombat:
            service.PushScene(ESceneState::Combat);
            break;
        case SceneCommand::PushMenu:
            service.PushScene(ESceneState::Menu);
            break;
        case SceneCommand::PushCutscene:
            service.PushScene(ESceneState::Cutscene);
            break;
        case SceneCommand::Pop:
            service.PopScene();
            break;
        case SceneCommand::PublishStageStarted:
            EventBus::Publish(StageStartedEvent{});
            break;
        case SceneCommand::PublishCutsceneFinished:
            EventBus::Publish(CutsceneFinishedEvent{});
            break;
        case SceneCommand::PublishEnemyDefeated:
            EventBus::Publish(EnemyDefeatedEvent{0});
            break;
        case SceneCommand::PublishStageClear:
            EventBus::Publish(StageClearEvent{});
            break;
        case SceneCommand::PublishStageFail:
            EventBus::Publish(StageFailEvent{});
            break;
        case SceneCommand::Update:
            service.Update(0.016f);
            break;
    }
}

RC_GTEST_PROP(SceneLifecycleProperties, StackAlwaysValidAfterCommandSequence,
              (const std::vector<SceneCommand>& commands)) {
    EventBus::Clear();

    NiceMock<MockRenderService> mockRender;
    NiceMock<MockInputService> mockInput;
    NiceMock<MockOSService> mockOS;

    auto service = CreateSceneService(&mockRender, &mockInput, &mockOS);
    
    service->RegisterScene(ESceneState::Info, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Combat, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Menu, std::make_unique<InstrumentedScene>());
    service->RegisterScene(ESceneState::Cutscene, std::make_unique<InstrumentedScene>());

    // Initialize
    service->Update(0.016f);
    ESceneState initialScene = service->GetCurrentScene();
    RC_ASSERT(initialScene == ESceneState::Info);

    // Execute command sequence
    for (const auto& cmd : commands) {
        ExecuteSceneCommand(*service, cmd);
        
        // INVARIANT: GetCurrentScene should always return a valid scene
        ESceneState currentScene = service->GetCurrentScene();
        RC_ASSERT(currentScene == ESceneState::Info || 
                  currentScene == ESceneState::Combat ||
                  currentScene == ESceneState::Menu ||
                  currentScene == ESceneState::Cutscene);
        
        // Should never crash on Update
        service->Update(0.016f);
    }

    EventBus::Clear();
}

RC_GTEST_PROP(SceneLifecycleProperties, CurrentSceneDeterministicForSameSequence,
              (const std::vector<SceneCommand>& commands)) {
    EventBus::Clear();

    // Run the same sequence twice and verify same outcome
    auto runSequence = [&]() -> ESceneState {
        NiceMock<MockRenderService> mockRender;
        NiceMock<MockInputService> mockInput;
        NiceMock<MockOSService> mockOS;

        auto service = CreateSceneService(&mockRender, &mockInput, &mockOS);
        
        service->RegisterScene(ESceneState::Info, std::make_unique<InstrumentedScene>());
        service->RegisterScene(ESceneState::Combat, std::make_unique<InstrumentedScene>());
        service->RegisterScene(ESceneState::Menu, std::make_unique<InstrumentedScene>());
        service->RegisterScene(ESceneState::Cutscene, std::make_unique<InstrumentedScene>());

        service->Update(0.016f);

        for (const auto& cmd : commands) {
            ExecuteSceneCommand(*service, cmd);
        }

        return service->GetCurrentScene();
    };

    ESceneState result1 = runSequence();
    EventBus::Clear();
    ESceneState result2 = runSequence();

    RC_ASSERT(result1 == result2);

    EventBus::Clear();
}

RC_GTEST_PROP(SceneLifecycleProperties, StartCallCountNeverDecreases,
              (const std::vector<SceneCommand>& commands)) {
    EventBus::Clear();

    NiceMock<MockRenderService> mockRender;
    NiceMock<MockInputService> mockInput;
    NiceMock<MockOSService> mockOS;

    auto service = CreateSceneService(&mockRender, &mockInput, &mockOS);
    
    auto* infoScene = new InstrumentedScene();
    auto* combatScene = new InstrumentedScene();
    auto* menuScene = new InstrumentedScene();
    auto* cutsceneScene = new InstrumentedScene();
    
    service->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoScene));
    service->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatScene));
    service->RegisterScene(ESceneState::Menu, std::unique_ptr<IScene>(menuScene));
    service->RegisterScene(ESceneState::Cutscene, std::unique_ptr<IScene>(cutsceneScene));

    service->Update(0.016f);

    int prevInfoStart = infoScene->startCount;
    int prevCombatStart = combatScene->startCount;
    int prevMenuStart = menuScene->startCount;
    int prevCutsceneStart = cutsceneScene->startCount;

    for (const auto& cmd : commands) {
        ExecuteSceneCommand(*service, cmd);
        
        // Start counts should never decrease
        RC_ASSERT(infoScene->startCount >= prevInfoStart);
        RC_ASSERT(combatScene->startCount >= prevCombatStart);
        RC_ASSERT(menuScene->startCount >= prevMenuStart);
        RC_ASSERT(cutsceneScene->startCount >= prevCutsceneStart);
        
        prevInfoStart = infoScene->startCount;
        prevCombatStart = combatScene->startCount;
        prevMenuStart = menuScene->startCount;
        prevCutsceneStart = cutsceneScene->startCount;
    }

    EventBus::Clear();
}

// ============================================================
// Validates: Requirements 2.1, 2.7, 2.10, 2.11, 3.4
// ============================================================

} // anonymous namespace
} // namespace mc
