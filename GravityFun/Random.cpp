#include "Random.h"

namespace GravityFun
{
    Random::Random() : device(), mt(device())
    {
    }

    double Random::GetDouble(double min, double max)
    {
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(mt);
    }
}
