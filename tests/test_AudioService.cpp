// Feature: code-review-fixes, Property 6: StepTowards never overshoots
//
// Property-based tests for mc::StepTowards using RapidCheck.
// Verifies all five sub-cases: upward without overshoot, upward clamped,
// downward without undershoot, downward clamped, and equal stays equal.
//
// Validates: Requirements 11.2, 11.3, 11.4, 11.5, 11.6

#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include <cmath>

import AudioService;

using namespace mc;

namespace {

// Generator for finite (non-NaN, non-Inf) floats.
auto finiteFloat()
{
    return rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f);
    });
}

// Generator for non-negative finite floats (valid step values).
auto nonNegativeFiniteFloat()
{
    return rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f) && f >= 0.f;
    });
}

} // namespace

// ---------------------------------------------------------------------------
// Sub-case 1: current < target, step small → returns current + step
// Req 11.2: when step is small enough that current + step < target,
//           the result is exactly current + step.
// ---------------------------------------------------------------------------

RC_GTEST_PROP(StepTowards, UpwardWithoutOvershooting, ())
{
    // Generate current, then target strictly above current, then a small step
    float current = *finiteFloat();
    // target = current + some positive gap
    float gap  = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f) && f > 0.f;
    });
    float target = current + gap;
    if (!std::isfinite(target)) RC_SUCCEED(); // skip if addition overflowed to inf

    // step must be small enough: current + step < target, i.e. step < gap
    float step = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [gap](float f) {
        return std::isfinite(f) && f >= 0.f && f < gap;
    });

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == current + step);
}

// ---------------------------------------------------------------------------
// Sub-case 2: current < target, step large → returns target (clamped)
// Req 11.3: when step is large enough that current + step >= target,
//           the result is exactly target (no overshoot).
// ---------------------------------------------------------------------------

RC_GTEST_PROP(StepTowards, UpwardClamped, ())
{
    float current = *finiteFloat();
    float gap = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f) && f > 0.f;
    });
    float target = current + gap;
    if (!std::isfinite(target)) RC_SUCCEED();

    // step >= gap means current + step >= target
    float extra = *nonNegativeFiniteFloat();
    float step  = gap + extra;
    if (!std::isfinite(step)) RC_SUCCEED();

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == target);
}

// ---------------------------------------------------------------------------
// Sub-case 3: current > target, step small → returns current - step
// Req 11.4: when step is small enough that current - step > target,
//           the result is exactly current - step.
// ---------------------------------------------------------------------------

RC_GTEST_PROP(StepTowards, DownwardWithoutUndershooting, ())
{
    float current = *finiteFloat();
    float gap = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f) && f > 0.f;
    });
    float target = current - gap;
    if (!std::isfinite(target)) RC_SUCCEED();

    // step must be strictly less than gap so current - step > target
    float step = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [gap](float f) {
        return std::isfinite(f) && f >= 0.f && f < gap;
    });

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == current - step);
}

// ---------------------------------------------------------------------------
// Sub-case 4: current > target, step large → returns target (clamped)
// Req 11.5: when step is large enough that current - step <= target,
//           the result is exactly target (no undershoot).
// ---------------------------------------------------------------------------

RC_GTEST_PROP(StepTowards, DownwardClamped, ())
{
    float current = *finiteFloat();
    float gap = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f) && f > 0.f;
    });
    float target = current - gap;
    if (!std::isfinite(target)) RC_SUCCEED();

    // step >= gap means current - step <= target
    float extra = *nonNegativeFiniteFloat();
    float step  = gap + extra;
    if (!std::isfinite(step)) RC_SUCCEED();

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == target);
}

// ---------------------------------------------------------------------------
// Sub-case 5: current == target → always returns target
// Req 11.6: regardless of step, when current equals target the result is target.
// ---------------------------------------------------------------------------

RC_GTEST_PROP(StepTowards, EqualStaysEqual, ())
{
    float current = *finiteFloat();
    float target  = current; // they are identical
    float step    = *nonNegativeFiniteFloat();

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == target);
}
