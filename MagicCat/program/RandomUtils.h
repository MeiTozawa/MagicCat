#pragma once
#include <cassert>
#include <random>

namespace mc {

namespace
{
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
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
        //template <typename... T> によって T は「型のリスト（パラメータパック）」になります（例: int, int, int）。 
        //しかし、std::discrete_distribution<T> の <T> には単一の整数型（戻り値の型である int や size_t など）を
        //指定しなければならないため、型のリストである T をそのまま渡すと「パラメータパックはこのコンテキストで
        //展開する必要があります」という文法エラーになってしまいます。
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

