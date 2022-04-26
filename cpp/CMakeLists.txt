project(HouLean)

# Specify the minimum required version of CMake to build the project.
cmake_minimum_required( VERSION 3.6 )

set(CMAKE_PREFIX_PATH "${CMAKE_HFS}/toolkit:${HFS}/toolkit")

### LIBRARIES ###
find_package(Houdini REQUIRED)

### Houdini Plugin ###
add_library(libHouLeanCore SHARED HouLeanCore.cpp)

target_include_directories(libHouLeanCore PUBLIC "${CMAKE_LEAN_SYSROOT}/include")
target_link_directories(libHouLeanCore PUBLIC "${CMAKE_LEAN_SYSROOT}/lib/lean")
	    
target_link_libraries(libHouLeanCore PUBLIC Houdini)

# target_link_libraries(libHouLeanCore PUBLIC leanshared)

houdini_configure_target(libHouLeanCore)

### Houdini -> Lean C API ###
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(HouLeanCApi STATIC SopFunctions.cpp)

target_include_directories(HouLeanCApi PUBLIC "${CMAKE_LEAN_SYSROOT}/include")
target_include_directories(HouLeanCApi PUBLIC "${HFS}/toolkit/include")
target_include_directories(HouLeanCApi PUBLIC "${CMAKE_HFS}/toolkit/include")

target_link_libraries(HouLeanCApi PUBLIC Houdini)
