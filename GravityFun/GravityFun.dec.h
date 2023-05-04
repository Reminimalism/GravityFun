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

/*

Loop with threads=hardware_concurrency + 1:
    SequentialGroup root:
        GameManager
        ParallelGroup physics & renderer:
            Renderer
            SequentialGroup* physics passes:
                ParallelGroup physics pass 1:
                    Physics[hardware_concurrency] // force/motion updates.
                ParallelGroup physics pass 2:
                    Physics[hardware_concurrency] // resolves collisions between objects if on, else, normal force/motion update.
                GameManager.PhysicsPassNotifier
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
    class Window;
    class MouseEventManager;
    class ShaderProgram;
    class GameManager;
    class Renderer;
    class Physics;
    class EnergySaver;
}
