#include "EnergySaver.h"

namespace GravityFun
{
    EnergySaver::EnergySaver(double energy_saving_factor)
        : IdlingTime(energy_saving_factor)
    {
        if (IdlingTime < 0)
            IdlingTime = 0;
        if (IdlingTime > 1)
            IdlingTime = 1;
    }

    void EnergySaver::SetIdlingTime(double value)
    {
        IdlingTime = value;
        if (IdlingTime < 0)
            IdlingTime = 0;
        if (IdlingTime > 1)
            IdlingTime = 1;
    }
    double EnergySaver::GetIdlingTime()
    {
        return IdlingTime;
    }

    void EnergySaver::OnRun()
    {
        if (IdlingTime == 0)
            return;
        auto g = GetParent();
        if (g == nullptr)
            return;
        g = g->GetParent();
        if (g == nullptr)
            return;
        Idle(IdlingTime);
    }
}
