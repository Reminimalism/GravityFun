if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))

    add_compile_options(-O2)

endif()

set(APP_ICON_RESOURCE_WINDOWS "")
if (WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "GravityFun.rc")
endif()

add_executable(GravityFun
    AnimatedModel.cpp
    BufferGeneration.cpp
    FloatingObject.cpp
    GameManager.cpp
    GravityFun.cpp
    Math.cpp
    Model.cpp
    Physics.cpp
    Random.cpp
    Renderer.cpp
    ShaderProgram.cpp
    Window.cpp
    ${APP_ICON_RESOURCE_WINDOWS}
)
target_link_libraries(GravityFun glfw)
target_link_libraries(GravityFun glad)
target_link_libraries(GravityFun LoopScheduler)
