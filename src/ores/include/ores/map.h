#pragma once

#include <engine/parts.h>

struct Map : public Child {
    Map(Child *parent, const std::string &path);
};
