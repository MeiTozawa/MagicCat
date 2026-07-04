#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockServices.h"

import EffectorFactory;
import RenderService;

using ::testing::_;
using ::testing::NiceMock;

namespace mc {
namespace {

TEST(HitFlashEffectorTest, StartState_DoesNotAlterBrightBeforeDraw) {
    NiceMock<MockRenderService> mockRs;
    
    auto effector = CreateHitFlashEffector(mockRs, 0xFF0000, 100);
    
    EXPECT_TRUE(effector->ShouldDraw());
}

TEST(HitFlashEffectorTest, Update_ReturnsTrueDuringFlash_AndFalseAtEnd) {
    NiceMock<MockRenderService> mockRs;
    auto effector = CreateHitFlashEffector(mockRs, 0xFF0000, 100);
    
    bool running = effector->Update(0.050f);
    EXPECT_TRUE(running);
    
    // Step forward another 110ms (total 160ms, exceeds 150ms total duration) -> should be completed
    running = effector->Update(0.110f);
    EXPECT_FALSE(running);
}

TEST(HitFlashEffectorTest, DrawHooks_AppliesAndResetsBright) {
    MockRenderService mockRs;
    auto effector = CreateHitFlashEffector(mockRs, 0xFF0000, 100);
    
    EXPECT_CALL(mockRs, SetDrawBright(_, _, _)).Times(1);
    effector->BeforeDraw();
    
    EXPECT_CALL(mockRs, SetDrawBright(255, 255, 255)).Times(1);
    effector->AfterDraw();
}

} // namespace
} // namespace mc
