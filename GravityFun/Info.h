#pragma once

namespace GravityFun::Info
{
    /// @brief The name to print. No newline character.
    const char * const NAME = "Reminimalism - Gravity Fun";
    /// @brief The version without 'v'. No newline character.
    const char * const VERSION = "0.1-dev";

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
