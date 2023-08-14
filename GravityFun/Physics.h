#pragma once

#include "GravityFun.dec.h"

#include <chrono>
#include <memory>
#include <set>
#include <vector>

namespace GravityFun
{
    class Physics final : public LoopScheduler::Module
    {
    public:
        /// @brief A physics module that can work with a fixed number of other physics modules.
        /// @param number Zero-based number of this physics module.
        /// @param total The total number of physics modules.
        /// @param pass1 The pass1 module with the same number if this is a hybrid (pass2) module.
        ///              Hybrid means whether the module is turned into a object collision module
        ///              when the GameManager::IsObjectCollisionOn() returns true.
        ///              Else it will only calculate forces, motion, and border collision.
        ///              Also, if true, this module's considered a part of pass2, else pass1.
        explicit Physics(std::shared_ptr<GameManager>, int number = 0, int total = 1, Physics * pass1 = nullptr);

        Physics(const Physics&) = delete;
        Physics(Physics&&) = delete;
        Physics& operator=(const Physics&) = delete;
        Physics& operator=(Physics&&) = delete;
    protected:
        virtual void OnRun() override;
    private:
        std::shared_ptr<GameManager> _GameManager;
        int Number;
        int Total;
        bool Hybrid;
        Physics * Pass1;

        std::chrono::steady_clock::time_point LastTime;
        double TimeDiff;
        double LastTimeDiff;
        double TimeDebt;

        /// @brief Used for collision mode
        int LastObjectCount;
        /// @brief Used for collision mode
        std::vector<std::set<int>> LastCollisions;
    };
}
