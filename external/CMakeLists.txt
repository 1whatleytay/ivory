set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build the Box2D testbed")
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests")

set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")

add_subdirectory(fmt)
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(box2d)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)
