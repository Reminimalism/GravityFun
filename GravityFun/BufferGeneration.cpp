#include "BufferGeneration.h"

#include <cmath>
#include <numbers>

namespace GravityFun::BufferGeneration
{
    std::tuple<std::vector<float>, std::vector<unsigned int>> GenerateCircle(int circle_resolution)
    {
        if (circle_resolution < 8)
            circle_resolution = 8;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // 0
        // center
        vertices.push_back(0); // position
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0); // normal
        vertices.push_back(0);
        vertices.push_back(1);

        // 1
        // circle start
        vertices.push_back(1);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        constexpr int CENTER_INDEX = 0;

        for (int i = 1; i < circle_resolution; i++)
        {
            double t = (2 * std::numbers::pi) * ((double)i/(double)circle_resolution);
            float x = (float)std::cos(t);
            float y = (float)std::sin(t);

            // i + 1
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(1);

            int current_index = i + 1;
            int previous_index = i;

            indices.push_back(CENTER_INDEX);
            indices.push_back(previous_index);
            indices.push_back(current_index);
        }

        constexpr int first_index = 1;
        int last_index = circle_resolution;

        indices.push_back(CENTER_INDEX);
        indices.push_back(last_index);
        indices.push_back(first_index);

        return std::make_tuple(vertices, indices);
    }

    std::tuple<std::vector<float>, std::vector<unsigned int>> GenerateSquare()
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // 0
        // bottom-left
        vertices.push_back(-1); // positions
        vertices.push_back(-1);
        vertices.push_back(0);
        vertices.push_back(0); // normal
        vertices.push_back(0);
        vertices.push_back(1);

        // 1
        // bottom-right
        vertices.push_back(1);
        vertices.push_back(-1);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 2
        // top-right
        vertices.push_back(1);
        vertices.push_back(1);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 3
        // top-left
        vertices.push_back(-1);
        vertices.push_back(1);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);

        return std::make_tuple(vertices, indices);
    }

    std::tuple<std::vector<float>, std::vector<unsigned int>> GenerateRoundedSquare(int corner_resolution, float corner_radius_x, float corner_radius_y)
    {
        if (corner_resolution < 2)
            corner_resolution = 2;
        if (corner_radius_x < 0)
            corner_radius_x = -corner_radius_x;
        if (corner_radius_x > 1)
            corner_radius_x = 1;
        if (corner_radius_y < 0)
            corner_radius_y = -corner_radius_y;
        if (corner_radius_y > 1)
            corner_radius_y = 1;

        float corner_start_x = 1 - corner_radius_x;
        float corner_start_y = 1 - corner_radius_y;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // 0
        // center
        vertices.push_back(0); // position
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0); // normal
        vertices.push_back(0);
        vertices.push_back(1);

        // 1
        // top-right corner start (on the right edge)
        vertices.push_back(1);
        vertices.push_back(corner_start_y);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 2
        // top-left corner start (on the top edge)
        vertices.push_back(-corner_start_x);
        vertices.push_back(1);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 3
        // bottom-left corner start (on the left edge)
        vertices.push_back(-1);
        vertices.push_back(-corner_start_y);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 4
        // bottom-right corner start (on the bottom edge)
        vertices.push_back(corner_start_x);
        vertices.push_back(-1);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        constexpr int TOP_RIGHT_CORNER_OFFSET = 0;
        constexpr int TOP_LEFT_CORNER_OFFSET = 1;
        constexpr int BOTTOM_LEFT_CORNER_OFFSET = 2;
        constexpr int BOTTOM_RIGHT_CORNER_OFFSET = 3;

        for (int i = 1; i < corner_resolution; i++)
        {
            double t = (double)i/(double)(corner_resolution - 1);
            double t1 = (2 * std::numbers::pi) * (t * 0.25);
            double t2 = (2 * std::numbers::pi) * (0.25 + t * 0.25);
            double t3 = (2 * std::numbers::pi) * (0.50 + t * 0.25);
            double t4 = (2 * std::numbers::pi) * (0.75 + t * 0.25);

            int current_index = 1 + i * 4;
            int previous_index = current_index - 4;

            // top-right corner
            float x = (float)std::cos(t1);
            float y = (float)std::sin(t1);
            vertices.push_back(corner_start_x + x * corner_radius_x);
            vertices.push_back(corner_start_y + y * corner_radius_y);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(1);

            // top-left corner
            x = (float)std::cos(t2);
            y = (float)std::sin(t2);
            vertices.push_back(-corner_start_x + x * corner_radius_x);
            vertices.push_back(corner_start_y + y * corner_radius_y);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(1);

            // bottom-left corner
            x = (float)std::cos(t3);
            y = (float)std::sin(t3);
            vertices.push_back(-corner_start_x + x * corner_radius_x);
            vertices.push_back(-corner_start_y + y * corner_radius_y);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(1);

            // bottom-right corner
            x = (float)std::cos(t4);
            y = (float)std::sin(t4);
            vertices.push_back(corner_start_x + x * corner_radius_x);
            vertices.push_back(-corner_start_y + y * corner_radius_y);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(1);

            indices.push_back(current_index + TOP_RIGHT_CORNER_OFFSET);
            indices.push_back(0);
            indices.push_back(previous_index + TOP_RIGHT_CORNER_OFFSET);

            indices.push_back(current_index + TOP_LEFT_CORNER_OFFSET);
            indices.push_back(0);
            indices.push_back(previous_index + TOP_LEFT_CORNER_OFFSET);

            indices.push_back(current_index + BOTTOM_LEFT_CORNER_OFFSET);
            indices.push_back(0);
            indices.push_back(previous_index + BOTTOM_LEFT_CORNER_OFFSET);

            indices.push_back(current_index + BOTTOM_RIGHT_CORNER_OFFSET);
            indices.push_back(0);
            indices.push_back(previous_index + BOTTOM_RIGHT_CORNER_OFFSET);
        }

        constexpr int first_index = 1;
        int last_index = 1 + (corner_resolution - 1) * 4;

        indices.push_back(first_index + TOP_RIGHT_CORNER_OFFSET);
        indices.push_back(0); // center
        indices.push_back(last_index + BOTTOM_RIGHT_CORNER_OFFSET);

        indices.push_back(first_index + TOP_LEFT_CORNER_OFFSET);
        indices.push_back(0);
        indices.push_back(last_index + TOP_RIGHT_CORNER_OFFSET);

        indices.push_back(first_index + BOTTOM_LEFT_CORNER_OFFSET);
        indices.push_back(0);
        indices.push_back(last_index + TOP_LEFT_CORNER_OFFSET);

        indices.push_back(first_index + BOTTOM_RIGHT_CORNER_OFFSET);
        indices.push_back(0);
        indices.push_back(last_index + BOTTOM_LEFT_CORNER_OFFSET);

        return std::make_tuple(vertices, indices);
    }

    std::tuple<std::vector<float>, std::vector<unsigned int>> GenerateExitIcon(float z,
                                                                               float line_offset,
                                                                               float line_thickness,
                                                                               float arrow_head_width,
                                                                               float arrow_head_height)
    {
        float half_thickness = line_thickness / 2;
        float line_outer_offset = line_offset + half_thickness;
        float line_inner_offset = line_offset - half_thickness;
        float arrow_head_side = arrow_head_width / 2;
        float arrow_head_end = line_inner_offset + arrow_head_height;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // 0
        // top-left of top quad AND top-left of left quad
        vertices.push_back(-line_outer_offset);
        vertices.push_back(line_outer_offset);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 1
        // bottom-left of top quad AND top-right of left quad
        vertices.push_back(-line_inner_offset);
        vertices.push_back(line_inner_offset);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 2
        // bottom-right of top quad
        vertices.push_back(line_inner_offset);
        vertices.push_back(line_inner_offset);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 3
        // top-right of top quad
        vertices.push_back(line_inner_offset);
        vertices.push_back(line_outer_offset);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);

        // 4
        // bottom-left of left quad AND bottom-left of bottom quad
        vertices.push_back(-line_outer_offset);
        vertices.push_back(-line_outer_offset);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 5
        // bottom-right of left quad AND top-left of bottom quad
        vertices.push_back(-line_inner_offset);
        vertices.push_back(-line_inner_offset);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        indices.push_back(0);
        indices.push_back(4);
        indices.push_back(5);

        indices.push_back(0);
        indices.push_back(5);
        indices.push_back(1);

        // 6
        // bottom-right of left quad
        vertices.push_back(line_inner_offset);
        vertices.push_back(-line_outer_offset);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 7
        // top-right of left quad
        vertices.push_back(line_inner_offset);
        vertices.push_back(-line_inner_offset);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        indices.push_back(5);
        indices.push_back(4);
        indices.push_back(6);

        indices.push_back(5);
        indices.push_back(6);
        indices.push_back(7);

        // 8
        // top-left of arrow line
        vertices.push_back(-half_thickness);
        vertices.push_back(half_thickness);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 9
        // bottom-left of arrow line
        vertices.push_back(-half_thickness);
        vertices.push_back(-half_thickness);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 10
        // bottom-right of arrow line
        vertices.push_back(line_inner_offset);
        vertices.push_back(-half_thickness);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 11
        // top-right of arrow line
        vertices.push_back(line_inner_offset);
        vertices.push_back(half_thickness);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        indices.push_back(8);
        indices.push_back(9);
        indices.push_back(10);

        indices.push_back(8);
        indices.push_back(10);
        indices.push_back(11);

        // 12
        // arrow head end (right)
        vertices.push_back(arrow_head_end);
        vertices.push_back(0);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 13
        // arrow top
        vertices.push_back(line_inner_offset);
        vertices.push_back(arrow_head_side);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        // 14
        // arrow bottom
        vertices.push_back(line_inner_offset);
        vertices.push_back(-arrow_head_side);
        vertices.push_back(z);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);

        indices.push_back(12);
        indices.push_back(13);
        indices.push_back(14);

        return std::make_tuple(vertices, indices);
    }
}
