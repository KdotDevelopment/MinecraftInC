#pragma once

typedef struct painting_s {
    char *title;
    int size_x;
    int size_y;
    int offset_x;
    int offset_y;
} painting_t;

const int painting_count = 19;

painting_t painting_list[] = {
    {
        .title = "Kebab",
        .size_x = 16,
        .size_y = 16,
        .offset_x = 0,
        .offset_y = 0
    },

    {
        .title = "Aztec",
        .size_x = 16,
        .size_y = 16,
        .offset_x = 16,
        .offset_y = 0
    },

    {
        .title = "Alban",
        .size_x = 16,
        .size_y = 16,
        .offset_x = 32,
        .offset_y = 0
    },

    {
        .title = "Aztec2",
        .size_x = 16,
        .size_y = 16,
        .offset_x = 48,
        .offset_y = 0
    },

    {
        .title = "Bomb",
        .size_x = 16,
        .size_y = 16,
        .offset_x = 64,
        .offset_y = 0
    },

    {
        .title = "Plant",
        .size_x = 16,
        .size_y = 16,
        .offset_x = 80,
        .offset_y = 0
    },

    {
        .title = "Wasteland",
        .size_x = 16,
        .size_y = 16,
        .offset_x = 96,
        .offset_y = 0
    },

    {
        .title = "Pool",
        .size_x = 32,
        .size_y = 16,
        .offset_x = 0,
        .offset_y = 32
    },

    {
        .title = "Courbet",
        .size_x = 32,
        .size_y = 16,
        .offset_x = 32,
        .offset_y = 32
    },

    {
        .title = "Sea",
        .size_x = 32,
        .size_y = 16,
        .offset_x = 64,
        .offset_y = 32
    },

    {
        .title = "Sunset",
        .size_x = 32,
        .size_y = 16,
        .offset_x = 96,
        .offset_y = 32
    },

    {
        .title = "Wanderer",
        .size_x = 16,
        .size_y = 32,
        .offset_x = 0,
        .offset_y = 64
    },

    {
        .title = "Match",
        .size_x = 32,
        .size_y = 32,
        .offset_x = 0,
        .offset_y = 128
    },

    {
        .title = "Bust",
        .size_x = 32,
        .size_y = 32,
        .offset_x = 32,
        .offset_y = 128
    },

    {
        .title = "Stage",
        .size_x = 32,
        .size_y = 32,
        .offset_x = 64,
        .offset_y = 128
    },

    {
        .title = "Void",
        .size_x = 32,
        .size_y = 32,
        .offset_x = 96,
        .offset_y = 128
    },

    {
        .title = "SkullAndRoses",
        .size_x = 32,
        .size_y = 32,
        .offset_x = 128,
        .offset_y = 128
    },

    {
        .title = "Fighters",
        .size_x = 64,
        .size_y = 32,
        .offset_x = 0,
        .offset_y = 96
    },

    {
        .title = "Pointer",
        .size_x = 64,
        .size_y = 64,
        .offset_x = 0,
        .offset_y = 192
    }
};