#pragma once
#include <random>

struct Random
{
    static float Real(float min, float max)
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> value(min, max);
        return value(mt);
    }

    static int Integer(int min, int max)
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> value(min, max);
        return value(mt);
    }
};