#pragma once

#include <random>

namespace GravityFun
{
    class Random
    {
    public:
        Random();

        double GetDouble(double min = 0, double max = 1);
    private:
        std::random_device device;
        std::mt19937 mt;
    };
}
