add_library(ores STATIC
    include/ores/game.h
    include/ores/player.h
    include/ores/world.h
    include/ores/camera.h
    include/ores/background.h
    include/ores/client.h
    include/ores/assets.h
include/ores/net-player.h

    src/game.cpp
    src/player.cpp
    src/world.cpp
    src/client.cpp
    src/camera.cpp
    src/background.cpp
    src/assets.cpp
src/net-player.cpp)

target_include_directories(ores PUBLIC include)
target_link_libraries(ores PUBLIC engine shared asio)
