#pragma once

#include "GravityFun.dec.h"

#include <string>

#include "../glad/include/glad/glad.h"

namespace GravityFun
{
    class ShaderProgram final
    {
    public:
        ShaderProgram(const char * vertex_shader_source, const char * fragment_shader_source);
        ~ShaderProgram();

        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram(ShaderProgram&&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;
        ShaderProgram& operator=(ShaderProgram&&) = delete;

        void Use();
        GLint GetUniformLocation(const std::string& name);
    private:
        GLuint Program;
    };
}
