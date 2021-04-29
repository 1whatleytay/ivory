#include <content/client.h>

#include <fmt/printf.h>

void Client::write(const Event &event) {
    Writer writer;
    event.write(writer);

    size_t fullSize = sizeof(Container) + writer.data.size();
    auto data = std::make_unique<uint8_t[]>(fullSize);
    *reinterpret_cast<Container *>(data.get()) = { event.type(), writer.data.size() };
    std::memcpy(&data[sizeof(Container)], writer.data.data(), writer.data.size());

    auto* ptr = data.get();

    asio::async_write(socket, asio::buffer(ptr, fullSize), [fullSize, data { std::move(data) }](
        asio::error_code ec, std::size_t n
    ) {
        if (ec == asio::error::eof || n != fullSize)
            throw std::runtime_error("Server disconnected.");
    });
}

void Client::handle(const Message &message) {
    if (auto *h = std::get_if<messages::Hello>(&message)) {
        hello = *h;
        hasHello = true;
    }

    std::lock_guard guard(messagesMutex);
    messages.push_back(message);
}

void Client::listenBody(const Container &container) {
    auto body = std::make_unique<uint8_t[]>(container.size);
    auto *ptr = body.get();

    auto respond = [this, container, body = std::move(body)](asio::error_code e, size_t n) {
        if (e == asio::error::eof || n != container.size)
            throw std::runtime_error("Server disconnected.");

        handle(parse(container, body.get()));

        listen();
    };

    asio::async_read(socket, asio::buffer(ptr, container.size), std::move(respond));
}

void Client::listen() {
    auto container = std::make_unique<Container>();
    auto *ptr = reinterpret_cast<uint8_t *>(container.get());

    auto respond = [this, container = std::move(container)](asio::error_code e, size_t n) {
        if (e == asio::error::eof || n != sizeof(Container))
            throw std::runtime_error("Server disconnected.");
        
        listenBody(*container);
    };

    asio::async_read(socket, asio::buffer(ptr, sizeof(Container)), std::move(respond));
}

void Client::run() {
    listen();

    write(messages::Log { "Taylor :D" });

    context.run();
}

Client::Client(Child *component, const std::string &url, const std::string &port)
    : Resource(component), context(), socket(context) {

    tcp::resolver resolver(context);
    auto endpoints = resolver.resolve(url, port);

    asio::connect(socket, endpoints);
}