#include "Renderer.h"

#include <cmath>
#include <numbers>
#include <random>

#include "BufferGeneration.h"
#include "Shaders.h"
#include "Window.h"
#include "GameManager.h"

constexpr int CIRCLE_RESOLUTION = 80;

constexpr int ROUNDED_SQUARE_CORNER_RESOLUTION = 20;
constexpr float ROUNDED_SQUARE_CORNER_RADIUS = 0.5;

constexpr float HINT_ICON_Z = 0.05;
constexpr float HINT_ICON_SIZE = 0.03;
constexpr float HINT_ICON_PADDING = 0.01;
constexpr float HINT_ICON_ANIMATION_SPEED = 2;

namespace GravityFun
{
    Renderer::Renderer(std::shared_ptr<Window> window, std::shared_ptr<GameManager> game_manager)
        : _Window(window), _GameManager(game_manager), MainThreadId(std::this_thread::get_id()),
          Program(SimpleVertexShaderSource, SimpleFragmentShaderSource),
          Circle(BufferGeneration::GenerateCircle(CIRCLE_RESOLUTION)),
          DownGravityToggle(BufferGeneration::GenerateDownGravityToggle(CIRCLE_RESOLUTION, HINT_ICON_Z)),
          LastTime(std::chrono::steady_clock::now()),
          LoopScheduler::Module(false, nullptr, nullptr, true)
    {
        ProgramModelUniform = Program.GetUniformLocation("Model");
        ProgramViewUniform = Program.GetUniformLocation("View");
        ProgramProjectionUniform = Program.GetUniformLocation("Projection");
        ProgramColorUniform = Program.GetUniformLocation("Color");

        AnimatedModels.push_back(&DownGravityToggle);
        DownGravityToggle.SetState(_GameManager->IsDownGravityOn() ? 1 : 0);

        glEnable(GL_DEPTH_TEST);
    }

    Renderer::~Renderer()
    {
    }

    bool Renderer::CanRun()
    {
        return std::this_thread::get_id() == MainThreadId;
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
        UpdateView();

        _Window->MakeCurrent();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Program.Use();

        // View
        glUniformMatrix4fv(ProgramViewUniform, 1, GL_FALSE, ViewMatrix.GetData());
        glUniformMatrix4fv(ProgramProjectionUniform, 1, GL_FALSE, ProjectionMatrix.GetData());

        // Update hints
        float temp = _GameManager->IsDownGravityOn() ? 1 : 0;
        if (
                temp != DownGravityToggle.GetState()
                && (
                    !ActiveToggleAnimations.contains(&DownGravityToggle)
                    || temp != ActiveToggleAnimations[&DownGravityToggle].e
                )
            )
            ActiveToggleAnimations[&DownGravityToggle] = Animation{
                DownGravityToggle.GetState(),
                temp,
                0
            };

        // Update time
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> time_diff_d = now - LastTime;
        float time_diff = time_diff_d.count();
        LastTime = now;
        if (time_diff > GameManager::MAX_TIME_DIFF)
            time_diff = GameManager::MAX_TIME_DIFF;
        float t_diff = time_diff * HINT_ICON_ANIMATION_SPEED;

        // Update animations
        std::vector<AnimatedModel*> animations_to_remove;
        for (auto& item : ActiveToggleAnimations)
        {
            item.second.t += t_diff;
            if (item.second.t >= 1)
            {
                item.first->SetState(
                    item.second.e
                );
                animations_to_remove.push_back(item.first);
            }
            else
            {
                item.first->SetState(
                    item.second.s + (item.second.e - item.second.s) * smoothstep(item.second.t)
                );
            }
        }
        for (auto& item : animations_to_remove)
            ActiveToggleAnimations.erase(item);

        // Render hints
        glUniform3f(ProgramColorUniform, 1, 1, 1);
        float start = -(HINT_ICON_SIZE + HINT_ICON_PADDING) * (AnimatedModels.size() - 1);
        for (int i = 0; i < AnimatedModels.size(); i++)
        {
            auto model = AnimatedModels[i];
            auto model_matrix =
                Math::Matrix4x4::Translation(start + i * (HINT_ICON_SIZE + HINT_ICON_PADDING) * 2, 1 - HINT_ICON_SIZE - HINT_ICON_PADDING, 0)
                * Math::Matrix4x4::Scale(
                    HINT_ICON_SIZE,
                    HINT_ICON_SIZE,
                    1
                );
            glUniformMatrix4fv(ProgramModelUniform, 1, GL_FALSE, model_matrix.GetData());
            model->Render();
        }

        // Render objects
        const auto& buffer = _GameManager->GetRenderBuffer();
        for (int i = 0; i < buffer.size(); i++)
        {
            const auto& item = buffer[i];
            std::uniform_real_distribution<double> distribution(0.5, 1.0);
            std::mt19937 mt(i * 3 + 0);
            double r = distribution(mt);
            mt.seed(i * 3 + 1);
            double g = distribution(mt);
            mt.seed(i * 3 + 2);
            double b = distribution(mt);
            glUniform3f(ProgramColorUniform, r, g, b);
            auto model_matrix =
                Math::Matrix4x4::Translation(item.Position.x, item.Position.y, 0)
                * Math::Matrix4x4::Scale(
                    item.Mass * GameManager::MASS_TO_RADIUS,
                    item.Mass * GameManager::MASS_TO_RADIUS,
                    1
                );
            glUniformMatrix4fv(ProgramModelUniform, 1, GL_FALSE, model_matrix.GetData());
            Circle.Render();
        }

        auto token = StartIdling(0, PredictLowerExecutionTime());
        _Window->SwapBuffers();
    }

    void Renderer::UpdateView()
    {
        ViewMatrix = Math::Matrix4x4::Scale(1 / _GameManager->GetBorderX(), 1 / _GameManager->GetBorderY(), -1);
        ProjectionMatrix = Math::Matrix4x4();
    }
}
