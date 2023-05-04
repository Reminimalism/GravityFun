#include "EnergySaver.h"

namespace GravityFun
{
    EnergySaver::EnergySaver(double energy_saving_factor)
        : EnergySavingFactor(energy_saving_factor)
    {
        if (EnergySavingFactor < 0)
            EnergySavingFactor = 0;
        if (EnergySavingFactor > 1)
            EnergySavingFactor = 1;
    }

    void EnergySaver::SetEnergySavingFactor(double value)
    {
        EnergySavingFactor = value;
        if (EnergySavingFactor < 0)
            EnergySavingFactor = 0;
        if (EnergySavingFactor > 1)
            EnergySavingFactor = 1;
    }
    double EnergySaver::GetEnergySavingFactor()
    {
        return EnergySavingFactor;
    }

    void EnergySaver::OnRun()
    {
        if (EnergySavingFactor == 0)
            return;
        auto g = GetParent();
        if (g == nullptr)
            return;
        g = g->GetParent();
        if (g == nullptr)
            return;
        Idle(EnergySavingFactor * g->PredictLowerRemainingExecutionTime());
    }
}
