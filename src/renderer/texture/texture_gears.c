#include <renderer/texture/texture_gears.h>
#include <world/block/blocks.h>

#include <assets/misc/gear.h>
#include <assets/misc/gearmiddle.h>

#include <stdlib.h>
#include <string.h>

texture_animated_t texture_gears_create(int rotation) {
    texture_animated_t texture = texture_animated_create(blocks.gears.texture_id);

    texture.gear_rotation_dir = (rotation << 1) - 1;
    texture.gear_rotation = 2;

    //load gear.png and gearmiddle.png into respective arrays
    memcpy(&texture.gear_color, asset_gear_rgba, 1024);
    memcpy(&texture.gear_middle_color, asset_gearmiddle_rgba, 1024);

    texture.tick = texture_gears_tick;  

    return texture;
}

void texture_gears_tick(texture_animated_t *texture) {
    texture->gear_rotation = (texture->gear_rotation + texture->gear_rotation_dir) & 63;

    float ss = tsin(texture->gear_rotation / 64.0f * M_PI * 2.0f);
    float cc = tcos(texture->gear_rotation / 64.0f * M_PI * 2.0f);

    for(int x = 0; x < 16; x++) {
        for(int y = 0; y < 16; y++) {
            float a = ((float)x / 15.0f - 0.5f) * 31.0f;
            float b = ((float)y / 15.0f - 0.5f) * 31.0f;

            float c = cc * a - ss * b;
            a = cc * b + ss * a;

            int d = (int)(c + 16.0f);
            int e = (int)(a + 16.0f);
            uint32_t color = 0;

            if(d >= 0 && e >= 0 && d < 32 && e < 32) {
                color = texture->gear_color[d + (e << 5)];
                uint32_t middle = texture->gear_middle_color[x + (y << 4)];
                if((middle >> 24) > 128) color = middle;
            }

            uint8_t r = (color >> 16) & 255;
            uint8_t g = (color >> 8) & 255;
            uint8_t b2 = color & 255;
            uint8_t a2 = ((color >> 24) > 128) ? 255 : 0;

            int idx = (x + (y << 4)) << 2;
            texture->data[idx + 0] = r;
            texture->data[idx + 1] = g;
            texture->data[idx + 2] = b2;
            texture->data[idx + 3] = a2;
        }
    }
}