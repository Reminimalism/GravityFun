#include "Renderer.h"

#include <cmath>
#include <numbers>

#include "BufferGeneration.h"
#include "Shaders.h"
#include "Window.h"

constexpr int CIRCLE_RESOLUTION = 80;

constexpr int ROUNDED_SQUARE_CORNER_RESOLUTION = 20;
constexpr float ROUNDED_SQUARE_CORNER_RADIUS = 0.5f;

constexpr float BUTTON_ICON_Z = 0.05f;

namespace GravityFun
{
    Renderer::Renderer(std::shared_ptr<Window> window, std::shared_ptr<GameManager> game_manager)
        : _Window(window), _GameManager(game_manager),
          Program(SimpleVertexShaderSource, SimpleFragmentShaderSource),
          Circle(BufferGeneration::GenerateCircle(CIRCLE_RESOLUTION))
    {
        ProgramModelUniform = Program.GetUniformLocation("Model");
        ProgramViewUniform = Program.GetUniformLocation("View");
        ProgramProjectionUniform = Program.GetUniformLocation("Projection");
        ProgramColorUniform = Program.GetUniformLocation("Color");

        glEnable(GL_DEPTH_TEST);
    }

    Renderer::~Renderer()
    {
    }

    float smoothstep(float t)
    {
        if (t < 0)
            return 0;
        if (t > 1)
            return 1;
        return t * t * (3 - 2 * t);
    }

    float smoothstart(float t)
    {
        if (t < 0)
            return 0;
        if (t > 1)
            return 1;
        return t * t * t;
    }

    float smoothstop(float t)
    {
        return 1 - smoothstart(1 - t);
    }

    void Renderer::OnRun()
    {
        _Window->MakeCurrent();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Program.Use();

        // View
        glUniformMatrix4fv(ProgramViewUniform, 1, GL_FALSE, ViewMatrix.GetData());
        glUniformMatrix4fv(ProgramProjectionUniform, 1, GL_FALSE, ProjectionMatrix.GetData());

        // Hints // TODO
        //glUniform3f(ProgramColorUniform, 1, 1, 1);
        //glUniformMatrix4fv(ProgramModelUniform, 1, GL_FALSE, HintModelMatrix.GetData());
        //Hint.Render();
        //glUniformMatrix4fv(ProgramModelUniform, 1, GL_FALSE, HintModelMatrix.GetData());
        //Hint.Render();
        //...

        // Objects
        // TODO

        _Window->SwapBuffers();
    }

    void Renderer::UpdateView() // TODO: call when needed
    {
        // TODO:
        //ViewMatrix = Math::Matrix4x4::Translation(?, ?, 0)
        //        * Math::Matrix4x4::Scale(?, ?, -1);
        ProjectionMatrix = Math::Matrix4x4();
    }
}
