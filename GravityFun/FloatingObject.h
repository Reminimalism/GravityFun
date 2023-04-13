#pragma once

#include "Math.h"

#include <vector>

namespace GravityFun
{
    class FloatingObject final
    {
    public:
        FloatingObject(double mass = 1, Math::Vec2 position = Math::Vec2(), Math::Vec2 velocity = Math::Vec2());

        double Mass;
        Math::Vec2 Position;
        Math::Vec2 Velocity;
    private:
    };
}
