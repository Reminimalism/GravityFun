#include "Model.h"

#include <utility>

namespace GravityFun
{
    Model::Model(std::vector<float> vertices, std::vector<unsigned int> indices)
        : VerticesBuffer(0), IndicesBuffer(0)
    {
        Vertices = std::move(vertices);
        Indices = std::move(indices);

        glGenVertexArrays(1, &VertexArray);
        glGenBuffers(1, &VerticesBuffer);
        glGenBuffers(1, &IndicesBuffer);

        glBindVertexArray(VertexArray);

        glBindBuffer(GL_ARRAY_BUFFER, VerticesBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            Vertices.size() * sizeof(decltype(Vertices)::value_type),
            Vertices.data(),
            GL_STATIC_DRAW
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesBuffer);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            Indices.size() * sizeof(decltype(Indices)::value_type),
            Indices.data(),
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    Model::Model(std::tuple<std::vector<float>, std::vector<unsigned int>> buffers)
        : Model(std::get<0>(buffers), std::get<1>(buffers))
    {
    }

    Model::~Model()
    {
        glDeleteVertexArrays(1, &VertexArray);
        glDeleteBuffers(1, &VerticesBuffer);
        glDeleteBuffers(1, &IndicesBuffer);
    }

    void Model::Render()
    {
        glBindVertexArray(VertexArray);
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    }

    void Model::UpdateVertices()
    {
        glBindBuffer(GL_ARRAY_BUFFER, VerticesBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            Vertices.size() * sizeof(decltype(Vertices)::value_type),
            Vertices.data(),
            GL_STATIC_DRAW
        );

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
