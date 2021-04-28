set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build the Box2D testbed")
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests")

set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")

set(CLI11_BUILD_TESTS OFF CACHE BOOL "Build CLI11 tests")
set(CLI11_BUILD_EXAMPLES OFF CACHE BOOL "Build CLI11 examples")
set(CLI11_BUILD_DOCS OFF CACHE BOOL "Build CLI11 documentation")
set(CLI11_INSTALL OFF CACHE BOOL "Install the CLI11 folder to include during install process")

add_subdirectory(fmt)
add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(box2d)
add_subdirectory(CLI11)
add_subdirectory(pugixml)
add_subdirectory(freetype)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE asio/asio/include)

if (WIN32)
    target_link_libraries(asio INTERFACE ws2_32 wsock32)
endif()

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE rapidjson/include)
