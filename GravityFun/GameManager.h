#pragma once

#include "GravityFun.dec.h"

#include "Random.h"
#include "FloatingObject.h"

#include <array>
#include <chrono>
#include <memory>
#include <thread>

namespace GravityFun
{
    /// @brief The SetGroups function MUST be called before running the module.
    class GameManager final : public LoopScheduler::Module
    {
    public:
        class PhysicsPassNotifier final : public LoopScheduler::Module
        {
            friend GameManager;
        protected:
            virtual void OnRun() override;
        private:
            PhysicsPassNotifier(GameManager*);
            GameManager * _GameManager;
        };
        friend PhysicsPassNotifier;

        explicit GameManager(std::shared_ptr<Window>, std::shared_ptr<EnergySaver>);

        /// @brief MUST be called before running.
        ///        The owner must take care of the group lifetimes
        ///        as these are not a smart pointers.
        void SetGroups(
            LoopScheduler::Group * root_group,
            LoopScheduler::Group * physics_pass1,
            LoopScheduler::Group * physics_pass2
        );

        GameManager(const GameManager&) = delete;
        GameManager(GameManager&&) = delete;
        GameManager& operator=(const GameManager&) = delete;
        GameManager& operator=(GameManager&&) = delete;

        /// @brief This module has to be added after the two physics pass.
        std::shared_ptr<PhysicsPassNotifier> GetPhysicsPassNotifier();

        static constexpr int DEFAULT_OBJECTS_COUNT = 10;
        static constexpr int MIN_OBJECTS_COUNT = 0;
        static constexpr int MAX_OBJECTS_COUNT = 1000;
        static constexpr double DEFAULT_MASS = 1;
        static constexpr double MIN_MASS = 0.5;
        static constexpr double MAX_MASS = 2;
        /// @brief Converts object mass to object size's radius when multiplied by mass.
        static constexpr double MASS_TO_RADIUS = 0.01;
        static constexpr double DEFAULT_TIME_MULTIPLIER = 1;
        static constexpr double MIN_TIME_MULTIPLIER = 0.125;
        static constexpr double MAX_TIME_MULTIPLIER = 8;
        static constexpr double DEFAULT_PHYSICS_FIDELITY = 0.625;
        static constexpr double PHYSICS_FIDELITY_STEP = 0.0625;
        static constexpr double MIN_PHYSICS_FIDELITY = 0;
        static constexpr double MAX_PHYSICS_FIDELITY = 1;

        const std::array<FloatingObject, MAX_OBJECTS_COUNT>& GetRenderBuffer();
        const std::array<FloatingObject, MAX_OBJECTS_COUNT>& GetPhysicsPass1ReadBuffer();
        std::array<FloatingObject, MAX_OBJECTS_COUNT>& GetPhysicsPass1WriteBuffer();
        const std::array<FloatingObject, MAX_OBJECTS_COUNT>& GetPhysicsPass2ReadBuffer();
        std::array<FloatingObject, MAX_OBJECTS_COUNT>& GetPhysicsPass2WriteBuffer();

        /// @brief Used for physics.
        static constexpr int MAX_COLLISION_COUNT = 24;
        /// @brief Used for physics. See also: COLLISION_PRESERVE
        static constexpr double COLLISION_LOSS = 0.2;
        /// @brief Used for physics. COLLISION_PRESERVE = 1 - COLLISION_LOSS
        static constexpr double COLLISION_PRESERVE = 1 - COLLISION_LOSS;
        /// @brief Used for physics.
        static constexpr double DOWN_GRAVITY_ACCELERATION = 1;
        /// @brief Used for physics.
        static constexpr double MASS_GRAVITY_ACCELERATION = 0.02;
        /// @brief Used for physics.
        static constexpr double MOUSE_GRAVITY_ACCELERATION = 0.1;
        /// @brief Used for physics.
        static constexpr double MOUSE_BRAKING_ACCELERATION = 1;
        /// @brief Used for physics.
        static constexpr double MAX_TIME_DIFF = 0.1;
        /// @brief Used for physics.
        ///        This is the scale applied to the actual time diff
        ///        to prevent negative soft time diff.
        static constexpr double MIN_TIME_DIFF_SCALE = 0.5;

        /// @brief How quickly the time strictness changes
        static constexpr double TIME_STRICTNESS_UPDATE_ALPHA = 0.05;

        /// @brief Used for physics.
        ///        Represents how much the time diff is close to the actual time diff.
        double GetTimeStrictness();

        int GetObjectsCount();
        double GetTimeMultiplier();
        /// @return In range [0, 1]
        double GetPhysicsFidelity();
        bool IsDownGravityOn();
        bool IsRelativeGravityOn();
        bool IsVariableMassOn();
        bool IsBorderCollisionOn();
        bool IsObjectCollisionOn();
        double GetBorderX();
        double GetBorderY();
        /// @brief Width / Height
        double GetAspectRatio();
        double GetMousePositionX();
        double GetMousePositionY();
        bool IsMousePulling();
        bool IsMousePushing();
        bool IsMouseBraking();
    protected:
        virtual void OnRun() override;
        virtual bool CanRun() override;
    private:
        std::shared_ptr<Window> _Window;
        std::shared_ptr<PhysicsPassNotifier> _PhysicsPassNotifier;
        std::shared_ptr<EnergySaver> _EnergySaver;
        LoopScheduler::Group * RootGroup;
        LoopScheduler::Group * PhysicsPass1;
        LoopScheduler::Group * PhysicsPass2;

        std::thread::id MainThreadId;

        Random _Random;

        /// @brief 0 or 2
        int RenderBufferIndex;
        /// @brief 0 or 2 (first RenderBufferIndex, then PhysicsPass2WriteBufferIndex)
        int PhysicsPass1ReadBufferIndex;
        static constexpr int PhysicsPass1WriteBufferIndex = 1;
        static constexpr int PhysicsPass2ReadBufferIndex = 1;
        /// @brief 2 or 0 (!= RenderBufferIndex)
        int PhysicsPass2WriteBufferIndex;

        /// @brief DO NOT SET IT TO 0. Must be in (0, 1].
        double TimeStrictness;
        int PhysicsUpdates;
        double PhysicsUpdatesSoft;

        int ObjectsCount;
        double TimeMultiplier;
        double PhysicsFidelity;
        /// @brief Minimum allowed physics execution timespan
        double EnergySavingMinExec;
        bool DownGravityOn;
        bool RelativeGravityOn;
        bool VariableMassOn;
        bool BorderCollisionOn;
        bool ObjectCollisionOn;
        double BorderX;
        double BorderY;
        /// @brief Width / Height
        double AspectRatio;
        double MouseX;
        double MouseY;
        bool MouseLeft;
        bool MouseRight;
        bool MouseMiddle;

        std::array<FloatingObject, MAX_OBJECTS_COUNT> ObjectBuffers[3];

        void PhysicsPassNotify();

#if GRAVITYFUN_DEBUG
        std::chrono::steady_clock::time_point PhysicsRateLastTime;
        int PhysicsRateCounter;
#endif
    };
}
