
#pragma once

#include "Math/Vector.h"
#include "Math/Quad.h"

namespace game
{
    struct PlayerInfo
    {
        math::Vector position;
        int ammunition;
    };

    extern PlayerInfo player_one;
    extern PlayerInfo player_two;

    extern math::Quad camera_viewport;
}
