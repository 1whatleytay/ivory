add_library(shared STATIC
    include/shared/buffer.h
    include/shared/messages.h

    src/buffer.cpp
    src/messages.cpp)

target_include_directories(shared PUBLIC include)
target_link_libraries(shared PUBLIC stb fmt)
