cmake_minimum_required(VERSION 3.15)

add_library(adiosy_platform
    Private/AdLog.cpp
    Private/AdWindow.cpp
    Private/Window/AdGLFWwindow.cpp
    
    Private/AdGraphicContext.cpp
    Private/Graphic/AdVKGraphicContext.cpp
    Private/Graphic/AdVKDevice.cpp
    Private/Graphic/AdVKQueue.cpp
    )


target_include_directories(adiosy_platform PUBLIC External)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)

add_subdirectory(External/glfw-3.4)

target_link_libraries(adiosy_platform PRIVATE glfw)

find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message("--->Find Vulkan success:${Vulkan_INCLUDE_DIRS}")
endif()
target_include_directories(adiosy_platform PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(adiosy_platform PRIVATE ${Vulkan_LIBRARY})

