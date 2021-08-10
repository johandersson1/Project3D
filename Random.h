#pragma once
#include <random>

struct Random
{
    static float Real(float min, float max)
    {
        // std::random_device is a uniformly-distributed integer random number generator that produces non-deterministic random numbers.
        // non-deterministic = provide different outputs for the same input on different executions
        std::random_device rd;
        // std::mt19937 class is a very efficient pseudo - random number generator
        std::mt19937 mt(rd());
        // Produces random floating-point values 
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