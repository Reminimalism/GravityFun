#pragma once

#include "GravityFun.dec.h"

#include "../glad/include/glad/glad.h"

#include <tuple>
#include <vector>

namespace GravityFun
{
    class Model
    {
    public:
        /// @param vertices Contains positions and normals.
        Model(std::vector<float> vertices, std::vector<unsigned int> indices);
        Model(std::tuple<std::vector<float>, std::vector<unsigned int>> buffers);
        virtual ~Model();

        void Render();

        Model(const Model&) = delete;
        Model(Model&&) = delete;
        Model& operator=(const Model&) = delete;
        Model& operator=(Model&&) = delete;
    protected:
        std::vector<float> Vertices;
        std::vector<unsigned int> Indices;
        void UpdateVertices();
    private:
        GLuint VerticesBuffer;
        GLuint IndicesBuffer;
        GLuint VertexArray;
    };
}
