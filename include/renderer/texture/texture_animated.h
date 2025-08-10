#pragma once

#include <stdint.h>

enum {
    TEXTURE_ANIMATED_NONE,
    TEXTURE_ANIMATED_WATER,
    TEXTURE_ANIMATED_LAVA,
};

typedef struct texture_animated_s {
    uint8_t data[1024];
    int texture_id;
    uint8_t anaglyph;

    union {
        // Water/lava
        struct {
            float red[256];
            float green[256];
            float blue[256];
            float alpha[256];

            int updates;
        };
        // Gears
        struct {
            int gear_rotation;
            int gear_color[1024];
            int gear_middle_color[1024];
            int gear_rotation_dir;
        };
        // Fire
        struct {
            float current_fire_frame[320];
            float last_fire_frame[320];
        };
    };

    void (*tick)(struct texture_animated_s *texture);
} texture_animated_t;

texture_animated_t texture_animated_create(int texture_id);
void texture_animated_tick(texture_animated_t *texture);