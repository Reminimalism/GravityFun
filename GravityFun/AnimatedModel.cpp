#include "AnimatedModel.h"

#include <stdexcept>

namespace GravityFun
{
    AnimatedModel::AnimatedModel(std::vector<float> vertices0, std::vector<float> vertices1, std::vector<unsigned int> indices)
        : Vertices0(vertices0), Vertices1(vertices1), Model(vertices0, indices)
    {
        if (Vertices0.size() != Vertices1.size())
            throw std::length_error("Vertices0's and vertices1's sizes must be equal.");
    }
    AnimatedModel::AnimatedModel(std::tuple<std::vector<float>, std::vector<float>, std::vector<unsigned int>> buffers)
        : Vertices0(std::get<0>(buffers)), Vertices1(std::get<1>(buffers)), Model(std::get<0>(buffers), std::get<2>(buffers))
    {
        if (Vertices0.size() != Vertices1.size())
            throw std::length_error("Vertices0's and vertices1's sizes must be equal.");
    }

    void AnimatedModel::Update(float t)
    {
        for (int i = 0; i < Vertices0.size(); i++)
            Vertices[i] = Vertices0[i] + (Vertices1[i] - Vertices0[i]) * t;
        UpdateVertices();
    }
}
