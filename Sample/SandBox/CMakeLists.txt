cmake_minimum_required(VERSION 3.15)



add_executable(SandBox 
Main.cpp)

find_package(fmt REQUIRED)
target_link_libraries(SandBox PRIVATE fmt::fmt)


target_link_libraries(SandBox PRIVATE adiosy_core)

# find_package(fmt REQUIRED)
# target_link_libraries(SandBox PRIVATE fmt::fmt)