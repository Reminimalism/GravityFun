/*
    Copyright (C) 2022  Majidzadeh (hashpragmaonce@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "../LoopScheduler/LoopScheduler/LoopScheduler.h"

#ifndef GRAVITYFUN_DEBUG
    #define GRAVITYFUN_DEBUG 0
#endif

#if GRAVITYFUN_DEBUG
#include <iostream>
#include <string>
#endif

/*

(concurrency = hardware_concurrency by default)

Loop with threads=concurrency:
    SequentialGroup root:
        GameManager
        ParallelGroup physics & renderer:
            Renderer (idles on thread block)
            SequentialGroup* physics passes:
                ParallelGroup physics pass 1:
                    Physics[concurrency] // force/motion updates.
                GameManager.PhysicsPass1Notifier
                ParallelGroup physics pass 2:
                    Physics[concurrency] // resolves collisions between objects if on, else, normal force/motion update.
                GameManager.PhysicsPass2Notifier
                EnergySaver

GameManager uses Window
Renderer uses Window
Renderer uses GameManager
Physics uses GameManager

Renderer contains
    ShaderProgram
    Model
    AnimatedModel

*/

namespace GravityFun
{
#if GRAVITYFUN_DEBUG
    inline void Log(std::string message = "", std::string end = "\n") { std::cout << message << end; }
#endif
    class Window;
    class MouseEventManager;
    class ShaderProgram;
    class GameManager;
    class Renderer;
    class Physics;
    class EnergySaver;
}
