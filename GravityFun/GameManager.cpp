#include "GameManager.h"

#include <algorithm>

#include "Window.h"
#include "EnergySaver.h"

namespace GravityFun
{
    GameManager::PhysicsPassNotifier::PhysicsPassNotifier(GameManager * gm, bool first_pass) : _GameManager(gm), FirstPass(first_pass) {}
    void GameManager::PhysicsPassNotifier::OnRun()
    {
        _GameManager->PhysicsPassNotify(FirstPass);
    }

    GameManager::GameManager(std::shared_ptr<Window> window, std::shared_ptr<EnergySaver> energy_saver)
        : _Window(window), _EnergySaver(energy_saver),
          RootGroup(nullptr), PhysicsPass1(nullptr), PhysicsPass2(nullptr),
          _PhysicsPass1Notifier(new PhysicsPassNotifier(this, true)),
          _PhysicsPass2Notifier(new PhysicsPassNotifier(this, false)),
          MainThreadId(std::this_thread::get_id()),
          TimeStrictness(1), PhysicsUpdates(1), PhysicsUpdatesSoft(1),
          ObjectsCount(DEFAULT_OBJECTS_COUNT), TimeMultiplier(DEFAULT_TIME_MULTIPLIER),
          PhysicsFidelity(DEFAULT_PHYSICS_FIDELITY),
          DownGravityOn(false), RelativeGravityState(0),
          VariableMassOn(false),
          BorderCollisionOn(true), ObjectCollisionOn(true),
          MotionBlurOn(true),
          BorderX(1), BorderY(1),
          PreviousRenderBufferIndex(0), RenderBufferIndex(1),
          PhysicsPass1ReadBufferIndex(1), PhysicsPass2WriteBufferIndex(2),
          LoopScheduler::Module(false, nullptr, nullptr, true)
    {
        for (int i = 0; i < ObjectsCount; i++)
        {
            FloatingObject new_obj(
                DEFAULT_MASS,
                Math::Vec2(
                    _Random.GetDouble(-BorderX + DEFAULT_MASS * MASS_TO_RADIUS, BorderX - DEFAULT_MASS * MASS_TO_RADIUS),
                    _Random.GetDouble(-BorderY + DEFAULT_MASS * MASS_TO_RADIUS, BorderY - DEFAULT_MASS * MASS_TO_RADIUS)
                )
            );
            for (int j = 0; j < 4; j++)
                ObjectBuffers[j][i] = new_obj;
        }
        UpdateMappedObjectBuffer(ObjectBuffers[PhysicsPass1ReadBufferIndex]);

        EnergySavingMinExec = 1 - PhysicsFidelity;
        EnergySavingMinExec = EnergySavingMinExec * EnergySavingMinExec * EnergySavingMinExec;
        _EnergySaver->SetIdlingTime(0);
#if GRAVITYFUN_DEBUG
        PhysicsRateLastTime = std::chrono::steady_clock::now();
        PhysicsRateCounter = 0;
#endif
    }

    void GameManager::SetGroups(
            LoopScheduler::Group * root_group,
            LoopScheduler::Group * physics_pass1,
            LoopScheduler::Group * physics_pass2
        )
    {
        RootGroup = root_group;
        PhysicsPass1 = physics_pass1;
        PhysicsPass2 = physics_pass2;
    }

    bool GameManager::CanRun()
    {
        return std::this_thread::get_id() == MainThreadId;
    }

    void GameManager::OnRun()
    {
        if (_Window->ShouldClose())
            GetLoop()->Stop();
        _Window->Update();

        auto temp_index = PreviousRenderBufferIndex;
        PreviousRenderBufferIndex = RenderBufferIndex;
        RenderBufferIndex = PhysicsPass2WriteBufferIndex;
        PhysicsPass2WriteBufferIndex = temp_index;
        PhysicsPass1ReadBufferIndex = RenderBufferIndex;

        // Toggles
        if (_Window->GetPressedKeys().contains(GLFW_KEY_G)
            || _Window->GetPressedKeys().contains(GLFW_KEY_1))
            DownGravityOn = !DownGravityOn;
        if (_Window->GetPressedKeys().contains(GLFW_KEY_R)
            || _Window->GetPressedKeys().contains(GLFW_KEY_2))
        {
            RelativeGravityState += 1;
            if (RelativeGravityState > 1)
                RelativeGravityState = -1;
        }
        if (_Window->GetPressedKeys().contains(GLFW_KEY_Q)
            || _Window->GetPressedKeys().contains(GLFW_KEY_0))
        {
            RelativeGravityState = 0;
        }
        if (_Window->GetPressedKeys().contains(GLFW_KEY_W)
            || _Window->GetPressedKeys().contains(GLFW_KEY_9))
        {
            RelativeGravityState = 1;
        }
        if (_Window->GetPressedKeys().contains(GLFW_KEY_E)
            || _Window->GetPressedKeys().contains(GLFW_KEY_8))
        {
            RelativeGravityState = -1;
        }
        if (_Window->GetPressedKeys().contains(GLFW_KEY_M)
            || _Window->GetPressedKeys().contains(GLFW_KEY_3))
            VariableMassOn = !VariableMassOn;
        if (_Window->GetPressedKeys().contains(GLFW_KEY_B)
            || _Window->GetPressedKeys().contains(GLFW_KEY_4))
            BorderCollisionOn = !BorderCollisionOn;
        if (_Window->GetPressedKeys().contains(GLFW_KEY_C)
            || _Window->GetPressedKeys().contains(GLFW_KEY_5))
            ObjectCollisionOn = !ObjectCollisionOn;
        if (_Window->GetPressedKeys().contains(GLFW_KEY_SLASH)
            || _Window->GetPressedKeys().contains(GLFW_KEY_6))
            MotionBlurOn = !MotionBlurOn;

        // Objects count
        int last_objects_count = ObjectsCount;
        if (_Window->GetPressedKeys().contains(GLFW_KEY_UP)
            || _Window->GetRepeatedKeys().contains(GLFW_KEY_UP))
        {
            for (int i = 0; i < ObjectsCount / 11 + 1; i++)
            {
                auto temp = ObjectsCount + 1;
                if (temp <= MAX_OBJECTS_COUNT)
                    ObjectsCount = temp;
            }
        }
        if (_Window->GetPressedKeys().contains(GLFW_KEY_DOWN)
            || _Window->GetRepeatedKeys().contains(GLFW_KEY_DOWN))
        {
            for (int i = 0; i < ObjectsCount / 11 + 1; i++)
            {
                auto temp = ObjectsCount - 1;
                if (MIN_OBJECTS_COUNT <= temp)
                    ObjectsCount = temp;
            }
        }

        // Update buffers based on objects count
        if (last_objects_count < ObjectsCount)
        {
            for (int i = last_objects_count; i < ObjectsCount; i++)
            {
                double mass = VariableMassOn ? _Random.GetDouble(MIN_MASS, MAX_MASS) : DEFAULT_MASS;
                FloatingObject new_obj(
                    mass,
                    Math::Vec2(
                        _Random.GetDouble(-BorderX + mass * MASS_TO_RADIUS, BorderX - mass * MASS_TO_RADIUS),
                        _Random.GetDouble(-BorderY + mass * MASS_TO_RADIUS, BorderY - mass * MASS_TO_RADIUS)
                    )
                );
                for (int j = 0; j < 4; j++)
                    ObjectBuffers[j][i] = new_obj;
            }
            // Only add new missing objects without clearing
            UpdateMappedObjectBuffer(ObjectBuffers[PhysicsPass1ReadBufferIndex], last_objects_count);
        }
        else if (ObjectsCount < last_objects_count)
        {
            // Clear and re-write from the reduced buffer
            UpdateMappedObjectBuffer(ObjectBuffers[PhysicsPass1ReadBufferIndex]);
        }

        // Time multiplier
        if (_Window->GetPressedKeys().contains(GLFW_KEY_LEFT)
            || _Window->GetRepeatedKeys().contains(GLFW_KEY_LEFT))
        {
            auto temp = TimeMultiplier * 0.5;
            if (MIN_TIME_MULTIPLIER <= temp)
                TimeMultiplier = temp;
        }
        if (_Window->GetPressedKeys().contains(GLFW_KEY_RIGHT)
            || _Window->GetRepeatedKeys().contains(GLFW_KEY_RIGHT))
        {
            auto temp = TimeMultiplier * 2;
            if (temp <= MAX_TIME_MULTIPLIER)
                TimeMultiplier = temp;
        }

        // Energy saving factor
        if (_Window->GetPressedKeys().contains(GLFW_KEY_MINUS)
            || _Window->GetRepeatedKeys().contains(GLFW_KEY_MINUS)
            || _Window->GetPressedKeys().contains(GLFW_KEY_KP_SUBTRACT)
            || _Window->GetRepeatedKeys().contains(GLFW_KEY_KP_SUBTRACT))
        {
            auto temp = PhysicsFidelity - PHYSICS_FIDELITY_STEP;
            if (MIN_PHYSICS_FIDELITY <= temp)
            {
                PhysicsFidelity = temp;
            }
            else
            {
                PhysicsFidelity = MIN_PHYSICS_FIDELITY;
            }
            EnergySavingMinExec = 1 - PhysicsFidelity;
            EnergySavingMinExec = EnergySavingMinExec * EnergySavingMinExec * EnergySavingMinExec;
        }
        if (_Window->GetPressedKeys().contains(GLFW_KEY_EQUAL)
            || _Window->GetRepeatedKeys().contains(GLFW_KEY_EQUAL)
            || _Window->GetPressedKeys().contains(GLFW_KEY_KP_ADD)
            || _Window->GetRepeatedKeys().contains(GLFW_KEY_KP_ADD))
        {
            auto temp = PhysicsFidelity + PHYSICS_FIDELITY_STEP;
            if (temp <= MAX_PHYSICS_FIDELITY)
            {
                PhysicsFidelity = temp;
            }
            else
            {
                PhysicsFidelity = MAX_PHYSICS_FIDELITY;
            }
            EnergySavingMinExec = 1 - PhysicsFidelity;
            EnergySavingMinExec = EnergySavingMinExec * EnergySavingMinExec * EnergySavingMinExec;
        }

        double min_exec = EnergySavingMinExec * RootGroup->PredictLowerExecutionTime();
        double exec = PhysicsPass1->PredictHigherExecutionTime() + PhysicsPass2->PredictHigherExecutionTime();
        if (min_exec <= exec)
        {
            _EnergySaver->SetIdlingTime(0);
        }
        else
        {
            _EnergySaver->SetIdlingTime(min_exec - exec);
        }

        // Update AspectRatio, BorderX, and BorderY
        int width, height;
        _Window->GetSize(width, height);
        AspectRatio = (double)width / height;
        BorderX = AspectRatio;
        BorderY = 1;

        auto [mouse_xi, mouse_yi] = _Window->GetMousePosition();
        MouseX = BorderX * (2 * (double)mouse_xi / width - 1);
        MouseY = BorderY * (-2 * (double)mouse_yi / height + 1);
        MouseLeft = _Window->GetMouseLeftButton();
        MouseRight = _Window->GetMouseRightButton();
        MouseMiddle = _Window->GetMouseMiddleButton();

        PhysicsUpdatesSoft += (PhysicsUpdates - PhysicsUpdatesSoft) * TIME_STRICTNESS_UPDATE_ALPHA;
        TimeStrictness = 1 / (double)PhysicsUpdatesSoft;
        PhysicsUpdates = 0;

#if GRAVITYFUN_DEBUG
        std::chrono::duration<double> diff = std::chrono::steady_clock::now() - PhysicsRateLastTime;
        double d = diff.count();
        if (d > 1)
        {
            Log(std::string("Physics update rate: ") + std::to_string(PhysicsRateCounter / d));
            PhysicsRateLastTime = std::chrono::steady_clock::now();
            PhysicsRateCounter = 0;
        }
#endif
    }

    void GameManager::PhysicsPassNotify(bool first_pass)
    {
        int next_read_buffer_index = first_pass ? PhysicsPass1WriteBufferIndex : PhysicsPass2WriteBufferIndex;
        UpdateMappedObjectBuffer(ObjectBuffers[next_read_buffer_index]);

        if (first_pass)
            return;

        PhysicsPass1ReadBufferIndex = PhysicsPass2WriteBufferIndex;
        PhysicsUpdates++;

#if GRAVITYFUN_DEBUG
        PhysicsRateCounter++;
#endif
    }

    void GameManager::UpdateMappedObjectBuffer(const std::array<FloatingObject, MAX_OBJECTS_COUNT>& object_buffer, int starting_index)
    {
        if (starting_index == 0)
            _ObjectMapper.Clear();
        for (int i = starting_index; i < ObjectsCount; i++)
        {
            _ObjectMapper.AddObject(object_buffer[i].Position, i);
        }
    }

    std::shared_ptr<GameManager::PhysicsPassNotifier> GameManager::GetPhysicsPass1Notifier()
    {
        return _PhysicsPass1Notifier;
    }

    std::shared_ptr<GameManager::PhysicsPassNotifier> GameManager::GetPhysicsPass2Notifier()
    {
        return _PhysicsPass2Notifier;
    }

    const std::array<FloatingObject, GameManager::MAX_OBJECTS_COUNT>& GameManager::GetPreviousRenderBuffer()
    {
        return ObjectBuffers[PreviousRenderBufferIndex];
    }
    const std::array<FloatingObject, GameManager::MAX_OBJECTS_COUNT>& GameManager::GetRenderBuffer()
    {
        return ObjectBuffers[RenderBufferIndex];
    }
    const std::array<FloatingObject, GameManager::MAX_OBJECTS_COUNT>& GameManager::GetPhysicsPass1ReadBuffer()
    {
        return ObjectBuffers[PhysicsPass1ReadBufferIndex];
    }
    std::array<FloatingObject, GameManager::MAX_OBJECTS_COUNT>& GameManager::GetPhysicsPass1WriteBuffer()
    {
        return ObjectBuffers[PhysicsPass1WriteBufferIndex];
    }
    const std::array<FloatingObject, GameManager::MAX_OBJECTS_COUNT>& GameManager::GetPhysicsPass2ReadBuffer()
    {
        return ObjectBuffers[PhysicsPass2ReadBufferIndex];
    }
    std::array<FloatingObject, GameManager::MAX_OBJECTS_COUNT>& GameManager::GetPhysicsPass2WriteBuffer()
    {
        return ObjectBuffers[PhysicsPass2WriteBufferIndex];
    }
    const GameManager::FloatingObjectMapper& GameManager::GetObjectMapper()
    {
        return _ObjectMapper;
    }

    double GameManager::GetTimeStrictness()
    {
        return TimeStrictness;
    }

    int GameManager::GetObjectsCount()
    {
        return ObjectsCount;
    }
    double GameManager::GetTimeMultiplier()
    {
        return TimeMultiplier;
    }
    double GameManager::GetPhysicsFidelity()
    {
        return PhysicsFidelity;
    }
    bool GameManager::IsDownGravityOn()
    {
        return DownGravityOn;
    }
    bool GameManager::IsRelativeGravityOn()
    {
        return RelativeGravityState != 0;
    }
    double GameManager::GetRelativeGravityScale()
    {
        return (double)RelativeGravityState;
    }
    bool GameManager::IsVariableMassOn()
    {
        return VariableMassOn;
    }
    bool GameManager::IsBorderCollisionOn()
    {
        return BorderCollisionOn;
    }
    bool GameManager::IsObjectCollisionOn()
    {
        return ObjectCollisionOn;
    }
    bool GameManager::IsMotionBlurOn()
    {
        return MotionBlurOn;
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
    double GameManager::GetMousePositionX() { return MouseX; }
    double GameManager::GetMousePositionY() { return MouseY; }
    bool GameManager::IsMousePulling() { return MouseLeft; }
    bool GameManager::IsMousePushing() { return MouseRight; }
    bool GameManager::IsMouseBraking() { return MouseMiddle; }
}
