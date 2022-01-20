
add_library(libHouLeanCore SHARED HouLeanCore.cpp)

target_include_directories(libHouLeanCore PUBLIC "~/.elan/toolchains/leanprover--lean4---nightly-2022-01-12/include")
target_link_directories(libHouLeanCore PUBLIC "~/.elan/toolchains/leanprover--lean4---nightly-2022-01-12/lib/lean")
	    
target_link_libraries(libHouLeanCore Houdini)

houdini_configure_target(libHouLeanCore)


