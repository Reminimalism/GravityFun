# GravityFun

A simple program that simulates 2D floating objects and gravity for fun.
The rendering is done using OpenGL with the help of GLFW and GLAD.

# How to use

## Download (no build)

You can download Linux or Windows builds from [the latest release](https://github.com/Reminimalism/GravityFun/releases/latest).

For Linux, make sure to run `chmod u+x path/to/GravityFun`
in terminal to be able to run it (replace `path/to/GravityFun` with the downloaded executable file path).
Also, you can use [reminimalism-gravityfun.desktop](https://github.com/Reminimalism/GravityFun/blob/main/reminimalism-gravityfun.desktop)
for your desktop environment to show GravityFun in its apps menu.
Replace `/path/to/GravityFun` with the executable path and `/path/to/GravityFun.svg` with the icon path.
The icon can be found [here](https://github.com/Reminimalism/GravityFun/blob/main/icon/GravityFun.svg) too.

## Building on Linux

Install cmake, make and a C++ compiler like GCC (g++) or Clang (clang++).
Here's how you can install them in some Linux distributions (only one of clang++/g++ is needed):

Arch Linux: `sudo pacman -S cmake make clang gcc`

Debian/Ubuntu: `sudo apt install cmake make clang g++`

Go to a directory where you want to download the repository to,
and enter the following commands to clone and build the latest version (may be work in progress):

```
git clone https://github.com/Reminimalism/GravityFun.git
cd GravityFun
mkdir Build
cd Build
cmake ..
cmake --build .
```

From here, you can enter `./GravityFun/GravityFun` in the same terminal session to run the game.

After building, the executable file can be found in `GravityFun/Build/GravityFun/`.

## Building on Windows

The easiest option is to open the project folder directly in the latest Visual Studio if you have its own CMake tools installed.
The build configuration can be changed using the configuration menu (usually x64-Debug by default). The "Release" configuration is recommended, "RelWithDebInfo" or any other configuration is not recommended. The project can be built using Build > Build All.

Alternatively, download and install CMake from [their website](https://cmake.org/download/).
Download and extract this repository from the "Code" menu > Download Zip.
Open the extracted project in CMake.
Generate the project for an IDE and use the supported IDE to build.
