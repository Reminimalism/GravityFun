#include "Physics.h"

#include "GameManager.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace GravityFun
{
    Physics::Physics(std::shared_ptr<GameManager> game_manager, int number, int total, Physics * pass1)
        : _GameManager(game_manager), Number(number), Total(total), Hybrid(pass1 != nullptr), Pass1(pass1),
        LastTimeDiff(0), TimeDebt(0)
    {
        LastTime = std::chrono::steady_clock::now();
        for (int i = 0; i < GameManager::MAX_OBJECTS_COUNT; i++)
        {
            // To make sure the loops/conditions that check LastCollisions do not go out of range,
            // without needing the extra range condition.
            LastCollisions[i][0] = GameManager::MAX_OBJECTS_COUNT + 1;
        }
    }

    void Physics::OnRun()
    {
        const auto& read_buffer = Hybrid ?
            _GameManager->GetPhysicsPass2ReadBuffer()
            : _GameManager->GetPhysicsPass1ReadBuffer();
        auto& write_buffer = Hybrid ?
            _GameManager->GetPhysicsPass2WriteBuffer()
            : _GameManager->GetPhysicsPass1WriteBuffer();

        const auto& object_mapper = _GameManager->GetObjectMapper();

        const int objects_count = _GameManager->GetObjectsCount();
        const int begin = Number * objects_count / Total;
        const int end = (Number + 1) * objects_count / Total;

        if (Hybrid && _GameManager->IsObjectCollisionOn()) // Object collision mode
        {
            std::array<int, GameManager::MAX_COLLISION_COUNT> collided;
            std::array<Math::Vec2, GameManager::MAX_COLLISION_COUNT> collision_direction;
            std::array<double, GameManager::MAX_COLLISION_COUNT> collision_threshold;
            for (int i = begin; i < end; i++)
            {
                int collisions_count = 0;
                object_mapper.VisitObjects(read_buffer[i].Position, 2 * GameManager::MAX_MASS * GameManager::MASS_TO_RADIUS,
                    [&](int j) -> bool
                    {
                        if (i == j)
                            return false;
                        auto distance2d = read_buffer[i].Position - read_buffer[j].Position; // from j, towards i
                        double distance = distance2d.GetMagnitude();
                        double threshold = (read_buffer[i].Mass + read_buffer[j].Mass) * GameManager::MASS_TO_RADIUS;
                        if (distance < threshold)
                        {
                            collided[collisions_count] = j;
                            collision_direction[collisions_count] = distance2d.GetNormalized();
                            collision_threshold[collisions_count] = threshold;
                            collisions_count++;
                        }
                        return collisions_count >= GameManager::MAX_COLLISION_COUNT;
                    }
                );

                write_buffer[i].Position = read_buffer[i].Position;

                if (collisions_count == 0)
                {
                    write_buffer[i].Velocity = read_buffer[i].Velocity;
                    continue;
                }

                auto shared_velocity = read_buffer[i].Velocity * read_buffer[i].Mass;
                double shared_mass = read_buffer[i].Mass;
                int last_col_i = 0;
                for (int n = 0; n < collisions_count; n++)
                {
                    int j = collided[n];
                    while (LastCollisions[i][last_col_i] < j) // No range condition required
                    {
                        last_col_i++;
                    }
                    if (LastCollisions[i][last_col_i] == j) // No range condition required
                    {
                        last_col_i++;
                    }
                    else
                    {
                        shared_velocity = read_buffer[j].Velocity * read_buffer[j].Mass;
                        shared_mass += read_buffer[j].Mass;
                    }
                }
                shared_velocity /= shared_mass;

                Math::Vec2 rebound(0, 0);
                int rebound_count = 0;
                int new_collisions[GameManager::MAX_COLLISION_COUNT + 1];
                last_col_i = 0;
                for (int n = 0; n < collisions_count; n++)
                {
                    int j = collided[n];
                    new_collisions[n] = j;
                    while (LastCollisions[i][last_col_i] < j) // No range condition required
                    {
                        last_col_i++;
                    }
                    if (LastCollisions[i][last_col_i] == j) // No range condition required
                    {
                        last_col_i++;
                    }
                    else
                    {
                        auto col_dir = collision_direction[n];
                        double rebound_d = (
                            read_buffer[j].Velocity.GetDotProduct(col_dir)
                            - read_buffer[i].Velocity.GetDotProduct(col_dir)
                        ) * 0.5;
                        rebound += col_dir * rebound_d;
                        rebound_count++;
                    }

                    // Update position to exit collision
                    auto distance2d = write_buffer[i].Position - read_buffer[j].Position; // from other, towards i
                    double distance = distance2d.GetMagnitude();
                    double col_threshold = collision_threshold[n];
                    if (distance < col_threshold)
                    {
                        write_buffer[i].Position
                            += distance2d.GetNormalized()
                                * ((col_threshold - distance) * 0.5); // Each object goes 0.5 => successful exit
                    }
                }
                // The new_collisions[collisions_count] is guaranteed to have no range issues
                // as the array size is GameManager::MAX_COLLISION_COUNT + 1.
                // This last value is to make sure the loops/conditions that check LastCollisions
                // do not go out of range, without needing the extra range condition.
                new_collisions[collisions_count] = GameManager::MAX_OBJECTS_COUNT + 1;
                std::copy(std::begin(new_collisions), std::end(new_collisions), LastCollisions[i].begin());
                if (rebound_count != 0)
                {
                    rebound /= rebound_count;
                    write_buffer[i].Velocity = shared_velocity + rebound * 0.5 * GameManager::COLLISION_PRESERVE;
                }
                else // Collided but no new collision
                {
                    auto prev_pos = read_buffer[i].Position - read_buffer[i].Velocity * Pass1->TimeDiff;
                    write_buffer[i].Velocity = (write_buffer[i].Position - prev_pos) / Pass1->TimeDiff;
                }
            }
        }
        else // Normal mode (forces, motion, and border collision)
        {
            auto& last_time = Hybrid ? Pass1->LastTime : LastTime;
            auto& time_diff = Hybrid ? Pass1->TimeDiff : TimeDiff;
            auto& last_time_diff = Hybrid ? Pass1->LastTimeDiff : LastTimeDiff;
            auto& time_debt = Hybrid ? Pass1->TimeDebt : TimeDebt;
            auto time = std::chrono::steady_clock::now();
            double actual_time_diff = std::min(
                GameManager::MAX_TIME_DIFF,
                std::chrono::duration<double>(time - last_time).count()
            ) * _GameManager->GetTimeMultiplier();
            double offset = actual_time_diff + time_debt - last_time_diff;
            double correction = std::abs(offset / actual_time_diff); // Scales small changes down to reduce oscillation
            correction = _GameManager->GetTimeStrictness() * offset * correction;
            if (std::abs(correction) > std::abs(offset))
                correction = offset;
            time_diff = last_time_diff + correction;
            if (time_diff <= 0)
                time_diff = actual_time_diff * GameManager::MIN_TIME_DIFF_SCALE;
            last_time_diff = time_diff;
            time_debt += actual_time_diff - time_diff;
            last_time = time;

            bool g = _GameManager->IsRelativeGravityOn();
            double g_scale = _GameManager->GetRelativeGravityScale();
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
                    if (objects_count > 10)
                    {
                        object_mapper.VisitObjects(read_buffer[i].Position, GameManager::MASS_GRAVITY_RADIUS,
                            [&](int j) -> bool
                            {
                                if (i == j)
                                    return false;
                                auto distance2d = read_buffer[j].Position - read_buffer[i].Position;
                                double distance = distance2d.GetMagnitude();
                                if (distance > GameManager::MASS_GRAVITY_RADIUS)
                                    return false;
                                double f = distance == 0 ? 0 : read_buffer[j].Mass * GameManager::MASS_GRAVITY_ACCELERATION / (distance * distance);
                                net_acceleration += distance2d.GetNormalized() * f;
                                return false;
                            }
                        );
                    }
                    else
                    {
                        for (int j = 0; j < objects_count; j++)
                        {
                            if (i == j)
                                continue;
                            auto distance2d = read_buffer[j].Position - read_buffer[i].Position;
                            double distance = distance2d.GetMagnitude();
                            double f = distance == 0 ? 0 : read_buffer[j].Mass * GameManager::MASS_GRAVITY_ACCELERATION / (distance * distance);
                            net_acceleration += distance2d.GetNormalized() * f;
                        }
                    }
                    net_acceleration = net_acceleration * g_scale;
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
                // Braking (applied to velocity)
                if (braking)
                {
                    if (write_buffer[i].Velocity.x != 0)
                    {
                        double x = std::abs(write_buffer[i].Velocity.x) - GameManager::MOUSE_BRAKING_ACCELERATION * time_diff;
                        if (x <= 0)
                            write_buffer[i].Velocity.x = 0;
                        else
                            write_buffer[i].Velocity.x = write_buffer[i].Velocity.x > 0 ? x : -x;
                    }
                    if (write_buffer[i].Velocity.y != 0)
                    {
                        double y = std::abs(write_buffer[i].Velocity.y) - GameManager::MOUSE_BRAKING_ACCELERATION * time_diff;
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
                    double local_bx = bx - read_buffer[i].Mass * GameManager::MASS_TO_RADIUS;
                    double local_by = by - read_buffer[i].Mass * GameManager::MASS_TO_RADIUS;
                    if (write_buffer[i].Position.x < -local_bx)
                    {
                        write_buffer[i].Position.x = -local_bx + (-local_bx - write_buffer[i].Position.x);
                        write_buffer[i].Velocity.x = -write_buffer[i].Velocity.x * GameManager::COLLISION_PRESERVE;
                    }
                    if (write_buffer[i].Position.x > local_bx)
                    {
                        write_buffer[i].Position.x = local_bx + (local_bx - write_buffer[i].Position.x);
                        write_buffer[i].Velocity.x = -write_buffer[i].Velocity.x * GameManager::COLLISION_PRESERVE;
                    }
                    if (write_buffer[i].Position.y < -local_by)
                    {
                        write_buffer[i].Position.y = -local_by + (-local_by - write_buffer[i].Position.y);
                        write_buffer[i].Velocity.y = -write_buffer[i].Velocity.y * GameManager::COLLISION_PRESERVE;
                    }
                    if (write_buffer[i].Position.y > local_by)
                    {
                        write_buffer[i].Position.y = local_by + (local_by - write_buffer[i].Position.y);
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
