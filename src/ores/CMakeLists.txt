add_library(ores STATIC
    include/ores/game.h
    include/ores/player.h
    include/ores/camera.h
    include/ores/resources.h
    include/ores/client.h
    include/ores/assets.h
    include/ores/font.h
    include/ores/net-player.h
    include/ores/options.h
    include/ores/asset-loader.h
    include/ores/map-loader.h
    include/ores/flag.h
    include/ores/map.h
    include/ores/capture.h

    src/game.cpp
    src/flag.cpp
    src/player.cpp
    src/client.cpp
    src/camera.cpp
    src/assets.cpp
    src/font.cpp
    src/net-player.cpp
    src/options.cpp
    src/asset-loader.cpp
    src/map-loader.cpp
    src/resources.cpp
    src/map.cpp
    src/capture.cpp)

target_include_directories(ores PUBLIC include)
target_link_libraries(ores PUBLIC engine shared asio CLI11 pugixml rapidjson freetype)
