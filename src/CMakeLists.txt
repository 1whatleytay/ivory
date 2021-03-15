add_subdirectory(ores)
add_subdirectory(engine)
add_subdirectory(server)
add_subdirectory(shared)

add_executable(game main.cpp)
target_link_libraries(game PRIVATE ores)
