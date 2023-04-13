#include "Physics.h"

#include "GameManager.h"

namespace GravityFun
{
    Physics::Physics(std::shared_ptr<GameManager> game_manager, int number, int total, bool hybrid)
        : _GameManager(game_manager), Number(number), Total(total), Hybrid(hybrid)
    {
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
                }
                rebound /= collided.size();

                write_buffer[i].Velocity = shared_velocity + rebound;
            }
        }
        else // Normal mode (forces, motion, and border collision)
        {
            bool g = _GameManager->IsRelativeGravityOn();
            bool col = _GameManager->IsBorderCollisionOn();
            double bx = _GameManager->GetBorderX();
            double by = _GameManager->GetBorderY();
            double down_acceleration = _GameManager->IsDownGravityOn() ? GameManager::DOWN_GRAVITY_ACCELERATION : 0;
            for (int i = begin; i < end; i++)
            {
                Math::Vec2 net_acceleration(0, 0);
                if (g)
                {
                    for (int j = 0; j < read_buffer.size(); j++)
                    {
                        if (i == j)
                            continue;
                        auto distance2d = read_buffer[j].Position - read_buffer[i].Position;
                        double distance = distance2d.GetMagnitude();
                        double f = distance == 0 ? 0 : read_buffer[j].Mass / (distance * distance);
                        net_acceleration += distance2d.GetNormalized() * f;
                    }
                }
                net_acceleration.y -= down_acceleration;
                write_buffer[i].Velocity = read_buffer[i].Velocity + net_acceleration;
                write_buffer[i].Position = read_buffer[i].Position + write_buffer[i].Velocity;
                if (col)
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
                else
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
