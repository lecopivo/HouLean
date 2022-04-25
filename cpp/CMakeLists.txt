project(HouLean)

# Specify the minimum required version of CMake to build the project.
cmake_minimum_required( VERSION 3.6 )

set(CMAKE_PREFIX_PATH "${CMAKE_HFS}/toolkit")

### LIBRARIES ###
find_package(Houdini REQUIRED)

### Houdini Plugin ###
add_library(libHouLeanCore SHARED HouLeanCore.cpp)

target_include_directories(libHouLeanCore PUBLIC "${CMAKE_LEAN_SYSROOT}/include")
target_link_directories(libHouLeanCore PUBLIC "${CMAKE_LEAN_SYSROOT}/lib/lean")
	    
target_link_libraries(libHouLeanCore PUBLIC Houdini)
target_link_libraries(libHouLeanCore PUBLIC leanshared)

houdini_configure_target(libHouLeanCore)

