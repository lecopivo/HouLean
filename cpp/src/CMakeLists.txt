
add_library(libHouLeanCore SHARED HouLeanCore.cpp)

# Not ideal way to find lean but makes sure that the version is in sync with what is defined in the `lean-toolchain`
execute_process(COMMAND "lake" "script" "run" "lean_sysroot" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/.." OUTPUT_VARIABLE LEAN_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

target_include_directories(libHouLeanCore PUBLIC "${LEAN_SYSROOT}/include")
target_link_directories(libHouLeanCore PUBLIC "${LEAN_SYSROOT}/lib/lean")
target_link_directories(libHouLeanCore PUBLIC "${CMAKE_SOURCE_DIR}/../build/lib")
	    
target_link_libraries(libHouLeanCore PUBLIC Houdini)
target_link_libraries(libHouLeanCore PUBLIC leanshared)
# target_link_libraries(libHouLeanCore PUBLIC HouLean) -- this does not work

houdini_configure_target(libHouLeanCore)

