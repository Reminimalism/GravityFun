#pragma once

#include "GravityFun.dec.h"

#include "../glad/include/glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "../glfw/include/GLFW/glfw3.h"

#include <atomic>
#include <functional>
#include <map>
#include <set>
#include <tuple>

namespace GravityFun
{
    class Window final
    {
    public:
        Window(const std::string& title = "GravityFun");
        ~Window();

        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        void SetResizeCallback(std::function<void(int width, int height)>);
        void GetSize(int& width, int& height);
        std::tuple<double, double> GetMousePosition();
        bool GetMouseLeftButton();
        bool GetMouseRightButton();
        bool GetMouseMiddleButton();
        const std::set<int>& GetPressedKeys();
        const std::set<int>& GetReleasedKeys();
        const std::set<int>& GetRepeatedKeys();
        void MakeCurrent();
        void Update();
        void SwapBuffers();
        bool ShouldClose();
        void Close();
    private:
        static int ObjectsCount;
        static std::map<GLFWwindow*, Window*> ObjectsMap;
        GLFWwindow * _GLFWWindow;
        std::function<void(int width, int height)> ResizeCallback;
        std::tuple<double, double> MousePosition;
        bool MouseLeftButton;
        bool MouseRightButton;
        bool MouseMiddleButton;
        std::set<int> PressedKeys;
        std::set<int> ReleasedKeys;
        std::set<int> RepeatedKeys;
    };
}
