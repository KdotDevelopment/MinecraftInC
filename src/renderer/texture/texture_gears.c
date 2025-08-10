#include <renderer/texture/texture_gears.h>
#include <level/block/blocks.h>

#include <stdlib.h>

texture_animated_t texture_gears_create(int rotation) {
    texture_animated_t texture = texture_animated_create(blocks.gears.texture_id);

    texture.gear_rotation_dir = (rotation << 1) - 1;
    texture.gear_rotation = 2;

    //load gear.png and gearmiddle.png into respective arrays

    texture.tick = texture_gears_tick;  

    return texture;
}

void texture_gears_tick(texture_animated_t *texture) {
    texture->gear_rotation += texture->gear_rotation_dir & 63;
    float ss = tsin(texture->gear_rotation / 64.0 * M_PI * 2);
    float cc = tcos(texture->gear_rotation / 64.0 * M_PI * 2);

    for(int x = 0; x < 16; x++) {
        for(int y = 0; y < 16; y++) {
            float a = (x / 15.0 - 0.5) * 31.0;
            float b = (y / 15.0 - 0.5) * 31.0;
            float c = cc * a - ss * b;
            a = ss * b + cc * a;
            uint16_t d = c + 16;
            uint16_t e = a + 16;
            uint16_t f = 0;
            if(d >= 0 && e >= 0 && d < 32 && e < 32) {
                f = texture->gear_color[d + (e << 5)];
                e = texture->gear_middle_color[x + (y << 4)];
                if(e >> 24 > 128) {
                    f = e;
                }
            }

            e = f >> 16 & 255;
            d = f >> 8 & 255;
            uint16_t g = f & 255;
            f = f >> 24 > 128 ? 255 : 0;
            uint16_t h = x + (y << 4);
            texture->data[h << 2] = (uint8_t)e;
            texture->data[(h << 2) + 1] = (uint8_t)d;
            texture->data[(h << 2) + 2] = (uint8_t)g;
            texture->data[(h << 2) + 3] = (uint8_t)f;
        }
    }
}