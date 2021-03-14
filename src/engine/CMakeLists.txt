add_library(engine STATIC
    include/engine/handle.h
    include/engine/engine.h
    include/engine/vertex.h
    include/engine/parts.h

    src/handle.cpp
    src/engine.cpp
    src/vertex.cpp
    src/parts.cpp)

target_include_directories(engine PUBLIC include)
target_link_libraries(engine PUBLIC fmt glad glfw box2d stb)
