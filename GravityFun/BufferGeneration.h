#pragma once

#include <tuple>
#include <vector>

namespace GravityFun::BufferGeneration
{
    /// @param circle_resolution The number of vertices around the circle. The minimum is 8.
    /// @return Vertices of vec3 position and vec3 normal, and triangles' indices.
    ///         Position xy is in range [-1, 1] and z is 0.
    std::tuple<std::vector<float>, std::vector<unsigned int>> GenerateCircle(int circle_resolution);
    /// @return Vertices of vec3 position and vec3 normal, and triangles' indices.
    ///         Position xy is in range [-1, 1]. Normal is (0, 0, 1) in all vertices.
    std::tuple<std::vector<float>, std::vector<unsigned int>> GenerateSquare();
    /// @param corner_radius_x The x radius of the round corners. In range [0, 1].
    /// @param corner_radius_y The y radius of the round corners. In range [0, 1].
    /// @param corner_resolution The number of vertices on each corner. The minimum is 2.
    /// @return Vertices of vec3 position and vec3 normal, and triangles' indices.
    ///         Position xy is in range [-1, 1]. Normal is (0, 0, 1) in all vertices.
    std::tuple<std::vector<float>, std::vector<unsigned int>> GenerateRoundedSquare(int corner_resolution, float corner_radius_x, float corner_radius_y);
    /// @param z The z of vertices.
    /// @param line_offset The offset of the lines from center.
    /// @param line_thickness The thickness of the lines.
    /// @param arrow_head_width The width of the arrow head start.
    /// @param arrow_head_height The height of the arrow head.
    /// @return Vertices of vec3 position and vec3 normal, and triangles' indices.
    ///         Normal is (0, 0, 1) in all vertices.
    std::tuple<std::vector<float>, std::vector<unsigned int>> GenerateExitIcon(
        float z = 0.1,
        float line_offset = 0.5,
        float line_thickness = 0.1,
        float arrow_head_width = 0.3,
        float arrow_head_height = 0.3
    );

    /// @param circle_resolution The number of vertices around the circle. The minimum is 8.
    /// @param z The z of vertices.
    /// @return 2 vertex lists of vec3 position and vec3 normal, for 2 states, and triangles' indices.
    ///           Normal is (0, 0, 1) in all vertices.
    ///           Position xy is in range [-1, 1].
    std::tuple<std::vector<float>, std::vector<float>, std::vector<unsigned int>> GenerateDownGravityToggle(
        int circle_resolution,
        float z = 0.1
    );

    /// @param circle_resolution The number of vertices around the circle. The minimum is 8.
    /// @param z The z of vertices.
    /// @return 2 vertex lists of vec3 position and vec3 normal, for 2 states, and triangles' indices.
    ///           Normal is (0, 0, 1) in all vertices.
    ///           Position xy is in range [-1, 1].
    std::tuple<std::vector<float>, std::vector<float>, std::vector<unsigned int>> GenerateRelativeGravityToggle(
        int circle_resolution,
        float z = 0.1
    );
}
