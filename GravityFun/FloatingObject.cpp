#include "FloatingObject.h"

namespace GravityFun
{
    FloatingObject::FloatingObject(double mass, Math::Vec2 position, Math::Vec2 velocity)
        : Mass(mass), Position(position), Velocity(velocity)
    {
    }
}
