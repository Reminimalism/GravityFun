#pragma once

#include "GravityFun.dec.h"

namespace GravityFun
{
    /// @brief A module that idles to save energy,
    ///        designed to be used in a SequentialGroup that is in a ParallelGroup.
    class EnergySaver final : public LoopScheduler::Module
    {
    public:
        explicit EnergySaver(double energy_saving_factor = 0);

        EnergySaver(const EnergySaver&) = delete;
        EnergySaver(EnergySaver&&) = delete;
        EnergySaver& operator=(const EnergySaver&) = delete;
        EnergySaver& operator=(EnergySaver&&) = delete;

        void SetIdlingTime(double);
        double GetIdlingTime();
    protected:
        virtual void OnRun() override;
    private:
        double IdlingTime;
    };
}
