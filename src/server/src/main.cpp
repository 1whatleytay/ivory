#include <server/server.h>

#include <fmt/printf.h>

int main(int count, const char **args) {
    Options options(count, args);

    try {
        std::make_unique<Server>(options)->run();
    } catch (const std::exception &e) {
        fmt::print("FUCK {}", e.what());
    }
}