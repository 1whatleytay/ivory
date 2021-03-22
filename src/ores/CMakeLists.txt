add_library(ores STATIC
    include/ores/game.h
    include/ores/player.h
    include/ores/camera.h
    include/ores/background.h
    include/ores/client.h
    include/ores/assets.h
    include/ores/net-player.h
    include/ores/options.h
    include/ores/asset-loader.h
    include/ores/map-loader.h
    include/ores/map.h

    src/game.cpp
    src/player.cpp
    src/client.cpp
    src/camera.cpp
    src/background.cpp
    src/assets.cpp
    src/net-player.cpp
    src/options.cpp
    src/asset-loader.cpp
    src/map-loader.cpp
    src/map.cpp)

target_include_directories(ores PUBLIC include)
target_link_libraries(ores PUBLIC engine shared asio CLI11 pugixml rapidjson)
