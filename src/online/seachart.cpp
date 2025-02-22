#include "seachart.hpp"

#include "macros.h"
#include "ultra64.h"
#include "functions.h"
#include "ld_addrs.h"
#include "online/seachart/background.png.h"
#include "online/seachart/islands.png.h"
#include "online/seachart/hub.png.h"

namespace paperpiracy::noise {
    // Fade function for Perlin noise
    static float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    // Linear interpolation
    static float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    // Pseudo-random gradient
    static float grad(int hash, float x, float y) {
        int h = hash & 3;
        float u = h < 2 ? x : y;
        float v = h < 2 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
    }

    // Perlin noise function
    float perlin(float x, float y) {
        int X = (int)x & 255;
        int Y = (int)y & 255;
        x -= (int)x;
        y -= (int)y;
        float u = fade(x);
        float v = fade(y);
        int hash = (X * 57 + Y * 131) & 255;
        return lerp(lerp(grad(hash, x, y), grad(hash + 1, x - 1, y), u),
                    lerp(grad(hash + 57, x, y - 1), grad(hash + 58, x - 1, y - 1), u),
                    v);
    }
};

using namespace paperpiracy;

float Seachart::rand_float() {
    seed = (214013 * seed + 2531011);
    return (seed >> 16) / 65535.0f;
}

// Checks if a new point is sufficiently far from existing points
bool Seachart::is_valid_island_pos(VecXZf pos, size_t numIslands) {
    // Point must be far away from the edges
    if (pos.x < minDist || pos.x > width - minDist || pos.z < minDist || pos.z > height - minDist) {
        return false;
    }

    // Point must be far away from other islands
    for (size_t i = 0; i < numIslands; i++) {
        // Ignore the island we're checking against
        if (islands[i].x == pos.x && islands[i].z == pos.z) {
            continue;
        }

        f32 dx = islands[i].x - pos.x;
        f32 dz = islands[i].z - pos.z;
        if (dx * dx + dz * dz < minDist * minDist) {
            return false;
        }
    }

    // Point must be far from the hub
    f32 dx = hub.x - pos.x;
    f32 dz = hub.z - pos.z;
    constexpr f32 minDistFromHub = minDist * 2.0f;
    return dx * dx + dz * dz >= minDistFromHub * minDistFromHub;
}

void Seachart::generate() {
    hub.x = width / 2.0f;
    hub.z = height / 2.0f;

    size_t numIslands = 0;
    while (numIslands < ARRAY_COUNT(islands)){
        VecXZf pos = { rand_float() * width, rand_float() * height };
        if (is_valid_island_pos(pos, numIslands)) {
            islands[numIslands++] = pos;
        }
    }

    // Bias using noise, to cluster islands
    f32 seed = rand_float() * 100.0f;
    for (auto& island : islands) {
        island.x += noise::perlin(seed + (island.x * 0.0625f), seed + (island.z * 0.0625f)) * clusterStrength - (clusterStrength / 2.0f);
        island.z += noise::perlin(seed + (island.x * 0.0625f), seed + (island.z * 0.0625f)) * clusterStrength - (clusterStrength / 2.0f);
    }

    // Reposition any badly-placed islands
    for (auto& island : islands) {
        while (!is_valid_island_pos(island)) {
            island = { -1000.0f, -1000.0f }; // So it's not considered in is_valid_island_pos

            VecXZf candidate = { rand_float() * width, rand_float() * height };
            candidate.x += noise::perlin(seed + (candidate.x * 0.0625f), seed + (candidate.z * 0.0625f)) * clusterStrength - (clusterStrength / 2.0f);
            candidate.z += noise::perlin(seed + (candidate.x * 0.0625f), seed + (candidate.z * 0.0625f)) * clusterStrength - (clusterStrength / 2.0f);
            if (is_valid_island_pos(candidate)) {
                island = candidate;
                break;
            }
        }
    }
}

void Seachart::appendGfx() {
    draw_ci_image_with_clipping(
        (IMG_PTR)online_seachart_background_png,
        online_seachart_background_png_width, online_seachart_background_png_height,
        G_IM_FMT_CI, G_IM_SIZ_8b,
        (PAL_PTR)(void*)online_seachart_background_pal,
        0, 0,
        0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
        255
    );

    constexpr s16 mapWidth = width * 3;
    constexpr s16 mapHeight = height * 3;
    constexpr s16 mapX = (SCREEN_WIDTH - mapWidth) / 2;
    constexpr s16 mapY = (SCREEN_HEIGHT - mapHeight) / 2;

    /*startup_draw_prim_rect(mapX, mapY, mapX + mapWidth, mapY + mapHeight, 0, 0, 0, 20);*/

    // Draw islands
    constexpr s16 islandWidth = 16;
    constexpr s16 islandHeight = 16;
    for (auto& island : islands) {
        s16 centerX = ((island.x / width) * mapWidth) + mapX;
        s16 centerY = ((island.z / height) * mapHeight) + mapY;
        s16 ulX = centerX - (islandWidth / 2);
        s16 ulY = centerY - (islandHeight / 2);

        draw_ci_image_with_clipping(
            (IMG_PTR)online_seachart_islands_png,
            online_seachart_islands_png_width, online_seachart_islands_png_height,
            G_IM_FMT_CI, G_IM_SIZ_4b,
            (PAL_PTR)(void*)online_seachart_islands_pal,
            ulX, ulY,
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
            255
        );
    }

    // Draw hub
    {
        s16 centerX = ((hub.x / width) * mapWidth) + mapX;
        s16 centerY = ((hub.z / height) * mapHeight) + mapY;
        s16 ulX = centerX - (islandWidth / 2);
        s16 ulY = centerY - (islandHeight / 2);

        draw_ci_image_with_clipping(
            (IMG_PTR)online_seachart_hub_png,
            online_seachart_hub_png_width, online_seachart_hub_png_height,
            G_IM_FMT_CI, G_IM_SIZ_4b,
            (PAL_PTR)(void*)online_seachart_hub_pal,
            ulX, ulY,
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
            255
        );
    }
}
