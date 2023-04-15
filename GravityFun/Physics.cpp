#include "Physics.h"

#include "GameManager.h"

#include <cmath>

namespace GravityFun
{
    Physics::Physics(std::shared_ptr<GameManager> game_manager, int number, int total, Physics * pass1)
        : _GameManager(game_manager), Number(number), Total(total), Hybrid(pass1 != nullptr), Pass1(pass1)
    {
        LastTime = std::chrono::steady_clock::now();
    }

    void Physics::OnRun()
    {
        const auto& read_buffer = Hybrid ?
            _GameManager->GetPhysicsPass2ReadBuffer()
            : _GameManager->GetPhysicsPass1ReadBuffer();
        auto& write_buffer = Hybrid ?
            _GameManager->GetPhysicsPass2WriteBuffer()
            : _GameManager->GetPhysicsPass1WriteBuffer();

        int begin = Number * read_buffer.size() / Total;
        int end = (Number + 1) * read_buffer.size() / Total;

        if (Hybrid && _GameManager->IsObjectCollisionOn()) // Object collision mode
        {
            for (int i = begin; i < end; i++)
            {
                std::vector<int> collided;
                std::vector<Math::Vec2> collision_direction;
                std::vector<double> collision_threshold;
                for (int j = 0; j < read_buffer.size(); j++)
                {
                    if (i == j)
                        continue;
                    auto distance2d = read_buffer[i].Position - read_buffer[j].Position; // from j, towards i
                    double distance = distance2d.GetMagnitude();
                    double threshold = (read_buffer[i].Mass + read_buffer[j].Mass) * GameManager::MASS_TO_RADIUS;
                    if (distance < threshold)
                    {
                        collided.push_back(j);
                        collision_direction.push_back(distance2d.GetNormalized());
                        collision_threshold.push_back(threshold);
                    }
                }

                write_buffer[i].Position = read_buffer[i].Position;

                if (collided.size() == 0)
                {
                    write_buffer[i].Velocity = read_buffer[i].Velocity;
                    continue;
                }

                auto shared_velocity = read_buffer[i].Velocity * read_buffer[i].Mass;
                double shared_mass = read_buffer[i].Mass;
                for (const auto& j : collided)
                {
                    shared_velocity = read_buffer[j].Velocity * read_buffer[j].Mass;
                    shared_mass += read_buffer[j].Mass;
                }
                shared_velocity /= shared_mass;

                Math::Vec2 rebound(0, 0);
                for (int n = 0; n < collided.size(); n++)
                {
                    double rebound_d = (
                        read_buffer[collided[n]].Velocity.GetDotProduct(collision_direction[n])
                        - read_buffer[i].Velocity.GetDotProduct(collision_direction[n])
                    ) * 0.5;
                    rebound += collision_direction[n] * rebound_d;

                    // Update position to exit collision
                    auto distance2d = write_buffer[i].Position - read_buffer[collided[n]].Position; // from other, towards i
                    double distance = distance2d.GetMagnitude();
                    if (distance < collision_threshold[n])
                    {
                        write_buffer[i].Position =
                            read_buffer[collided[n]].Position
                            + distance2d.GetNormalized() * collision_threshold[n];
                    }
                }
                rebound /= collided.size();

                write_buffer[i].Velocity = shared_velocity + rebound * 0.5 * GameManager::COLLISION_PRESERVE;
            }
        }
        else // Normal mode (forces, motion, and border collision)
        {
            auto& last_time = Hybrid ? Pass1->LastTime : LastTime;
            auto time = std::chrono::steady_clock::now();
            double time_diff = std::min(
                GameManager::MAX_TIME_DIFF,
                std::chrono::duration<double>(time - last_time).count()
            ) * _GameManager->GetTimeMultiplier();
            last_time = time;

            bool g = _GameManager->IsRelativeGravityOn();
            bool col = _GameManager->IsBorderCollisionOn();
            double bx = _GameManager->GetBorderX();
            double by = _GameManager->GetBorderY();
            Math::Vec2 mouse_position(_GameManager->GetMousePositionX(), _GameManager->GetMousePositionY());
            double mouse_g = _GameManager->IsMousePulling() ?
                                GameManager::MOUSE_GRAVITY_ACCELERATION
                                : (_GameManager->IsMousePushing() ? -GameManager::MASS_GRAVITY_ACCELERATION : 0);
            double braking = _GameManager->IsMouseBraking();
            double down_acceleration = _GameManager->IsDownGravityOn() ? GameManager::DOWN_GRAVITY_ACCELERATION : 0;
            for (int i = begin; i < end; i++)
            {
                // Acceleration
                Math::Vec2 net_acceleration(0, 0);
                if (g)
                {
                    for (int j = 0; j < read_buffer.size(); j++)
                    {
                        if (i == j)
                            continue;
                        auto distance2d = read_buffer[j].Position - read_buffer[i].Position;
                        double distance = distance2d.GetMagnitude();
                        double f = distance == 0 ? 0 : read_buffer[j].Mass * GameManager::MASS_GRAVITY_ACCELERATION / (distance * distance);
                        net_acceleration += distance2d.GetNormalized() * f;
                    }
                }
                net_acceleration.y -= down_acceleration;
                // Mouse pull/push
                if (mouse_g != 0)
                {
                    auto distance2d = mouse_position - read_buffer[i].Position;
                    double distance = distance2d.GetMagnitude();
                    double f = distance == 0 ? 0 : mouse_g / (distance * distance);
                    net_acceleration += distance2d.GetNormalized() * f;
                }
                // Velocity
                write_buffer[i].Velocity = read_buffer[i].Velocity + net_acceleration * time_diff;
                // Braking (velocity)
                if (braking)
                {
                    if (write_buffer[i].Velocity.x != 0)
                    {
                        double x = std::abs(write_buffer[i].Velocity.x) - GameManager::MOUSE_BRAKING_ACCELERATION;
                        if (x <= 0)
                            write_buffer[i].Velocity.x = 0;
                        else
                            write_buffer[i].Velocity.x = write_buffer[i].Velocity.x > 0 ? x : -x;
                    }
                    if (write_buffer[i].Velocity.y != 0)
                    {
                        double y = std::abs(write_buffer[i].Velocity.y) - GameManager::MOUSE_BRAKING_ACCELERATION;
                        if (y <= 0)
                            write_buffer[i].Velocity.y = 0;
                        else
                            write_buffer[i].Velocity.y = write_buffer[i].Velocity.y > 0 ? y : -y;
                    }
                }
                // Position
                write_buffer[i].Position = read_buffer[i].Position + write_buffer[i].Velocity * time_diff;
                // Handle out of borders position
                if (col) // Border collision => bounce
                {
                    if (write_buffer[i].Position.x < -bx)
                    {
                        write_buffer[i].Position.x = -bx + (-bx - write_buffer[i].Position.x);
                        write_buffer[i].Velocity.x = -write_buffer[i].Velocity.x * GameManager::COLLISION_PRESERVE;
                    }
                    if (write_buffer[i].Position.x > bx)
                    {
                        write_buffer[i].Position.x = bx + (bx - write_buffer[i].Position.x);
                        write_buffer[i].Velocity.x = -write_buffer[i].Velocity.x * GameManager::COLLISION_PRESERVE;
                    }
                    if (write_buffer[i].Position.y < -by)
                    {
                        write_buffer[i].Position.y = -by + (-by - write_buffer[i].Position.y);
                        write_buffer[i].Velocity.y = -write_buffer[i].Velocity.y * GameManager::COLLISION_PRESERVE;
                    }
                    if (write_buffer[i].Position.y > by)
                    {
                        write_buffer[i].Position.y = by + (by - write_buffer[i].Position.y);
                        write_buffer[i].Velocity.y = -write_buffer[i].Velocity.y * GameManager::COLLISION_PRESERVE;
                    }
                }
                else // No border collision => come from the other side
                {
                    if (write_buffer[i].Position.x < -bx)
                    {
                        write_buffer[i].Position.x = bx + (write_buffer[i].Position.x - (-bx));
                    }
                    if (write_buffer[i].Position.x > bx)
                    {
                        write_buffer[i].Position.x = -bx + (write_buffer[i].Position.x - bx);
                    }
                    if (write_buffer[i].Position.y < -by)
                    {
                        write_buffer[i].Position.y = by + (write_buffer[i].Position.y - (-by));
                    }
                    if (write_buffer[i].Position.y > by)
                    {
                        write_buffer[i].Position.y = -by + (write_buffer[i].Position.y - (by));
                    }
                }
            }
        }
    }
}
