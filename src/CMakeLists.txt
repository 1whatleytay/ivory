add_subdirectory(ores)
add_subdirectory(engine)

add_executable(game main.cpp)
target_link_libraries(game PRIVATE ores)
