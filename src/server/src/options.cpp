#include <server/options.h>

#include <CLI/CLI.hpp>

Options::Options(int count, const char **args) {
    CLI::App app;

    app.add_flag("--no-break", noBreak, "Whether blocks should be breakable");

    app.add_option("-p,--port", port, "Server hosting port");
    app.add_option("--width", worldWidth, "World generation width");
    app.add_option("--height", worldHeight, "World generation height");

    try {
        app.parse(count, args);
    } catch (const CLI::ParseError &e) {
        throw std::runtime_error(e.what());
    }
}