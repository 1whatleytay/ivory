add_executable(server
    include/server/server.h
    include/server/options.h

    src/main.cpp
    src/server.cpp
    src/options.cpp)

target_include_directories(server PRIVATE include)
target_link_libraries(server PUBLIC shared CLI11 asio)
