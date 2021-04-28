#include <content/net-player.h>

std::pair<float, float> NetPlayer::flagPosition() {
    return { x, y };
}

void NetPlayer::update(float time) {
    if (visual)
        visual->set(x, y, 1, 1, *range);
}

NetPlayer::NetPlayer(Child *parent, size_t playerId) : Child(parent), playerId(playerId) {
    range = supply<parts::Texture>(1, 1)->grab(1, 1, Color(0x0000FF).data().data());

    visual = make<parts::BoxVisual>();
}
