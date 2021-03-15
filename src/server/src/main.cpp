#include <server/server.h>

int main(int count, const char **args) {
    Options options(count, args);

    std::make_unique<Server>(options)->run();
}