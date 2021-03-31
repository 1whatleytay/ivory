#include <ores/options.h>

#include <CLI/CLI.hpp>

Options::Options(int count, const char **args) {
    CLI::App app;

    auto multiplayerOpt = app.add_flag("-m,--multi", multiplayer, "Multiplayer mode");

    app.add_option("--address", address, "Multiplayer server address")->needs(multiplayerOpt);
    app.add_option("--port", port, "Multiplayer server port")->needs(multiplayerOpt);

    app.add_option("--width", worldWidth, "World width")->excludes(multiplayerOpt);
    app.add_option("--height", worldHeight, "World height")->excludes(multiplayerOpt);

    app.add_option("-a,--assets", assetsPath, "Path to assets");

    app.add_option("--map", map, "Map file name")->excludes(multiplayerOpt);

    try {
        app.parse(count, args);
    } catch (const CLI::ParseError &e) {
        throw std::runtime_error(e.what());
    }
}
