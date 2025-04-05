#include <renderer/texture/texture_water.h>
#include <level/block/blocks.h>

#include <stdlib.h>

texture_animated_t texture_water_create() {
    texture_animated_t texture = texture_animated_create(blocks.water.texture_id);
    texture.tick = texture_water_tick;

    return texture;
}

void texture_water_tick(texture_animated_t *texture) {
    texture->updates++;
    
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            float v = 0.0;
            for (int i = x - 1; i <= x + 1; i++) {
                int a = i & 15;
                int b = y & 15;
                v += texture->red[a + (b << 4)];
            }
            texture->blue[x + (y << 4)] = v / 3.3 + texture->green[x + (y << 4)] * 0.8;
        }
    }
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            texture->green[x + (y << 4)] += texture->alpha[x + (y << 4)] * 0.05;
            if (texture->green[x + (y << 4)] < 0.0) { texture->green[x + (y << 4)] = 0.0; }
            texture->alpha[x + (y << 4)] -= 0.1;
            if (((float)rand() / RAND_MAX) < 0.05) { texture->alpha[x + (y << 4)] = 0.5; }
        }
    }
    
    for (int i = 0; i < 256; i++) {
        float blue = texture->blue[i];
        texture->blue[i] = texture->red[i];
        texture->red[i] = blue;
        float v = texture->red[i];
        if (v > 1.0) { v = 1.0; }
        if (v < 0.0) { v = 0.0; }
        int a = 32.0 + v * v * 32.0;
        int b = 50.0 + v * v * 64.0;
        int c = 255;
        int d = 146.0 + v * v * 50.0;
        if (texture->anaglyph) {
            int aa = (a * 30 + b * 59 + c * 11) / 100;
            int bb = (a * 30 + b * 70) / 100;
            int cc = (a * 30 + c * 70) / 100;
            a = aa;
            b = bb;
            c = cc;
        }
        texture->data[i << 2] = a;
        texture->data[(i << 2) + 1] = b;
        texture->data[(i << 2) + 2] = c;
        texture->data[(i << 2) + 3] = d;
    }
}