#include <ores/player.h>

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
}

void Player::keyboard(int key, int action) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_R) {
            box->body->value->SetTransform(b2Vec2(0, 0.5), 0);
            box->body->value->SetAwake(true);
        }

        if (key == GLFW_KEY_W && isTouchingGround())
            box->body->setVelocity(std::nullopt, 5);
    }
}

Player::Player(Child *parent, float x, float y) : Child(parent) {
    box = make<parts::Box>(x, y, 1, 1, Color(0xFF0000u), 1);
}
