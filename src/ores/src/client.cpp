#include <ores/client.h>

Message Client::read() {
    size_t length;

    Container container;
    length = socket.read_some(asio::buffer(reinterpret_cast<uint8_t *>(&container), sizeof(container)));
    assert(length == sizeof(container));

    std::vector<uint8_t> buffer(container.size);
    length = socket.read_some(asio::buffer(buffer));
    assert(length == container.size);

    return parse(container, buffer.data());
}

void Client::write(const Event &event) {
    Writer writer;
    event.write(writer);

    Container container;
    container.type = event.type();
    container.size = writer.data.size();

    const uint8_t *containerPtr = reinterpret_cast<const uint8_t *>(&container);

    std::vector<uint8_t> data;
    data.reserve(sizeof(container) + writer.data.size());
    data.insert(data.end(), containerPtr, containerPtr + sizeof(container));
    data.insert(data.end(), writer.data.begin(), writer.data.end());

    socket.async_write_some(asio::buffer(data), [](asio::error_code, std::size_t) { });
}

void Client::run() {
    while (!kill) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

Client::Client(Child *component, const std::string &url, const std::string &port)
    : Resource(component), context(), socket(context) {

    tcp::resolver resolver(context);
    auto endpoints = resolver.resolve(url, port);

    asio::connect(socket, endpoints);
    hello = std::get<messages::Hello>(read());
}