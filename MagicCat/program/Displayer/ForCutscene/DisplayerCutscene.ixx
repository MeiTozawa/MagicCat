module;

#include <memory>
#include <RenderUtils.h>

export module Displayer:Cutscene;
export import :Core;
import RenderService;

namespace mc {
    export std::unique_ptr<IDisplayer> CreateCutsceneFocusDisplayer(
        IRenderService& renderService,
        Point<float> source,
        Point<float> destination,
        float halfWidth,
        float halfHeight,
        int cornerRadius,
        int thickness,
        uint32_t color = 0xFFFFFF,
        float duration = 0.5f);
}
