cmake_minimum_required(VERSION 3.15)

add_library(adiosy_core
    Private/AdApplication.cpp
)

target_link_libraries(adiosy_core PUBLIC adiosy_platform)