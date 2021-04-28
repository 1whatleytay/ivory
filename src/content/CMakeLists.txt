add_library(content STATIC
    include/content/game.h
    include/content/player.h
    include/content/camera.h
    include/content/resources.h
    include/content/client.h
    include/content/assets.h
    include/content/font.h
    include/content/net-player.h
    include/content/options.h
    include/content/asset-loader.h
    include/content/map-loader.h
    include/content/flag.h
    include/content/map.h
    include/content/capture.h

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

target_include_directories(content PUBLIC include)
target_link_libraries(content PUBLIC engine shared asio CLI11 pugixml rapidjson freetype)
