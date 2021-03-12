set(BOX2D_BUILD_TESTBED OFF)
set(BOX2D_BUILD_UNIT_TESTS OFF)

set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)

add_subdirectory(fmt)
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(box2d)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)
