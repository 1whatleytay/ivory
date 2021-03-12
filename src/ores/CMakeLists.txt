add_library(ores STATIC
    include/ores/game.h
    include/ores/player.h
    include/ores/world.h
    include/ores/camera.h

    src/game.cpp
    src/player.cpp
    src/world.cpp
    src/camera.cpp)

target_include_directories(ores PUBLIC include)
target_link_libraries(ores PUBLIC engine)
