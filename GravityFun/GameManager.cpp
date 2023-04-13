#include "GameManager.h"

#include <algorithm>

#include "Window.h"

namespace GravityFun
{
    GameManager::PhysicsPassNotifier::PhysicsPassNotifier(GameManager * gm) : _GameManager(gm) {}
    void GameManager::PhysicsPassNotifier::OnRun()
    {
        _GameManager->PhysicsPassNotify();
    }

    GameManager::GameManager(std::shared_ptr<Window> window)
        : _Window(window), _PhysicsPassNotifier(new PhysicsPassNotifier(this)),
          ObjectsCount(DEFAULT_OBJECTS_COUNT), TimeMultiplier(DEFAULT_TIME_MULTIPLIER),
          DownGravityOn(false), RelativeGravityOn(false),
          VariableWeightOn(false),
          BorderCollisionOn(true), ObjectCollisionOn(false),
          BorderX(1), BorderY(1),
          RenderBufferIndex(0),
          PhysicsPass1ReadBufferIndex(0), PhysicsPass2WriteBufferIndex(2)
    {
        for (int i = 0; i < 3; i++)
        {
            ObjectBuffers[i].resize(ObjectsCount);
            for (int j = 0; j < ObjectsCount; j++)
            {
                ObjectBuffers[i][j] = FloatingObject(
                    DEFAULT_MASS,
                    Math::Vec2(
                        _Random.GetDouble(-BorderX + DEFAULT_MASS * MASS_TO_RADIUS, BorderX - DEFAULT_MASS * MASS_TO_RADIUS),
                        _Random.GetDouble(-BorderY + DEFAULT_MASS * MASS_TO_RADIUS, BorderY - DEFAULT_MASS * MASS_TO_RADIUS)
                    )
                );
            }
        }
    }

    void GameManager::OnRun()
    {
        if (_Window->ShouldClose())
            GetLoop()->Stop();
        _Window->Update();

        std::swap(RenderBufferIndex, PhysicsPass2WriteBufferIndex);
        PhysicsPass1ReadBufferIndex = RenderBufferIndex;

        if (_Window->GetPressedKeys().contains(GLFW_KEY_UP)
            && _Window->GetReleasedKeys().contains(GLFW_KEY_UP))
        {
            auto temp = ObjectsCount + 1;
            if (temp <= MAX_OBJECTS_COUNT)
                ObjectsCount = temp;
        }
        if (_Window->GetPressedKeys().contains(GLFW_KEY_DOWN)
            && _Window->GetReleasedKeys().contains(GLFW_KEY_DOWN))
        {
            auto temp = ObjectsCount - 1;
            if (MIN_OBJECTS_COUNT <= temp)
                ObjectsCount = temp;
        }

        // TODO: Update buffers based on ObjectsCount

        if (_Window->GetPressedKeys().contains(GLFW_KEY_LEFT)
            && _Window->GetReleasedKeys().contains(GLFW_KEY_LEFT))
        {
            auto temp = TimeMultiplier * 0.5;
            if (MIN_TIME_MULTIPLIER <= temp)
                TimeMultiplier = temp;
        }
        if (_Window->GetPressedKeys().contains(GLFW_KEY_RIGHT)
            && _Window->GetReleasedKeys().contains(GLFW_KEY_RIGHT))
        {
            auto temp = TimeMultiplier * 2;
            if (temp <= MAX_TIME_MULTIPLIER)
                TimeMultiplier = temp;
        }

        if (_Window->GetPressedKeys().contains(GLFW_KEY_G))
            DownGravityOn = !DownGravityOn;
        if (_Window->GetPressedKeys().contains(GLFW_KEY_R))
            RelativeGravityOn = !RelativeGravityOn;
        if (_Window->GetPressedKeys().contains(GLFW_KEY_W))
            VariableWeightOn = !VariableWeightOn;
        if (_Window->GetPressedKeys().contains(GLFW_KEY_B))
            BorderCollisionOn = !BorderCollisionOn;
        if (_Window->GetPressedKeys().contains(GLFW_KEY_C))
            ObjectCollisionOn = !ObjectCollisionOn;

        int width, height;
        _Window->GetSize(width, height);
        AspectRatio = width / height;
        BorderX = AspectRatio;
        BorderY = 1;
    }

    void GameManager::PhysicsPassNotify()
    {
        PhysicsPass1ReadBufferIndex = PhysicsPass2WriteBufferIndex;
    }

    std::shared_ptr<GameManager::PhysicsPassNotifier> GameManager::GetPhysicsPassNotifier()
    {
        return _PhysicsPassNotifier;
    }

    const std::vector<FloatingObject>& GameManager::GetRenderBuffer()
    {
        return ObjectBuffers[RenderBufferIndex];
    }
    const std::vector<FloatingObject>& GameManager::GetPhysicsPass1ReadBuffer()
    {
        return ObjectBuffers[PhysicsPass1ReadBufferIndex];
    }
    std::vector<FloatingObject>& GameManager::GetPhysicsPass1WriteBuffer()
    {
        return ObjectBuffers[PhysicsPass1WriteBufferIndex];
    }
    const std::vector<FloatingObject>& GameManager::GetPhysicsPass2ReadBuffer()
    {
        return ObjectBuffers[PhysicsPass2ReadBufferIndex];
    }
    std::vector<FloatingObject>& GameManager::GetPhysicsPass2WriteBuffer()
    {
        return ObjectBuffers[PhysicsPass2WriteBufferIndex];
    }

    int GameManager::GetObjectsCount()
    {
        return ObjectsCount;
    }
    double GameManager::GetTimeMultiplier()
    {
        return TimeMultiplier;
    }
    bool GameManager::IsDownGravityOn()
    {
        return DownGravityOn;
    }
    bool GameManager::IsRelativeGravityOn()
    {
        return RelativeGravityOn;
    }
    bool GameManager::IsVariableWeightOn()
    {
        return VariableWeightOn;
    }
    bool GameManager::IsBorderCollisionOn()
    {
        return BorderCollisionOn;
    }
    bool GameManager::IsObjectCollisionOn()
    {
        return ObjectCollisionOn;
    }
    double GameManager::GetBorderX()
    {
        return BorderX;
    }
    double GameManager::GetBorderY()
    {
        return BorderY;
    }
    double GameManager::GetAspectRatio()
    {
        return AspectRatio;
    }
}
