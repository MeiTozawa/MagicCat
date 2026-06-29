#pragma once
#include <cassert>
#include <random>

namespace mc {

namespace
{
    inline thread_local std::random_device rd;
    inline thread_local std::mt19937 gen(rd());
}

class Random
{
private:
    static std::mt19937& GetEngine()
    {
        return gen;
    }

public:
    Random() = delete;

    template <typename... T>
    static int RandomSelection(T... list)
    {
        // discrete_distribution は初期化子リストから整数型のみ受け取るため double にキャストする
        std::discrete_distribution<int> dist({static_cast<double>(list)...});
        return dist(GetEngine());
    }

    static int RangeInt(int min = INT32_MIN, int max = INT32_MAX)
    {
        std::uniform_int_distribution dist(min, max);
        return dist(GetEngine());
    }

    template <std::ranges::random_access_range R>
    static auto& Choice(R& container)
    {
        assert(!std::ranges::empty(container));
        std::uniform_int_distribution<size_t> dist(0, std::ranges::size(container) - 1);
        return container[dist(GetEngine())];
    }

    template <std::ranges::random_access_range R>
    static void Shuffle(R& container)
    {
        std::shuffle(std::ranges::begin(container), std::ranges::end(container), GetEngine());
    }
};

} // namespace mc

