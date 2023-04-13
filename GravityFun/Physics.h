#pragma once

#include "GravityFun.dec.h"

namespace GravityFun
{
    class Physics final : public LoopScheduler::Module
    {
    public:
        /// @brief A physics module that can work with a fixed number of other physics modules.
        /// @param number Zero-based number of this physics module.
        /// @param total The total number of physics modules.
        /// @param hybrid Whether the module is turned into a collision module
        ///               when the GameManager::IsObjectCollisionOn() returns true.
        ///               Else it will only calculate forces and motions.
        explicit Physics(std::shared_ptr<GameManager>, int number = 0, int total = 1, bool hybrid = false);

        Physics(const Physics&) = delete;
        Physics(Physics&&) = delete;
        Physics& operator=(const Physics&) = delete;
        Physics& operator=(Physics&&) = delete;

        virtual void OnRun() override;

        // TODO
    };
}
