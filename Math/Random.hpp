#pragma once
#include <random>

inline float randomFloat() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen);
}

inline float randRange(float min, float max) {
    std::tie(min, max) = std::minmax(min, max);
    return randomFloat() * (max - min) + min;
}
