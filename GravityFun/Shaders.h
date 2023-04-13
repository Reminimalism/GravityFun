#pragma once

const char * const SimpleVertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * View * Model * vec4(Position, 1.0);
}
)";

const char * const SimpleFragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

uniform vec3 Color;

void main()
{
    vec3 color = Color;
    FragColor = vec4(color, 1.0);
}
)";
