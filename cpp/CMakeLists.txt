project(HouLean)

# Specify the minimum required version of CMake to build the project.
cmake_minimum_required( VERSION 3.6 )

set(CMAKE_PREFIX_PATH "${HFS}/toolkit")

### LIBRARIES ###
find_package(Houdini REQUIRED)

add_subdirectory(src)
