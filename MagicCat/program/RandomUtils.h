#pragma once
#include <random>

class Random {
private:
    static std::mt19937& GetEngine() {
        thread_local std::random_device rd;
        thread_local std::mt19937 gen(rd());
        return gen;
    }

public:
    Random() = delete;

    template<typename... T>
    static T RandomSelection(T ...weights) {
        std::discrete_distribution dist({weights...});
        return dist(GetEngine());
    }

};