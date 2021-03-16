#include <ores/net-player.h>

void NetPlayer::update(float time) {
    if (visual)
        visual->set(x, y, 1, 1, range);
}

NetPlayer::NetPlayer(Child *parent, size_t playerId)
    : Child(parent), playerId(playerId),
    range(std::move(supply<parts::Texture>(1, 1)->grab(1, 1).write(Color(0x0000FF).data().data()))) {

    visual = make<parts::BoxVisual>();
}
