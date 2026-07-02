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

// current < target, step small enough that current + step < target → returns current + step
RC_GTEST_PROP(StepTowards, UpwardWithoutOvershooting, ())
{
    float current = *finiteFloat();
    float gap  = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f) && f > 0.f;
    });
    float target = current + gap;
    if (!std::isfinite(target)) RC_SUCCEED();

    float step = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [gap](float f) {
        return std::isfinite(f) && f >= 0.f && f < gap;
    });

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == current + step);
}

// current < target, step >= gap → returns target (no overshoot)
RC_GTEST_PROP(StepTowards, UpwardClamped, ())
{
    float current = *finiteFloat();
    float gap = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f) && f > 0.f;
    });
    float target = current + gap;
    if (!std::isfinite(target)) RC_SUCCEED();

    float extra = *nonNegativeFiniteFloat();
    float step  = gap + extra;
    if (!std::isfinite(step)) RC_SUCCEED();

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == target);
}

// current > target, step small enough that current - step > target → returns current - step
RC_GTEST_PROP(StepTowards, DownwardWithoutUndershooting, ())
{
    float current = *finiteFloat();
    float gap = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f) && f > 0.f;
    });
    float target = current - gap;
    if (!std::isfinite(target)) RC_SUCCEED();

    float step = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [gap](float f) {
        return std::isfinite(f) && f >= 0.f && f < gap;
    });

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == current - step);
}

// current > target, step >= gap → returns target (no undershoot)
RC_GTEST_PROP(StepTowards, DownwardClamped, ())
{
    float current = *finiteFloat();
    float gap = *rc::gen::suchThat(rc::gen::arbitrary<float>(), [](float f) {
        return std::isfinite(f) && f > 0.f;
    });
    float target = current - gap;
    if (!std::isfinite(target)) RC_SUCCEED();

    float extra = *nonNegativeFiniteFloat();
    float step  = gap + extra;
    if (!std::isfinite(step)) RC_SUCCEED();

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == target);
}

// current == target → always returns target regardless of step
RC_GTEST_PROP(StepTowards, EqualStaysEqual, ())
{
    float current = *finiteFloat();
    float target  = current;
    float step    = *nonNegativeFiniteFloat();

    float result = StepTowards(current, target, step);
    RC_ASSERT(result == target);
}
