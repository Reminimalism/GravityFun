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
constexpr float HINT_ICON_ANIMATION_SPEED = 4;

namespace GravityFun
{
    Renderer::Renderer(std::shared_ptr<Window> window, std::shared_ptr<GameManager> game_manager)
        : _Window(window), _GameManager(game_manager), MainThreadId(std::this_thread::get_id()),
          Program(SimpleVertexShaderSource, SimpleFragmentShaderSource),
          Circle(BufferGeneration::GenerateCircle(CIRCLE_RESOLUTION)),
          DownGravityToggle(BufferGeneration::GenerateDownGravityToggle(CIRCLE_RESOLUTION, HINT_ICON_Z)),
          RelativeGravityToggle(BufferGeneration::GenerateRelativeGravityToggle(CIRCLE_RESOLUTION, HINT_ICON_Z)),
          VariableMassToggle(BufferGeneration::GenerateVariableMassToggle(CIRCLE_RESOLUTION, HINT_ICON_Z)),
          BorderCollisionToggle(BufferGeneration::GenerateBorderCollisionToggle(CIRCLE_RESOLUTION, HINT_ICON_Z)),
          ObjectCollisionToggle(BufferGeneration::GenerateObjectCollisionToggle(CIRCLE_RESOLUTION, HINT_ICON_Z)),
          ObjectsCountSlider(BufferGeneration::GenerateObjectsCountSlider(CIRCLE_RESOLUTION, HINT_ICON_Z)),
          TimeMultiplierSlider(BufferGeneration::GenerateTimeMultiplierSlider(
                CIRCLE_RESOLUTION,
                (std::log2(GameManager::DEFAULT_TIME_MULTIPLIER) - std::log2(GameManager::MIN_TIME_MULTIPLIER))
                    / (std::log2(GameManager::MAX_TIME_MULTIPLIER) - std::log2(GameManager::MIN_TIME_MULTIPLIER)),
                HINT_ICON_Z
          )),
          EnergySavingSlider(BufferGeneration::GenerateEnergySavingSlider(CIRCLE_RESOLUTION, HINT_ICON_Z)),
          LastTime(std::chrono::steady_clock::now()),
          LoopScheduler::Module(false, nullptr, nullptr, true)
    {
        ProgramModelUniform = Program.GetUniformLocation("Model");
        ProgramViewUniform = Program.GetUniformLocation("View");
        ProgramProjectionUniform = Program.GetUniformLocation("Projection");
        ProgramColorUniform = Program.GetUniformLocation("Color");

        AnimatedModels.push_back(&DownGravityToggle);
        AnimatedModels.push_back(&RelativeGravityToggle);
        AnimatedModels.push_back(&VariableMassToggle);
        AnimatedModels.push_back(&BorderCollisionToggle);
        AnimatedModels.push_back(&ObjectCollisionToggle);
        AnimatedModels.push_back(&ObjectsCountSlider);
        AnimatedModels.push_back(&TimeMultiplierSlider);
        AnimatedModels.push_back(&EnergySavingSlider);
        AnimationTargetFunctions[&DownGravityToggle] = [this]() { return _GameManager->IsDownGravityOn() ? 1 : 0; };
        AnimationTargetFunctions[&RelativeGravityToggle] = [this]() { return _GameManager->IsRelativeGravityOn() ? 1 : 0; };
        AnimationTargetFunctions[&VariableMassToggle] = [this]() { return _GameManager->IsVariableMassOn() ? 1 : 0; };
        AnimationTargetFunctions[&BorderCollisionToggle] = [this]() { return _GameManager->IsBorderCollisionOn() ? 1 : 0; };
        AnimationTargetFunctions[&ObjectCollisionToggle] = [this]() { return _GameManager->IsObjectCollisionOn() ? 1 : 0; };
        AnimationTargetFunctions[&ObjectsCountSlider] = [this]() { return (float)_GameManager->GetObjectsCount() / GameManager::MAX_OBJECTS_COUNT; };
        AnimationTargetFunctions[&TimeMultiplierSlider] = [this]() {
            return (std::log2(_GameManager->GetTimeMultiplier()) - std::log2(GameManager::MIN_TIME_MULTIPLIER))
                / (std::log2(GameManager::MAX_TIME_MULTIPLIER) - std::log2(GameManager::MIN_TIME_MULTIPLIER));
        };
        AnimationTargetFunctions[&EnergySavingSlider] = [this]() {
            return (
                (_GameManager->GetPhysicsFidelity() - GameManager::MIN_PHYSICS_FIDELITY)
                / (GameManager::MAX_PHYSICS_FIDELITY - GameManager::MIN_PHYSICS_FIDELITY)
            );
        };

        for (auto& item : AnimatedModels)
            item->SetState(AnimationTargetFunctions[item]());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
        for (auto& item : AnimatedModels)
        {
            float temp = AnimationTargetFunctions[item]();
            if (
                    temp != item->GetState()
                    && (
                        !ActiveToggleAnimations.contains(item)
                        || temp != ActiveToggleAnimations[item].e
                    )
                )
                ActiveToggleAnimations[item] = Animation{
                    item->GetState(),
                    temp,
                    0
                };
        }

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
        glUniform4f(ProgramColorUniform, 1, 1, 1, 1);
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
        const auto& previous_buffer = _GameManager->GetPreviousRenderBuffer();
        const auto& buffer = _GameManager->GetRenderBuffer();
        for (int i = 0; i < _GameManager->GetObjectsCount(); i++)
        {
            const auto& item_previous = previous_buffer[i];
            const auto& item = buffer[i];

            auto pos = (item_previous.Position + item.Position) * 0.5;
            auto d = (item.Position - item_previous.Position);
            auto stretch = d.GetMagnitude() * 0.5;
            double angle = 0;
            if (stretch != 0)
            {
                d = d.GetNormalized();
                angle = d.y < 0 ? -std::acos(d.x) : std::acos(d.x);
            }
            auto model_matrix =
                Math::Matrix4x4::Translation(pos.x, pos.y, 0)
                * Math::Matrix4x4::RotationAroundZ(angle)
                * Math::Matrix4x4::Scale(
                    item.Mass * GameManager::MASS_TO_RADIUS + stretch,
                    item.Mass * GameManager::MASS_TO_RADIUS,
                    1
                );
            glUniformMatrix4fv(ProgramModelUniform, 1, GL_FALSE, model_matrix.GetData());

            std::uniform_real_distribution<double> distribution(0.5, 1.0);
            std::mt19937 mt(i * 3 + 0);
            double r = distribution(mt);
            mt.seed(i * 3 + 1);
            double g = distribution(mt);
            mt.seed(i * 3 + 2);
            double b = distribution(mt);
            double a = 1 / (1 + (stretch / (item.Mass * GameManager::MASS_TO_RADIUS)));
            glUniform4f(ProgramColorUniform, r, g, b, a);

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
