cmake_minimum_required(VERSION 3.15)

add_library(adiosy_platform
    Private/AdLog.cpp
    Private/AdWindow.cpp
    Private/Window/AdGLFWwindow.cpp
)


target_include_directories(adiosy_platform PUBLIC External)

add_subdirectory(External/glfw-3.4)

target_link_libraries(adiosy_platform PRIVATE glfw)