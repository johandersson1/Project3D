#pragma once
#include <chrono>

struct Timer
{
private:
    std::chrono::time_point<std::chrono::steady_clock> start;
public:
    void Start()
    {
        start = std::chrono::steady_clock::now();
    }

    float DeltaTime()
    {
        auto end = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0f;
    }
};