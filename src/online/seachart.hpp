#pragma once

#include "common_structs.h"

namespace paperpiracy {

class Seachart {
    static constexpr f32 width = 96.0f;
    static constexpr f32 height = 64.0f;
    static constexpr f32 minDist = 8.0f; // Minimum distance between islands
    static constexpr f32 clusterStrength = 8.0f; // How much islands are influenced by perlin noise

    u32 seed;

    // We have our own rand function so generation can be easily reproduced over the network
    f32 rand_float();

    bool is_valid_island_pos(VecXZf pos, size_t numIslands = ARRAY_COUNT(islands));

    void generate();

public:
    VecXZf hub;
    VecXZf islands[24];

    Seachart(u32 seed) : seed(seed) {
        generate();
    }

    void appendGfx();
};

}; // namespace paperpiracy
