cmake_minimum_required(VERSION 3.15)


set(SHADERS 
      00_hello_triangle.vert
      00_hello_triangle.frag
)
spirv_shaders(
      SHADER_HEADERS
      SPIRV_VERSION
      1.0 #vulkan 1.0
      SOURCES
      ${SHADERS}
)


add_executable(SandBox
${SHADER_HEADERS}
Main.cpp)

find_package(fmt REQUIRED)
target_link_libraries(SandBox PRIVATE fmt::fmt)


target_link_libraries(SandBox PRIVATE adiosy_core)

# find_package(fmt REQUIRED)
# target_link_libraries(SandBox PRIVATE fmt::fmt)