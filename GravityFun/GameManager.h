#pragma once

#include "GravityFun.dec.h"

#include "Random.h"
#include "FloatingObject.h"

#include <memory>
#include <vector>

namespace GravityFun
{
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

        explicit GameManager(std::shared_ptr<Window>);

        GameManager(const GameManager&) = delete;
        GameManager(GameManager&&) = delete;
        GameManager& operator=(const GameManager&) = delete;
        GameManager& operator=(GameManager&&) = delete;

        /// @brief This module has to be added after the two physics pass.
        std::shared_ptr<PhysicsPassNotifier> GetPhysicsPassNotifier();

        const std::vector<FloatingObject>& GetRenderBuffer();
        const std::vector<FloatingObject>& GetPhysicsPass1ReadBuffer();
        std::vector<FloatingObject>& GetPhysicsPass1WriteBuffer();
        const std::vector<FloatingObject>& GetPhysicsPass2ReadBuffer();
        std::vector<FloatingObject>& GetPhysicsPass2WriteBuffer();

        static constexpr int DEFAULT_OBJECTS_COUNT = 10;
        static constexpr int MIN_OBJECTS_COUNT = 0;
        static constexpr int MAX_OBJECTS_COUNT = 100;
        static constexpr double DEFAULT_MASS = 1;
        static constexpr double MIN_MASS = 0.5;
        static constexpr double MAX_MASS = 2;
        /// @brief Converts object mass to object size's radius when multiplied by mass.
        static constexpr double MASS_TO_RADIUS = 0.1;
        static constexpr double DEFAULT_TIME_MULTIPLIER = 1;
        static constexpr double MIN_TIME_MULTIPLIER = 0.125;
        static constexpr double MAX_TIME_MULTIPLIER = 8;

        /// @brief Used for physics. See also: COLLISION_PRESERVE
        static constexpr double COLLISION_LOSS = 0.1;
        /// @brief Used for physics. COLLISION_PRESERVE = 1 - COLLISION_LOSS
        static constexpr double COLLISION_PRESERVE = 1 - COLLISION_LOSS;
        static constexpr double DOWN_GRAVITY_ACCELERATION = 1;

        int GetObjectsCount();
        double GetTimeMultiplier();
        bool IsDownGravityOn();
        bool IsRelativeGravityOn();
        bool IsVariableMassOn();
        bool IsBorderCollisionOn();
        bool IsObjectCollisionOn();
        double GetBorderX();
        double GetBorderY();
        /// @brief Width / Height
        double GetAspectRatio();
        // double GetComputationIntensity(); // TODO ?
    protected:
        virtual void OnRun() override;
    private:
        std::shared_ptr<Window> _Window;
        std::shared_ptr<PhysicsPassNotifier> _PhysicsPassNotifier;

        Random _Random;

        std::vector<FloatingObject> ObjectBuffers[3];
        /// 0 or 2
        int RenderBufferIndex;
        /// 0 or 2 (first RenderBufferIndex, then PhysicsPass2WriteBufferIndex)
        int PhysicsPass1ReadBufferIndex;
        static constexpr int PhysicsPass1WriteBufferIndex = 1;
        static constexpr int PhysicsPass2ReadBufferIndex = 1;
        /// 2 or 0 (!= RenderBufferIndex)
        int PhysicsPass2WriteBufferIndex;

        int ObjectsCount;
        double TimeMultiplier;
        bool DownGravityOn;
        bool RelativeGravityOn;
        bool VariableMassOn;
        bool BorderCollisionOn;
        bool ObjectCollisionOn;
        double BorderX;
        double BorderY;
        /// @brief Width / Height
        double AspectRatio;

        void PhysicsPassNotify();
    };
}
