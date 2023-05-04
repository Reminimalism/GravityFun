#pragma once

#include "GravityFun.dec.h"

namespace GravityFun
{
    class EnergySaver final : public LoopScheduler::Module
    {
    public:
        /// @brief A module that idles to save energy,
        ///        designed to be used in a SequentialGroup that is in a ParallelGroup.
        explicit EnergySaver(double energy_saving_factor = 0);

        EnergySaver(const EnergySaver&) = delete;
        EnergySaver(EnergySaver&&) = delete;
        EnergySaver& operator=(const EnergySaver&) = delete;
        EnergySaver& operator=(EnergySaver&&) = delete;

        void SetEnergySavingFactor(double);
        double GetEnergySavingFactor();
    protected:
        virtual void OnRun() override;
    private:
        double EnergySavingFactor;
    };
}
