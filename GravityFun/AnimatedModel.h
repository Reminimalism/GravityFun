#pragma once

#include "GravityFun.dec.h"

#include "Model.h"

namespace GravityFun
{
    /// @brief A model that can be animated between 2 states.
    class AnimatedModel final : public Model
    {
    public:
        /// @param vertices0 Contains positions and normals of the 0 state.
        /// @param vertices1 Contains positions and normals of the 1 state.
        AnimatedModel(std::vector<float> vertices0, std::vector<float> vertices1, std::vector<unsigned int> indices);
        AnimatedModel(std::tuple<std::vector<float>, std::vector<float>, std::vector<unsigned int>> buffers);

        /// @param t A number between 0 and 1 that linearly controls the state of the model.
        void SetState(float t);
        float GetState();

        AnimatedModel(const AnimatedModel&) = delete;
        AnimatedModel(AnimatedModel&&) = delete;
        AnimatedModel& operator=(const AnimatedModel&) = delete;
        AnimatedModel& operator=(AnimatedModel&&) = delete;
    private:
        std::vector<float> Vertices0;
        std::vector<float> Vertices1;
        float State;
    };
}
