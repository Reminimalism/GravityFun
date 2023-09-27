#pragma once

namespace GravityFun::Info
{
    /// @brief The creater name to print. No newline character.
    const char * const CREATOR = "Reminimalism";
    /// @brief The name to print. No newline character.
    const char * const NAME = "Gravity Fun";
    /// @brief The version without 'v'. No newline character.
    const char * const VERSION = "0.8-dev";

    /// @brief Indented multiline string with extra newlines at the beginning and the end.
    const char * const DESCRIPTION = R"(
    Controls:
        G or 1: Toggle down gravity
        R or 2: Toggle relative gravity
        M or 3: Toggle variable mass (when adding objects)
        B or 4: Toggle border collision
        C or 5: Toggle object to object collision
        Up/Down: Add/remove objects
        Left/Right: Decrease/increase time multiplier (simulation speed)
            Possible time multiplier values: 0.125x, 0.25x, 0.5x, 1x, 2x, 4x, 8x
        -/+: Decrease/increase physics simulation CPU usage
        Mouse left button: Pull objects toward mouse
        Mouse right button: Push objects away from mouse
        Mouse middle button: Apply brake
    )";

    /// @brief Indented multiline string with extra newlines at the beginning and the end.
    const char * const DEPENDENCIES = R"(
    OpenGL loader generated by Glad: https://github.com/Dav1dde/glad
    GLFW: https://github.com/glfw/glfw
    LoopScheduler: https://github.com/LoopScheduler/LoopScheduler
    )";

    /// @brief Indented multiline string with extra newlines at the beginning and the end.
    const char * const LICENSE = R"(
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
    )";
}
