#include <renderer/texture/texture_fire.h>
#include <world/block/blocks.h>

#include <stdlib.h>
#include <string.h>

#define RAND_FLOAT ((double)rand() / (double)((unsigned)RAND_MAX + 1))

texture_animated_t texture_fire_create(int type) {
    texture_animated_t texture = texture_animated_create(blocks.fire.texture_id + (type << 4));

    texture.tick = texture_fire_tick;  

    return texture;
}

void texture_fire_tick(texture_animated_t *texture) {
    for(int x = 0; x < 16; x++) {
        for(int y = 0; y < 20; y++) {
            int a = 18;
            int b = texture->current_fire_frame[x + ((y + 1) % 20 << 4)] * 18.0;

            for(int xx = x - 1; xx <= x + 1; xx++) {
                for(int yy = y; yy <= y + 1; yy++) {
                    if(xx >= 0 && yy >= 0 && xx < 16 && yy < 20) {
                        b += texture->current_fire_frame[xx + (yy << 4)];
                    }
                    a++;
                }
            }

            texture->last_fire_frame[x + (y << 4)] = b / (a * 1.06);
            if(y >= 19) {
                texture->last_fire_frame[x + (y << 4)] = (RAND_FLOAT * RAND_FLOAT * RAND_FLOAT * 4.0 + RAND_FLOAT * 0.1 + 0.2);
            }
        }
    }

    float swap[320];
    memcpy(swap, texture->last_fire_frame, sizeof(float) * 320);
    memcpy(texture->last_fire_frame, texture->current_fire_frame, sizeof(float) * 320);
    memcpy(texture->current_fire_frame, swap, sizeof(float) * 320);

    for(int i = 0; i < 256; i++) {
        float pixel_intensity = texture->current_fire_frame[i] * 1.8;
        if(pixel_intensity > 1) pixel_intensity = 1;
        if(pixel_intensity < 0) pixel_intensity = 0;

        int b = pixel_intensity * 155 + 100;
        int c = pixel_intensity * pixel_intensity * 255;
        int d = pixel_intensity * pixel_intensity * pixel_intensity * pixel_intensity * pixel_intensity * pixel_intensity * pixel_intensity * pixel_intensity * pixel_intensity * pixel_intensity * 255;
        int e = 255;
        // Makes pixel transparent if not intense enough
        if(pixel_intensity < 0.5) e = 0;

        if(texture->anaglyph) {
            int bb = (b * 30 + c * 59 + d * 11) / 100;
            int cc = (b * 30 + c * 70) / 100;
            int dd = (b * 30 + d * 70) / 100;
            b = bb;
            c = cc;
            d = dd;
        }

        texture->data[i << 2] = (uint8_t)b;
        texture->data[(i << 2) + 1] = (uint8_t)c;
        texture->data[(i << 2) + 2] = (uint8_t)d;
        texture->data[(i << 2) + 3] = (uint8_t)e;
    }
}