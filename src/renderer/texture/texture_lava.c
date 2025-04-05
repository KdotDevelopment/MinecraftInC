#include <renderer/texture/texture_lava.h>
#include <level/block/blocks.h>

#include <stdlib.h>
#include <math.h>

texture_animated_t texture_lava_create() {
    texture_animated_t texture = texture_animated_create(blocks.lava.texture_id);
    texture.tick = texture_lava_tick;

    return texture;
}

void texture_lava_tick(texture_animated_t *texture) {
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            int sy = tsin(y * M_PI / 8.0) * 1.2;
            int sx = tsin(x * M_PI / 8.0) * 1.2;
            float v = 0.0;
            for (int i = x - 1; i <= x + 1; i++) {
                for (int j = y - 1; j <= y + 1; j++) {
                    int a = (i + sy) & 15;
                    int b = (j + sx) & 15;
                    v += texture->red[a + (b << 4)];
                }
            }
            texture->green[x + (y << 4)] = v / 10.0 + (texture->blue[(x & 15) + ((y & 15) << 4)] + texture->blue[((x + 1) & 15) + ((y & 15) << 4)] + texture->blue[((x + 1) & 15) + (((y + 1) & 15) << 4)] + texture->blue[(x & 15) + (((y + 1) & 15) << 4)]) / 4.0 * 0.8;
            texture->blue[x + (y << 4)] += texture->alpha[x + (y << 4)] * 0.01;
            if (texture->blue[x + (y << 4)] < 0.0) { texture->blue[x + (y << 4)] = 0.0; }
            texture->alpha[x + (y << 4)] -= 0.06;
            if (((float)rand() / RAND_MAX) < 0.005) { texture->alpha[x + (y << 4)] = 1.5; }
        }
    }
    
    for (int i = 0; i < 256; i++) {
        float red = texture->red[i];
        texture->red[i] = texture->green[i];
        texture->green[i] = red;
        float v = texture->red[i] * 2.0;
        if (v > 1.0) { v = 1.0; }
        if (v < 0.0) { v = 0.0; }
        int a = v * 100.0 + 155.0;
        int b = v * v * 255.0;
        int c = v * v * v * v * 128.0;
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
        texture->data[(i << 2) + 3] = -1;
    }
}