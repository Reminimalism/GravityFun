#pragma once

#include "GravityFun.dec.h"

#include "Math.h"
#include "Model.h"
#include "AnimatedModel.h"
#include "ShaderProgram.h"

#include "../glad/include/glad/glad.h"

#include <chrono>
#include <map>
#include <memory>
#include <thread>
#include <vector>

namespace GravityFun
{
    class Renderer final : public LoopScheduler::Module
    {
    public:
        Renderer(std::shared_ptr<Window>, std::shared_ptr<GameManager>);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;
    protected:
        virtual void OnRun() override;
        virtual bool CanRun() override;
    private:
        std::shared_ptr<Window> _Window;
        std::shared_ptr<GameManager> _GameManager;

        std::thread::id MainThreadId;

        Model Circle;
        AnimatedModel DownGravityToggle;
        std::vector<AnimatedModel*> AnimatedModels;
        struct Animation { public: float s, e, t; };
        std::map<AnimatedModel*, Animation> ActiveToggleAnimations;
        std::chrono::steady_clock::time_point LastTime;

        ShaderProgram Program;
        GLint ProgramModelUniform;
        GLint ProgramViewUniform;
        GLint ProgramProjectionUniform;
        GLint ProgramColorUniform;

        Math::Matrix4x4 CalculateCircleModelMatrix(double x, double y);
        Math::Matrix4x4 ViewMatrix;
        Math::Matrix4x4 ProjectionMatrix;

        void UpdateView();
    };
}
