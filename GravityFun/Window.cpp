#include "Window.h"

#include <iostream>
#include <stdexcept>

namespace GravityFun
{
    int Window::ObjectsCount = 0;
    std::map<GLFWwindow*, Window*> Window::ObjectsMap;

    Window::Window() : ResizeCallback(nullptr),
                       MousePosition({0, 0}),
                       MouseLeftButton(false),
                       MouseRightButton(false),
                       MouseMiddleButton(false)
    {
        if (ObjectsCount == 0)
        {
            if (!glfwInit())
                throw std::logic_error("GLFW initialization failed.");
            glfwSetErrorCallback([](int error, const char * description) {
                std::cerr << "Error " << error << ": " << description << '\n';
            });
        }
        ObjectsCount++;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SAMPLES, 8);
        _GLFWWindow = glfwCreateWindow(640, 480, "GravityFun", NULL, NULL);
        //_GLFWWindow = glfwCreateWindow(640, 480, "GravityFun", glfwGetPrimaryMonitor(), NULL); // fullscreen
        if (!_GLFWWindow)
        {
            ObjectsCount--;
            throw std::logic_error("Window or OpenGL context creation failed.");
        }
        //GLFWmonitor * monitor = glfwGetPrimaryMonitor();
        //const GLFWvidmode * mode = glfwGetVideoMode(monitor);
        //glfwSetWindowMonitor(_GLFWWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate); // fullscreen
        glfwMakeContextCurrent(_GLFWWindow);
        gladLoadGL();
        glfwSwapInterval(1);
        ObjectsMap[_GLFWWindow] = this;
        glfwSetKeyCallback(_GLFWWindow, [](GLFWwindow * _GLFWWindow, int key, int scancode, int action, int mods) {
            auto window = Window::ObjectsMap[_GLFWWindow];
            if (action == GLFW_PRESS)
                window->PressedKeys.insert(key);
            if (action == GLFW_RELEASE)
                window->ReleasedKeys.insert(key);
            if (action == GLFW_REPEAT)
                window->RepeatedKeys.insert(key);
        });
        glfwSetFramebufferSizeCallback(_GLFWWindow, [](GLFWwindow * _GLFWWindow, int width, int height) {
            glViewport(0, 0, width, height);
            auto window = Window::ObjectsMap[_GLFWWindow];
            if (window->ResizeCallback != nullptr)
                window->ResizeCallback(width, height);
        });
        glEnable(GL_MULTISAMPLE);
    }

    void Window::SetResizeCallback(std::function<void(int width, int height)> callback)
    {
        ResizeCallback = callback;
    }

    void Window::GetSize(int& width, int& height)
    {
        glfwGetFramebufferSize(_GLFWWindow, &width, &height);
    }

    std::tuple<double, double> Window::GetMousePosition() { return MousePosition; }
    bool Window::GetMouseLeftButton() { return MouseLeftButton; }
    bool Window::GetMouseRightButton() { return MouseRightButton; }
    bool Window::GetMouseMiddleButton() { return MouseMiddleButton; }
    const std::set<int>& Window::GetPressedKeys() { return PressedKeys; }
    const std::set<int>& Window::GetReleasedKeys() { return ReleasedKeys; }
    const std::set<int>& Window::GetRepeatedKeys() { return RepeatedKeys; }

    void Window::MakeCurrent()
    {
        glfwMakeContextCurrent(_GLFWWindow);
    }

    void Window::Update()
    {
        double x, y;
        glfwGetCursorPos(_GLFWWindow, &x, &y);
        MousePosition = { x, y };
        MouseLeftButton = glfwGetMouseButton(_GLFWWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        MouseRightButton = glfwGetMouseButton(_GLFWWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        MouseMiddleButton = glfwGetMouseButton(_GLFWWindow, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
        PressedKeys.clear();
        ReleasedKeys.clear();
        RepeatedKeys.clear();
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(_GLFWWindow);
    }

    bool Window::ShouldClose()
    {
        return glfwWindowShouldClose(_GLFWWindow);
    }

    void Window::Close()
    {
        return glfwSetWindowShouldClose(_GLFWWindow, GLFW_TRUE);
    }

    Window::~Window()
    {
        glfwDestroyWindow(_GLFWWindow);
        ObjectsMap.erase(_GLFWWindow);

        ObjectsCount--;
        if (ObjectsCount == 0)
            glfwTerminate();
    }
}
