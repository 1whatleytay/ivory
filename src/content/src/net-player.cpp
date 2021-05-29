#include <content/net-player.h>

#include <content/client.h>

#include <fmt/printf.h>

#include <unordered_map>

void NetPlayer::handle(const messages::Move &event) {
    body->value->SetTransform(b2Vec2(event.x, event.y), 0);
    body->value->SetLinearVelocity(b2Vec2(event.velocityX, event.velocityY));
    body->value->SetAwake(true);

    std::unordered_map<std::string, TagInfo *> tags;

    auto add = [&tags](TagInfo &tag) {
        tags.insert({ tag.name, &tag });
    };

    assert(false);

    auto iterator = tags.find(event.animation);
    if (iterator != tags.end())
        doAnimation(*iterator->second);
    else
        assert(false);
}

void NetPlayer::click(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        auto cursor = engine.cursor();

        auto pos = body->value->GetPosition();

        if (cursor.x >= pos.x - visualWidth / 2 && cursor.x <= pos.x + visualWidth / 2
            && cursor.y >= pos.y && cursor.y <= pos.y + visualHeight) {
            clicks--;

            fmt::print("Clicking: {}, clicks left: {}\n", playerId, clicks);

            if (clicks <= 0) {
                client->write(messages::SetHealth {
                    playerId,

                    true
                });

                clicks = 20;
            }
        }
    }
}

NetPlayer::NetPlayer(Child *parent, int64_t playerId, float x, float y) : PlayerBase(parent, x, y), playerId(playerId) {
    client = find<Client>();

    assert(client);
}
