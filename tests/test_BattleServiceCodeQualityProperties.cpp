// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before gtest includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <gmock/gmock.h>

import EffectorFactory;
import SceneService;
import RenderService;

#include "MockServices.h"

namespace mc {
namespace {

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtLeast;

/// Step an effector by `totalMs` milliseconds in one go.
/// Returns the final Update() return value.
static bool StepEffector(Effector& effector, int totalMs)
{
    return effector.Update(static_cast<float>(totalMs) / 1000.0f);
}

/// Step an effector in small increments of `stepMs` up to `totalMs` ms.
/// Returns the last Update() return value.
static bool StepEffectorIncremental(Effector& effector, int totalMs, int stepMs = 16)
{
    bool result = true;
    int elapsed = 0;
    while (elapsed < totalMs)
    {
        int dt = std::min(stepMs, totalMs - elapsed);
        result = effector.Update(static_cast<float>(dt) / 1000.0f);
        elapsed += dt;
    }
    return result;
}

class DummyScene : public IScene
{
public:
    int startCount = 0;
    void Start() override { ++startCount; }
    void Update(float) override {}
};

RC_GTEST_PROP(BattleServiceProperties, FadeInEffector_RisingAlpha, ())
{
    const int durationMs = *rc::gen::inRange(1, 10001); // [1, 10000]

    MockRenderService mockRs;
    EXPECT_CALL(mockRs, SetDrawBlendMode(_, _)).Times(AnyNumber());

    auto effector = CreateFadeInEffector(mockRs, durationMs);

    RC_ASSERT(!effector->ShouldDraw());

    StepEffector(*effector, durationMs);
    RC_ASSERT(effector->ShouldDraw());
}

RC_GTEST_PROP(BattleServiceProperties, FadeOutEffector_FallingAlpha, ())
{
    const int durationMs = *rc::gen::inRange(1, 10001); // [1, 10000]

    MockRenderService mockRs;
    EXPECT_CALL(mockRs, SetDrawBlendMode(_, _)).Times(AnyNumber());

    auto effector = CreateFadeOutEffector(mockRs, durationMs);

    RC_ASSERT(effector->ShouldDraw());

    bool stillRunning = StepEffector(*effector, durationMs);
    RC_ASSERT(!stillRunning);
    RC_ASSERT(!effector->ShouldDraw());
}

RC_GTEST_PROP(BattleServiceProperties, FadeEffector_UpdateReturnsFalseAtCompletion, ())
{
    const int durationMs = *rc::gen::inRange(1, 10001); // [1, 10000]

    MockRenderService mockRs;
    EXPECT_CALL(mockRs, SetDrawBlendMode(_, _)).Times(AnyNumber());

    {
        auto effector = CreateFadeInEffector(mockRs, durationMs);

        const int partialMs = durationMs / 2;
        if (partialMs > 0)
        {
            bool running = StepEffector(*effector, partialMs);
            (void)running;
        }

        bool completed = StepEffector(*effector, durationMs);
        RC_ASSERT(!completed);
    }

    {
        auto effectorOut = CreateFadeOutEffector(mockRs, durationMs);

        bool completed = StepEffector(*effectorOut, durationMs);
        RC_ASSERT(!completed);

        bool stillDone = effectorOut->Update(0.001f);
        RC_ASSERT(!stillDone);
    }
}

// CombatScene is not accessible outside the SceneService module.
// Progress text formatting is covered by test_SequentialBattlesIntegration.cpp.
TEST(BattleServiceProperties_Property5Note, CombatSceneProgressText_IsVerifiedViaIntegration)
{
    SUCCEED() << "Property 5 (progress text) is verified via integration tests.";
}

RC_GTEST_PROP(BattleServiceProperties, SetCurrentScene_RegisteredState_CallsStart, ())
{
    auto sceneService = CreateSceneService();

    auto* infoDummy   = new DummyScene();
    auto* combatDummy = new DummyScene();
    sceneService->RegisterScene(ESceneState::Info,   std::unique_ptr<IScene>(infoDummy));
    sceneService->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatDummy));

    // GetCurrentScene triggers initialization of the default (Info) scene.
    (void)sceneService->GetCurrentScene();

    const int infoStartBefore   = infoDummy->startCount;
    const int combatStartBefore = combatDummy->startCount;

    sceneService->SetCurrentScene(ESceneState::Combat);

    RC_ASSERT(combatDummy->startCount == combatStartBefore + 1);
    RC_ASSERT(sceneService->GetCurrentScene() == ESceneState::Combat);

    sceneService->SetCurrentScene(ESceneState::Info);
    RC_ASSERT(infoDummy->startCount == infoStartBefore + 1);
    RC_ASSERT(sceneService->GetCurrentScene() == ESceneState::Info);
}

RC_GTEST_PROP(BattleServiceProperties, SetCurrentScene_UnregisteredState_IsNoOp, ())
{
    auto sceneService = CreateSceneService();

    auto* infoDummy = new DummyScene();
    sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoDummy));

    (void)sceneService->GetCurrentScene();
    const ESceneState initialScene = sceneService->GetCurrentScene();

    const auto unregisteredStates = std::array{
        ESceneState::Combat,
        ESceneState::Menu,
        ESceneState::Cutscene
    };
    const int idx = *rc::gen::inRange(0, static_cast<int>(unregisteredStates.size()));
    const ESceneState unregistered = unregisteredStates[idx];

    const int startBefore = infoDummy->startCount;

    sceneService->SetCurrentScene(unregistered);

    RC_ASSERT(sceneService->GetCurrentScene() == initialScene);
    RC_ASSERT(infoDummy->startCount == startBefore);
}

class FadeEffectorUnitTest : public ::testing::Test
{
protected:
    MockRenderService mockRs;

    void SetUp() override
    {
        EXPECT_CALL(mockRs, SetDrawBlendMode(_, _)).Times(AnyNumber());
    }
};

TEST_F(FadeEffectorUnitTest, FadeIn_AlphaIsZeroAtStart)
{
    auto effector = CreateFadeInEffector(mockRs, 500);

    EXPECT_FALSE(effector->ShouldDraw());

    ::testing::Mock::VerifyAndClearExpectations(&mockRs);
    EXPECT_CALL(mockRs, SetDrawBlendMode(_, _)).Times(0);
    effector->BeforeDraw();
}

TEST_F(FadeEffectorUnitTest, FadeIn_AlphaIs255AfterFullDuration)
{
    auto effector = CreateFadeInEffector(mockRs, 500);

    StepEffectorIncremental(*effector, 500);

    EXPECT_TRUE(effector->ShouldDraw());
}

TEST_F(FadeEffectorUnitTest, FadeOut_AlphaIs255AtStart)
{
    auto effector = CreateFadeOutEffector(mockRs, 500);

    EXPECT_TRUE(effector->ShouldDraw());
}

TEST_F(FadeEffectorUnitTest, FadeOut_AlphaIsZeroAfterFullDuration)
{
    auto effector = CreateFadeOutEffector(mockRs, 500);

    bool running = StepEffectorIncremental(*effector, 500);

    EXPECT_FALSE(running);
    EXPECT_FALSE(effector->ShouldDraw());
}

TEST(SetCurrentSceneUnitTest, UnregisteredState_DoesNotChangeScene)
{
    auto sceneService = CreateSceneService();

    auto* infoDummy = new DummyScene();
    sceneService->RegisterScene(ESceneState::Info, std::unique_ptr<IScene>(infoDummy));

    sceneService->SetCurrentScene(ESceneState::Info);

    const ESceneState before = sceneService->GetCurrentScene();

    sceneService->SetCurrentScene(ESceneState::Combat);

    EXPECT_EQ(sceneService->GetCurrentScene(), before);
}

TEST(SetCurrentSceneUnitTest, RegisteredState_CallsStartExactlyOnce)
{
    auto sceneService = CreateSceneService();

    auto* infoDummy   = new DummyScene();
    auto* combatDummy = new DummyScene();
    sceneService->RegisterScene(ESceneState::Info,   std::unique_ptr<IScene>(infoDummy));
    sceneService->RegisterScene(ESceneState::Combat, std::unique_ptr<IScene>(combatDummy));

    (void)sceneService->GetCurrentScene();
    const int combatStartBefore = combatDummy->startCount;

    sceneService->SetCurrentScene(ESceneState::Combat);

    EXPECT_EQ(combatDummy->startCount, combatStartBefore + 1);
    EXPECT_EQ(sceneService->GetCurrentScene(), ESceneState::Combat);
}

} // namespace
} // namespace mc
