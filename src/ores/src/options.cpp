#include <ores/options.h>

#include <CLI/CLI.hpp>

Options::Options(int count, const char **args) {
    CLI::App app;

    auto multiplayerOpt = app.add_flag("-m,--multi", multiplayer, "Enable to start using multiplayer.");

    app.add_option("--address", address, "Multiplayer server address")->needs(multiplayerOpt);
    app.add_option("--port", port, "Multiplayer server port")->needs(multiplayerOpt);

    app.add_option("--width", worldWidth, "World width")->excludes(multiplayerOpt);
    app.add_option("--height", worldHeight, "World height")->excludes(multiplayerOpt);

    app.add_option("-a,--assets", assetsPath, "Path to assets");

    try {
        app.parse(count, args);
    } catch (const CLI::ParseError &e) {
        throw std::runtime_error(e.what());
    }
}

OptionsResource::OptionsResource(Child *component, const Options &options) : Resource(component), value(options) { }
