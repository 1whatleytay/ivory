#include <ores/player.h>

#include <ores/client.h>

bool Player::isTouchingGround() const {
    b2ContactEdge *edge = box->body->value->GetContactList();

    while (edge) {
        auto body = reinterpret_cast<parts::BoxBody *>(edge->other->GetUserData().pointer);

        if (body->isGround())
            return true;

        edge = edge->next;
    }

    return false;
}

void Player::update(float time) {
    if (glfwGetKey(engine.window, GLFW_KEY_A) == GLFW_PRESS)
        box->body->setVelocity(-5, std::nullopt);
    else if (glfwGetKey(engine.window, GLFW_KEY_D) == GLFW_PRESS)
        box->body->setVelocity(+5, std::nullopt);

    box->body->capVelocity(5, std::nullopt);

    if (client) {
        float timeToGo = netUpdates[netUpdateIndex % netUpdates.size()];

        netUpdateTime += time;
        if (netUpdateTime > timeToGo) {
            if (timeToGo == 0) {
                netUpdateTime = 0;
            } else {
                netUpdateTime = std::fmod(netUpdateTime, timeToGo);
            }

            auto pos = box->body->value->GetPosition();

            client->write(messages::Move {
                client->hello.playerId,

                pos.x, pos.y
            });
        }
    }
}

void Player::keyboard(int key, int action) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_R) {
            float x = client ? client->hello.playerX : 0;
            float y = client ? client->hello.playerY : 2;

            box->body->value->SetTransform(b2Vec2(x, y), 0);
            box->body->value->SetAwake(true);
        }

        if (key == GLFW_KEY_W && isTouchingGround())
            box->body->setVelocity(std::nullopt, 5);

        if (key == GLFW_KEY_U) {
            netUpdateIndex++;
        }
    }
}

Player::Player(Child *parent) : Child(parent) {
    client = find<Client>();

    float x = client ? client->hello.playerX : 0;
    float y = client ? client->hello.playerY : 0.5;

    box = make<parts::Box>(x, y, 1, 1, Color(0xFF0000u), 1);
}
