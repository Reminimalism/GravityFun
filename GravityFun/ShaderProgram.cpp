#include "ShaderProgram.h"

#include <stdexcept>

namespace GravityFun
{
    ShaderProgram::ShaderProgram(const char * vertex_shader_source, const char * fragment_shader_source)
    {
        int success;

        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char info_log[512];
            glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
            throw std::runtime_error(
                std::string("Vertex shader failed to compile: ")
                + info_log
            );
        }

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);
        if (!success)
        {
            glDeleteShader(vertex_shader);
            char info_log[512];
            glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
            throw std::runtime_error(
                std::string("Fragment shader failed to compile: ")
                + info_log
            );
        }

        Program = glCreateProgram();
        glAttachShader(Program, vertex_shader);
        glAttachShader(Program, fragment_shader);
        glLinkProgram(Program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char info_log[512];
            glGetProgramInfoLog(Program, 512, nullptr, info_log);
            throw std::runtime_error(
                std::string("Shader program linking failed: ")
                + info_log
            );
        }
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(Program);
    }

    void ShaderProgram::Use()
    {
        glUseProgram(Program);
    }

    GLint ShaderProgram::GetUniformLocation(const std::string& name)
    {
        return glGetUniformLocation(Program, name.c_str());
    }
}
