#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockServices.h"

import Displayer;
import RenderService;

using ::testing::_;
using ::testing::NiceMock;

namespace mc {
namespace {

TEST(CutsceneFocusDisplayerTest, InitialState_IsPlayingAndVisible) {
    NiceMock<MockRenderService> mockRs;
    Point<float> src{100.f, 200.f};
    Point<float> dst{300.f, 200.f};
    
    auto displayer = CreateCutsceneFocusDisplayer(mockRs, src, dst, 50.f, 50.f, 8, 2);
    
    EXPECT_TRUE(displayer->IsVisible());
    EXPECT_TRUE(displayer->IsPlaying());
}

TEST(CutsceneFocusDisplayerTest, Draw_CallsDrawRoundRectFrame_WithLerpedPosition) {
    MockRenderService mockRs;
    Point<float> src{100.f, 200.f};
    Point<float> dst{300.f, 200.f};
    float duration = 1.0f;
    
    auto displayer = CreateCutsceneFocusDisplayer(
        mockRs, src, dst, 50.f, 50.f, 8, 2, 0xFFFFFF, duration);
    
    // At t=0, focus should be centered at source (100, 200)
    // Frame rect: [100-50, 200-50] to [100+50, 200+50] -> [50, 150] to [150, 250]
    EXPECT_CALL(mockRs, DrawRoundRectFrame(50, 150, 150, 250, 8, 2, 0xFFFFFF))
        .Times(1);
    displayer->Draw(0.0f);
    
    // Update by 0.5s (halfway) -> center should be at (200, 200)
    // Frame rect: [200-50, 200-50] to [200+50, 200+50] -> [150, 150] to [250, 250]
    displayer->Update(0.5f);
    EXPECT_CALL(mockRs, DrawRoundRectFrame(150, 150, 250, 250, 8, 2, 0xFFFFFF))
        .Times(1);
    displayer->Draw(0.0f);
}

} // namespace
} // namespace mc
