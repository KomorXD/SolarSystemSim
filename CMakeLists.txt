cmake_minimum_required(VERSION 3.8)

project("SolarSystemSim" VERSION 0.0.3)

add_subdirectory("src")
add_subdirectory("dependencies/glfw")
add_subdirectory("dependencies/glm")
add_subdirectory("dependencies/spdlog")

if(MSVC AND NOT WIN32)
    set(GLAD_LIBRARIES dl)
endif()
