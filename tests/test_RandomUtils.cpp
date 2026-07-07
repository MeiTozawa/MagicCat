#include <gtest/gtest.h>
#include <RandomUtils.h>
#include <vector>

namespace mc {
namespace {

TEST(RandomUtilsTest, DeterministicSeeding_ProducesSameSequence)
{
    std::vector<int> data1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> data2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    Random::Seed(42);
    Random::Shuffle(data1);

    Random::Seed(42);
    Random::Shuffle(data2);

    EXPECT_EQ(data1, data2);
}

} // namespace
} // namespace mc
