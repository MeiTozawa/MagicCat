module;

#include <memory>
#include <cstdint>
#include <algorithm>
#include <RenderUtils.h>

export module Displayer:CutsceneFocus;

import DisplayerBase;
import RenderService;

namespace mc
{
    /// @brief カットシーンのスロット間をスライドする角丸矩形フォーカス枠の Displayer。
    /// 内部に線形補間アニメーションを持ち、source から destination へ duration 秒で移動する。
    export class CutsceneFocusDisplayer : public Displayer
    {
        IRenderService& m_renderService;

        // レイアウト
        float    m_halfWidth;
        float    m_halfHeight;
        int      m_cornerRadius;
        int      m_thickness;
        uint32_t m_color;

        // 線形補間アニメーション
        Point m_source;
        Point m_destination;
        float m_duration;
        float m_elapsed = 0.f;

    public:
        static constexpr float DEFAULT_DURATION = 0.5f;

        CutsceneFocusDisplayer(IRenderService& renderService,
                               Point source, Point destination,
                               float halfWidth, float halfHeight,
                               int cornerRadius, int thickness,
                               uint32_t color    = 0xFFFFFF,
                               float    duration = DEFAULT_DURATION)
            : m_renderService(renderService)
            , m_halfWidth(halfWidth), m_halfHeight(halfHeight)
            , m_cornerRadius(cornerRadius), m_thickness(thickness)
            , m_color(color)
            , m_source(source), m_destination(destination)
            , m_duration(duration)
        {}

        void OnUpdate(float deltaTime) override
        {
            if (deltaTime > 0.f)
                m_elapsed += deltaTime;
        }

        void OnDraw(float) const override
        {
            const float t = std::clamp(m_elapsed / m_duration, 0.f, 1.f);
            const float cx = m_source.x + (m_destination.x - m_source.x) * t;
            const float cy = m_source.y + (m_destination.y - m_source.y) * t;

            m_renderService.DrawRoundRectFrame(
                static_cast<int>(cx - m_halfWidth),
                static_cast<int>(cy - m_halfHeight),
                static_cast<int>(cx + m_halfWidth),
                static_cast<int>(cy + m_halfHeight),
                m_cornerRadius, m_thickness, m_color);
        }

    };

    export std::unique_ptr<CutsceneFocusDisplayer> CreateCutsceneFocusDisplayer(
        IRenderService& renderService,
        Point source, Point destination,
        float halfWidth, float halfHeight,
        int cornerRadius, int thickness,
        uint32_t color    = 0xFFFFFF,
        float    duration = CutsceneFocusDisplayer::DEFAULT_DURATION)
    {
        return std::make_unique<CutsceneFocusDisplayer>(
            renderService, source, destination,
            halfWidth, halfHeight, cornerRadius, thickness,
            color, duration);
    }
}
