#include "BufferGeneration.h"

#include <cmath>
#include <numbers>

#include "Math.h"

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

    inline void generate_animated_circle(
            int& next_index,
            std::vector<float>& vertices0,
            std::vector<float>& vertices1,
            std::vector<unsigned int>& indices,
            float z,
            int circle_resolution,
            float center0x,
            float center0y,
            float radius0,
            float center1x,
            float center1y,
            float radius1
        )
    {
        // next_index + 0
        // center
        vertices0.push_back(center0x); // position
        vertices0.push_back(center0y);
        vertices0.push_back(z);
        vertices0.push_back(0); // normal
        vertices0.push_back(0);
        vertices0.push_back(1);
        vertices1.push_back(center1x); // position
        vertices1.push_back(center1y);
        vertices1.push_back(z);
        vertices1.push_back(0); // normal
        vertices1.push_back(0);
        vertices1.push_back(1);

        // next_index + 1
        // circle start
        vertices0.push_back(center0x + radius0);
        vertices0.push_back(center0y);
        vertices0.push_back(z);
        vertices0.push_back(0);
        vertices0.push_back(0);
        vertices0.push_back(1);
        vertices1.push_back(center1x + radius1);
        vertices1.push_back(center1y);
        vertices1.push_back(z);
        vertices1.push_back(0);
        vertices1.push_back(0);
        vertices1.push_back(1);

        int center_index = next_index + 0;

        for (int i = 1; i < circle_resolution; i++)
        {
            double t = (2 * std::numbers::pi) * ((double)i/(double)circle_resolution);
            float x = (float)std::cos(t);
            float y = (float)std::sin(t);

            // i + 1
            vertices0.push_back(center0x + x * radius0);
            vertices0.push_back(center0y + y * radius0);
            vertices0.push_back(z);
            vertices0.push_back(0);
            vertices0.push_back(0);
            vertices0.push_back(1);
            vertices1.push_back(center1x + x * radius1);
            vertices1.push_back(center1y + y * radius1);
            vertices1.push_back(z);
            vertices1.push_back(0);
            vertices1.push_back(0);
            vertices1.push_back(1);

            int current_index = next_index + i + 1;
            int previous_index = next_index + i;

            indices.push_back(center_index);
            indices.push_back(previous_index);
            indices.push_back(current_index);
        }

        int first_index = next_index + 1;
        int last_index = next_index + circle_resolution;

        indices.push_back(center_index);
        indices.push_back(last_index);
        indices.push_back(first_index);

        next_index = last_index + 1;
    }

    inline void generate_animated_arrow(
            int& next_index,
            std::vector<float>& vertices0,
            std::vector<float>& vertices1,
            std::vector<unsigned int>& indices,
            float z,
            Math::Vec2 tailpos0,
            Math::Vec2 headpos0,
            float tailsize0,
            Math::Vec2 headsize0,
            Math::Vec2 tailpos1,
            Math::Vec2 headpos1,
            float tailsize1,
            Math::Vec2 headsize1
        )
    {
        Math::Vec2 diff0 = headpos0 - tailpos0;
        Math::Vec2 diff1 = headpos1 - tailpos1;
        Math::Vec2 direction0 = diff0.GetNormalized();
        Math::Vec2 direction1 = diff1.GetNormalized();
        float length0 = (headpos0 - tailpos0).GetMagnitude();
        float length1 = (headpos1 - tailpos1).GetMagnitude();
        float taillength0 = length0 - headsize0.y;
        float taillength1 = length1 - headsize1.y;

        Math::Vec2 left0 = Math::Vec2(-direction0.y, direction0.x).GetNormalized();
        Math::Vec2 right0 = Math::Vec2(direction0.y, -direction0.x).GetNormalized();
        Math::Vec2 left1 = Math::Vec2(-direction1.y, direction1.x).GetNormalized();
        Math::Vec2 right1 = Math::Vec2(direction1.y, -direction1.x).GetNormalized();

        Math::Vec2 v0 = tailpos0 + left0 * tailsize0;
        Math::Vec2 v1 = tailpos1 + left1 * tailsize1;

        // arrow tail

        // next_index + 0
        vertices0.push_back(v0.x); // position
        vertices0.push_back(v0.y);
        vertices0.push_back(z);
        vertices0.push_back(0); // normal
        vertices0.push_back(0);
        vertices0.push_back(1);
        vertices1.push_back(v1.x); // position
        vertices1.push_back(v1.y);
        vertices1.push_back(z);
        vertices1.push_back(0); // normal
        vertices1.push_back(0);
        vertices1.push_back(1);

        v0 = tailpos0 + right0 * tailsize0;
        v1 = tailpos1 + right1 * tailsize1;

        // next_index + 1
        vertices0.push_back(v0.x);
        vertices0.push_back(v0.y);
        vertices0.push_back(z);
        vertices0.push_back(0);
        vertices0.push_back(0);
        vertices0.push_back(1);
        vertices1.push_back(v1.x);
        vertices1.push_back(v1.y);
        vertices1.push_back(z);
        vertices1.push_back(0);
        vertices1.push_back(0);
        vertices1.push_back(1);

        v0 = tailpos0 + (direction0 * taillength0) + right0 * tailsize0;
        v1 = tailpos1 + (direction1 * taillength1) + right1 * tailsize1;

        // next_index + 2
        vertices0.push_back(v0.x);
        vertices0.push_back(v0.y);
        vertices0.push_back(z);
        vertices0.push_back(0);
        vertices0.push_back(0);
        vertices0.push_back(1);
        vertices1.push_back(v1.x);
        vertices1.push_back(v1.y);
        vertices1.push_back(z);
        vertices1.push_back(0);
        vertices1.push_back(0);
        vertices1.push_back(1);

        v0 = tailpos0 + (direction0 * taillength0) + left0 * tailsize0;
        v1 = tailpos1 + (direction1 * taillength1) + left1 * tailsize1;

        // next_index + 3
        vertices0.push_back(v0.x);
        vertices0.push_back(v0.y);
        vertices0.push_back(z);
        vertices0.push_back(0);
        vertices0.push_back(0);
        vertices0.push_back(1);
        vertices1.push_back(v1.x);
        vertices1.push_back(v1.y);
        vertices1.push_back(z);
        vertices1.push_back(0);
        vertices1.push_back(0);
        vertices1.push_back(1);

        indices.push_back(next_index + 0);
        indices.push_back(next_index + 1);
        indices.push_back(next_index + 2);

        indices.push_back(next_index + 0);
        indices.push_back(next_index + 2);
        indices.push_back(next_index + 3);

        next_index += 4;

        // arrow head

        v0 = tailpos0 + (direction0 * taillength0) + left0 * headsize0.x;
        v1 = tailpos1 + (direction1 * taillength1) + left1 * headsize1.x;

        // next_index + 0
        vertices0.push_back(v0.x);
        vertices0.push_back(v0.y);
        vertices0.push_back(z);
        vertices0.push_back(0);
        vertices0.push_back(0);
        vertices0.push_back(1);
        vertices1.push_back(v1.x);
        vertices1.push_back(v1.y);
        vertices1.push_back(z);
        vertices1.push_back(0);
        vertices1.push_back(0);
        vertices1.push_back(1);

        v0 = tailpos0 + (direction0 * taillength0) + right0 * headsize0.x;
        v1 = tailpos1 + (direction1 * taillength1) + right1 * headsize1.x;

        // next_index + 1
        vertices0.push_back(v0.x);
        vertices0.push_back(v0.y);
        vertices0.push_back(z);
        vertices0.push_back(0);
        vertices0.push_back(0);
        vertices0.push_back(1);
        vertices1.push_back(v1.x);
        vertices1.push_back(v1.y);
        vertices1.push_back(z);
        vertices1.push_back(0);
        vertices1.push_back(0);
        vertices1.push_back(1);

        // next_index + 2
        vertices0.push_back(headpos0.x);
        vertices0.push_back(headpos0.y);
        vertices0.push_back(z);
        vertices0.push_back(0);
        vertices0.push_back(0);
        vertices0.push_back(1);
        vertices1.push_back(headpos1.x);
        vertices1.push_back(headpos1.y);
        vertices1.push_back(z);
        vertices1.push_back(0);
        vertices1.push_back(0);
        vertices1.push_back(1);

        indices.push_back(next_index + 0);
        indices.push_back(next_index + 1);
        indices.push_back(next_index + 2);

        next_index += 3;
    }

    std::tuple<std::vector<float>, std::vector<float>, std::vector<unsigned int>> GenerateDownGravityToggle(
            int circle_resolution,
            float z
        )
    {
        if (circle_resolution < 8)
            circle_resolution = 8;

        constexpr float off_state_circle_center_y = 0;
        constexpr float off_state_circle_size = 0.75;
        constexpr float on_state_circle_center_y = 0.5;
        constexpr float on_state_circle_size = 0.5;
        constexpr float arrow_size = 0.1;
        constexpr float arrow_head_size_x = 0.4;
        constexpr float arrow_start_y = 0.5;
        constexpr float arrow_head_start_y = -0.5;
        constexpr float arrow_head_size_y = 0.5;

        std::vector<float> vertices0;
        std::vector<float> vertices1;
        std::vector<unsigned int> indices;

        int next_index = 0;

        generate_animated_circle(
            next_index,
            vertices0,
            vertices1,
            indices,
            z, circle_resolution,
            0, off_state_circle_center_y, off_state_circle_size,
            0, on_state_circle_center_y, on_state_circle_size
        );

        generate_animated_arrow(
            next_index,
            vertices0,
            vertices1,
            indices,
            z,
            Math::Vec2(0, 0), Math::Vec2(0, -arrow_head_size_y),
            arrow_size, Math::Vec2(arrow_head_size_x / 2, arrow_head_size_y / 2),
            Math::Vec2(0, arrow_start_y), Math::Vec2(0, arrow_head_start_y - arrow_head_size_y),
            arrow_size, Math::Vec2(arrow_head_size_x, arrow_head_size_y)
        );

        return std::make_tuple(vertices0, vertices1, indices);
    }

    std::tuple<std::vector<float>, std::vector<float>, std::vector<unsigned int>> GenerateRelativeGravityToggle(
            int circle_resolution,
            float z
        )
    {
        if (circle_resolution < 8)
            circle_resolution = 8;

        std::vector<float> vertices0;
        std::vector<float> vertices1;
        std::vector<unsigned int> indices;

        int next_index = 0;

        generate_animated_circle(
            next_index,
            vertices0,
            vertices1,
            indices,
            z, circle_resolution,
            -0.5, 0, 0.4,
            -0.75, 0, 0.25
        );

        generate_animated_circle(
            next_index,
            vertices0,
            vertices1,
            indices,
            z, circle_resolution,
            0.5, 0, 0.4,
            0.75, 0, 0.25
        );

        generate_animated_arrow(
            next_index,
            vertices0,
            vertices1,
            indices,
            z,
            Math::Vec2(-0.5, 0), Math::Vec2(-0.25, 0),
            0.05, Math::Vec2(0.1, 0.2),
            Math::Vec2(-0.75, 0), Math::Vec2(-0.1, 0),
            0.05, Math::Vec2(0.2, 0.2)
        );

        generate_animated_arrow(
            next_index,
            vertices0,
            vertices1,
            indices,
            z,
            Math::Vec2(0.5, 0), Math::Vec2(0.25, 0),
            0.05, Math::Vec2(0.1, 0.2),
            Math::Vec2(0.75, 0), Math::Vec2(0.1, 0),
            0.05, Math::Vec2(0.2, 0.2)
        );

        return std::make_tuple(vertices0, vertices1, indices);
    }
}
