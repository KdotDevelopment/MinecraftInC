#include <level/levelgen/level_gen.h>
#include <level/block/blocks.h>
#include <level/level.h>
#include <level/levelgen/noise/noise.h>
#include <level/levelgen/noise/noise_octave.h>
#include <level/levelgen/noise/noise_composite.h>

#include <util/array_list.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

level_gen_t level_gen_create() {
    level_gen_t level_gen;
    level_gen.flood_data = calloc(1024 * 1024, sizeof(int));
    level_gen.random = random_create(time(NULL));
    return level_gen;
}

void level_gen_populate_ore(level_gen_t *level_gen, uint8_t ore_id, int a1, int a2, int a3) {
    int w = level_gen->width;
    int h = level_gen->height;
    int d = level_gen->depth;
    int ii = w * d * h / 256 / 64 * a1 / 100;
    for(int i = 0; i < ii; i++) {
        //Progress Bar ++
        float x1 = random_next_uniform(&level_gen->random) * w;
        float y1 = random_next_uniform(&level_gen->random) * h;
        float z1 = random_next_uniform(&level_gen->random) * d;
        int jj = (random_next_uniform(&level_gen->random) + random_next_uniform(&level_gen->random)) * 75 * a1 / 100;
        float r1x = random_next_uniform(&level_gen->random) * 2 * M_PI;
        float r1y = random_next_uniform(&level_gen->random) * 2 * M_PI;
        float r2x = 0;
        float r2y = 0;
        for(int j = 0; j < jj; j++) {
            x1 += tsin(r1x) * tcos(r1y);
            y1 += tsin(r1y);
            z1 += tcos(r1x) * tcos(r1y);
            r1x += r2x * 0.2;
            r2x = r2x * 0.9 + random_next_uniform(&level_gen->random) - random_next_uniform(&level_gen->random);
            r1y = (r1y + r2y * 0.5) * 0.5;
            r2y = r2y * 0.9 + random_next_uniform(&level_gen->random) - random_next_uniform(&level_gen->random);
            float v2 = tsin(j * M_PI / jj) * a1 / 100 + 1;
            for(int x = x1 - v2; x <= (int)(x1 + v2); x++) {
                for(int y = y1 - v2; y <= (int)(y1 + v2); y++) {
                    for(int z = z1 - v2; z <= (int)(z1 + v2); z++) {
                        float x2 = x - x1;
                        float y2 = y - y1;
                        float z2 = z - z1;
                        if (x2 * x2 + 2.0 * y2 * y2 + z2 * z2 < v2 * v2 && x >= 1 && y >= 1 && z >= 1 && x < w - 1 && y < h - 1 && z < d - 1) {
                            int c = (y * d + z) * w + x;
                            if (level_gen->blocks[c] == blocks.stone.id) level_gen->blocks[c] = ore_id;
                        }
                    }
                }
            }
        }
    }
}

int64_t level_gen_flood(level_gen_t *level_gen, int x, int y, int z, uint8_t block_id) {
    int **flood_stack = array_list_create(sizeof(int *));

    int xx = 1;
    int zz = 1;
    for(xx = 1; 1 << zz < level_gen->width; zz++);
    while(1 << xx < level_gen->depth) xx++;
    int d = level_gen->depth - 1;
    int w = level_gen->width - 1;
    level_gen->flood_data[0] = (((y << xx) + z) << zz) + x;
    int i = 1;
    int64_t j = 0;
    int ii = level_gen->width * level_gen->depth;
    while(i > 0) {
        i--;
        int f = level_gen->flood_data[i];
        if(i == 0 && array_list_length(flood_stack) > 0) {
            free(level_gen->flood_data);
            level_gen->flood_data = flood_stack[array_list_length(flood_stack) - 1];
            flood_stack = array_list_remove(flood_stack, array_list_length(flood_stack) - 1);
        }
        int v1 = f >> zz & d;
        int v2 = f >> (zz + xx);
        int v3 = f & w;
        int v4 = f & w;
        for(; v3 > 0 && level_gen->blocks[f - 1] == 0; f--) v3--;
        while(v4 < level_gen->width && level_gen->blocks[f + v4 - v3] == 0) v4++;
        int v5 = f >> zz & d;
        int v6 = f >> (zz + xx);
        if(v5 != v1 || v6 != v2) printf("Diagonal flood?!\n");

        uint8_t b1 = 0;
        uint8_t b2 = 0;
        uint8_t b3 = 0;
        j += v4 - v3;
        for(; v3 < v4; v3++) {
            level_gen->blocks[f] = block_id;
            if(v1 > 0) {
                uint8_t b4 = level_gen->blocks[f - level_gen->width] == 0;
                if(b4 && !b1) {
                    if(i == 1024 * 1024) {
                        flood_stack = array_list_push(flood_stack, &level_gen->flood_data);
                        level_gen->flood_data = calloc(1024 * 1024, sizeof(int));
                        i = 0;
                    }
                    level_gen->flood_data[i++] = f - level_gen->width;
                }
                b1 = b4;
            }
            if(v1 < level_gen->depth - 1) {
                uint8_t b4 = level_gen->blocks[f + level_gen->width] == 0;
                if(b4 && !b2) {
                    if(i == 1024 * 1024) {
                        flood_stack = array_list_push(flood_stack, &level_gen->flood_data);
                        level_gen->flood_data = calloc(1024 * 1024, sizeof(int));
                        i = 0;
                    }
                    level_gen->flood_data[i++] = f + level_gen->width;
                }
                b2 = b4;
            }
            if(v2 > 0) {
                uint8_t b = level_gen->blocks[f - ii];
                if((block_id == blocks.lava.id || block_id == blocks.still_lava.id) && (b == blocks.water.id || b == blocks.still_water.id)) {
                    level_gen->blocks[f - ii] = blocks.stone.id;
                }
                uint8_t b4 = b == 0;
                if(b4 && !b3) {
                    if(i == 1024 * 1024) {
                        flood_stack = array_list_push(flood_stack, &level_gen->flood_data);
                        level_gen->flood_data = calloc(1024 * 1024, sizeof(int));
                        i = 0;
                    }
                    level_gen->flood_data[i++] = f - ii;
                }
                b3 = b4;
            }
            f++;
        }
    }
    for(int i = 0; i < array_list_length(flood_stack); i++) {
        free(flood_stack[i]);
    }
    array_list_free(flood_stack);
    return j;
}

void level_gen_generate(level_gen_t *level_gen, int width, int depth, struct level_s *proto_level) {
    level_t *level = (level_t *)proto_level;
    level_gen->progress_bar = level->progress_bar;
    progress_bar_set_title(level_gen->progress_bar, "Generating level");
    level_gen->width = width;
    level_gen->depth = depth;
    level_gen->height = 64;
    level_gen->water_level = 32;
    level_gen->blocks = calloc(width * depth * 64, 1);
    int w = level_gen->width;
    int h = level_gen->height;
    int d = level_gen->depth;

    progress_bar_set_text(level_gen->progress_bar, "Raising...");
    printf("Raising...\n");
    noise_t oct[4], n1, n2, n3;
    for(int i = 0; i < sizeof(oct) / sizeof(oct[0]); i++) {
        oct[i] = noise_octave_create(&level_gen->random, 8);
    }

    n1 = noise_composite_create(&oct[0], &oct[1]);
    n2 = noise_composite_create(&oct[2], &oct[3]);
    n3 = noise_octave_create(&level_gen->random, 6);
    int *height_map = calloc(w * d, sizeof(int));
    for(int x = 0; x < w; x++) {
        progress_bar_set_progress(level_gen->progress_bar, x * 100 / (w - 1));
        for(int y = 0; y < d; y++) {
            float v1 = noise_get(&n1, x * 1.3, y * 1.3) / 6 - 4;
            float v2 = noise_get(&n2, x * 1.3, y * 1.3) / 5 + 6;
            if(noise_get(&n3, x, y) / 8 > 0) v2 = v1;
            float m = fmaxf(v1, v2) / 2;
            if(m < 0) m *= 0.8;
            height_map[x + y * w] = m;
        }
    }

    progress_bar_set_text(level_gen->progress_bar, "Eroding...");
    printf("Eroding...\n");
    for(int x = 0; x < w; x++) {
        progress_bar_set_progress(level_gen->progress_bar, x * 100 / (w - 1));
        for(int y = 0; y < d; y++) {
            float v1 = noise_get(&n1, x * 2, y * 2) / 8;
            int v2 = noise_get(&n2, x * 2, y *2) > 0;
            if(v1 > 2) height_map[x + y * w] = ((height_map[x + y * w] - v2) / 2 << 1) + v2;
        }
    }

    progress_bar_set_text(level_gen->progress_bar, "Soiling...");
    printf("Soiling...\n");
    for(int x = 0; x < w; x++) {
        progress_bar_set_progress(level_gen->progress_bar, x * 100 / (w - 1));
        for(int y = 0; y < d; y++) {
            int v1 = (int)(noise_get(&n3, x, y) / 24.0) - 4;
            int v2 = height_map[x + y * w] + level_gen->water_level;
            int v3 = v2 + v1;
            height_map[x + y * w] = v2 > v3 ? v2 : v3;
            if(height_map[x + y * w] > h - 2) height_map[x + y * w] = h - 2;
            if(height_map[x + y * w] < 1) height_map[x + y * w] = 1;
            for(int z = 0; z < h; z++) {
                int c = (z * d + y) * w + x;
                uint8_t block = blocks.air.id;
                if(z <= v2) block = blocks.dirt.id;
                if(z <= v3) block = blocks.stone.id;
                if(z == 0) block = blocks.lava.id;
                level_gen->blocks[c] = block;
            }
        }
    }

    progress_bar_set_text(level_gen->progress_bar, "Carving...");
    printf("Carving...\n");
    int ii = w * d * h / 256 / 64 << 1;
    for(int i = 0; i < ii; i++) {
        progress_bar_set_progress(level_gen->progress_bar, i * 100 / (ii - 1));
        
        float x1 = random_next_uniform(&level_gen->random) * w;
        float y1 = random_next_uniform(&level_gen->random) * h;
        float z1 = random_next_uniform(&level_gen->random) * d;
        int jj = (random_next_uniform(&level_gen->random) + random_next_uniform(&level_gen->random)) * 200.0;
        float r1x = random_next_uniform(&level_gen->random) * M_PI * 2.0;
        float r1y = random_next_uniform(&level_gen->random) * M_PI * 2.0;
        float r2y = 0;
        float v2 = random_next_uniform(&level_gen->random) * random_next_uniform(&level_gen->random);

        for(int j = 0; j < jj; j++) {
            x1 += tsin(r1x) * tcos(r1y);
            y1 += tsin(r1y);
            z1 += tcos(r1x) * tcos(r1y);
            r1x = (r1x + r1x * 0.2) * 0.9;
            r1y = (r1y + r2y * 0.5) * 0.5;
            r2y = r2y * 0.75 + random_next_uniform(&level_gen->random) - random_next_uniform(&level_gen->random);
            if(random_next_uniform(&level_gen->random) >= 0.25) {
                float x2 = x1 + (random_next_uniform(&level_gen->random) * 4 - 2) * 0.2;
                float y2 = y1 + (random_next_uniform(&level_gen->random) * 4 - 2) * 0.2;
                float z2 = z1 + (random_next_uniform(&level_gen->random) * 4 - 2) * 0.2;
                float v4 = (h - y2) / h;
                v4 = 1.2 + (v4 * 3.5 + 1) * v2;
                v4 *= tsin(j * M_PI / jj);
                for(int x = x2 - v4; x <= (int)(x2 + v4); x++) {
                    for(int y = y2 - v4; y <= (int)(y2 + v4); y++) {
                        for(int z = z2 - v4; z <= (int)(z2 + v4); z++) {
                            float x3 = x - x2;
                            float y3 = y - y2;
                            float z3 = z - z2;
                            if (x3 * x3 + 2.0 * y3 * y3 + z3 * z3 < v4 * v4 && x >= 1 && y >= 1 && z >= 1 && x < w - 1 && y < h - 1 && z < d - 1) {
                                int c = (y * d + z) * w + x;
                                if (level_gen->blocks[c] == blocks.stone.id) { level_gen->blocks[c] = blocks.air.id; }
                            }
                        }
                    }
                }
            }
        }
    }

    level_gen_populate_ore(level_gen, blocks.coal_ore.id, 90, 1, 4);
    level_gen_populate_ore(level_gen, blocks.iron_ore.id, 70, 2, 4);
    level_gen_populate_ore(level_gen, blocks.gold_ore.id, 50, 3, 4);

    progress_bar_set_text(level_gen->progress_bar, "Watering...");
    printf("Watering...\n");
    progress_bar_set_progress(level_gen->progress_bar, 0);

    uint8_t flood = blocks.still_water.id;
    for(int x = 0; x < w; x++) {
        level_gen_flood(level_gen, x, h / 2 - 1, 0, flood);
        level_gen_flood(level_gen, x, h / 2 - 1, d - 1, flood);
    }

    for(int z = 0; z < d; z++) {
        level_gen_flood(level_gen, 0, h / 2 - 1, z, flood);
        level_gen_flood(level_gen, w - 1, h / 2 - 1, z, flood);
    }

    ii = w * d / 8000;

    for(int i = 0; i < ii; i++) {
        if(i % 100) progress_bar_set_progress(level_gen->progress_bar, i * 100 / (ii - 1));
        int x = (int)random_next_int_range(&level_gen->random, 0, w - 1);
        int y = level_gen->water_level - 1 - (int)random_next_int_range(&level_gen->random, 0, 1);
        int z = (int)random_next_int_range(&level_gen->random, 0, d - 1);
        if(level_gen->blocks[(y * d + z) * w + x] == blocks.air.id) {
            level_gen_flood(level_gen, x, y, z, flood);
        }
    }
    progress_bar_set_progress(level_gen->progress_bar, 100);

    progress_bar_set_text(level_gen->progress_bar, "Melting...");
    printf("Melting...\n");
    ii = w * d * h / 20000;
    for(int i = 0; i < ii; i++) {
        if(i % 100) progress_bar_set_progress(level_gen->progress_bar, i * 100 / (ii - 1));
        int x = (int)random_next_int_range(&level_gen->random, 0, w - 1);
        int y = (int)random_next_uniform(&level_gen->random) * (int)random_next_uniform(&level_gen->random) * (level_gen->water_level - 3);
        int z = (int)random_next_int_range(&level_gen->random, 0, d - 1);
        if(level_gen->blocks[(y * d + z) * w + x] == blocks.air.id) {
            level_gen_flood(level_gen, x, y, z, blocks.still_lava.id);
        }
    }
    progress_bar_set_progress(level_gen->progress_bar, 100);

    progress_bar_set_text(level_gen->progress_bar, "Growing...");
    printf("Growing...\n");
    for(int x = 0; x < w; x++) {
        progress_bar_set_progress(level_gen->progress_bar, x * 100 / (w - 1));
        for(int y = 0; y < d; y++) {
            uint8_t v1 = noise_get(&n1, x, y) > 8;
            uint8_t v2 = noise_get(&n2, x, y) > 12;
            int z = height_map[x + y * w];
            int c = (z * d + y) * w + x;
            uint8_t above = level_gen->blocks[((z + 1) * d + y) * w + x];
            if((above == blocks.water.id || above == blocks.still_water.id) && z <= h / 2 - 1 && v2) {
                level_gen->blocks[c] = blocks.gravel.id;
            }
            if(above == blocks.air.id) {
                uint8_t block = blocks.grass.id;
                if(z <= h / 2 - 1 && v1) block = blocks.sand.id;
                level_gen->blocks[c] = block;
            }
        }
    }

    progress_bar_set_text(level_gen->progress_bar, "Planting...");
    printf("Planting...\n");
    ii = w * d / 3000;
    for(int i = 0; i < ii; i++) {
        progress_bar_set_progress(level_gen->progress_bar, i * 50 / (ii - 1));
        int x = random_next_int_range(&level_gen->random, 0, w - 1);
        int z = random_next_int_range(&level_gen->random, 0, d - 1);
        int f = random_next_int_range(&level_gen->random, 0, 1);
        for(int j = 0; j < 10; j++) {
            int xx = x;
            int zz = z;
            for(int k = 0; k < 5; k++) {
                xx += (int)random_next_int_range(&level_gen->random, 0, 5) - (int)random_next_int_range(&level_gen->random, 0, 5);
                zz += (int)random_next_int_range(&level_gen->random, 0, 5) - (int)random_next_int_range(&level_gen->random, 0, 5);
                if((f < 2 || random_next_int_range(&level_gen->random, 0, 3) == 0) && xx >= 0 && zz >= 0 && xx < w && zz < d) {
                    int y = height_map[xx + zz * w] + 1;
                    int c = (y * d + zz) * w + xx;
                    if(level_gen->blocks[c] == blocks.air.id) {
                        if(level_gen->blocks[((y - 1) * d + zz) * w + xx] == blocks.grass.id) {
                            if(f == 0) level_gen->blocks[c] = blocks.dandelion.id;
                            else if(f == 1) level_gen->blocks[c] = blocks.rose.id;
                        }
                    }
                }
            }
        }
    }

    ii = w * d * h / 2000;
    for(int i = 0; i < ii; i++) {
        progress_bar_set_progress(level_gen->progress_bar, i * 50 / (ii - 1) + 50);
        int m = (int)random_next_int_range(&level_gen->random, 0, 1);
        int x = (int)random_next_int_range(&level_gen->random, 0, w - 1);
        int y = (int)random_next_int_range(&level_gen->random, 0, h - 1);
        int z = (int)random_next_int_range(&level_gen->random, 0, d - 1);
        for(int j = 0; j < 20; j++) {
            int xx = x;
            int yy = y;
            int zz = z;
            for(int k = 0; k < 5; k++) {
                xx += (int)random_next_int_range(&level_gen->random, 0, 5) - (int)random_next_int_range(&level_gen->random, 0, 5);
                yy += (int)random_next_int_range(&level_gen->random, 0, 1) - (int)random_next_int_range(&level_gen->random, 0, 1);
                zz += (int)random_next_int_range(&level_gen->random, 0, 5) - (int)random_next_int_range(&level_gen->random, 0, 5);
                int c = (yy * d + zz) * w + xx;
                if((m < 2 || random_next_int_range(&level_gen->random, 0, 3) == 0) && xx >= 0 && zz >= 0 && yy >= 1 && xx < w && zz < d && yy < height_map[xx + zz * w] - 1 && level_gen->blocks[c] == 0) {
                    if(level_gen->blocks[((yy - 1) * d + zz) * w + xx] == blocks.stone.id) {
                        if(m == 0) level_gen->blocks[c] = blocks.brown_mushroom.id;
                        else level_gen->blocks[c] = blocks.red_mushroom.id;
                    }
                }
            }
        }
    }

    level->water_level = level_gen->water_level;
    level_set_data(level, w, 64, d, level_gen->blocks);
    ii = w * d / 4000;
    for(int i = 0; i < ii; i++) {
        progress_bar_set_progress(level_gen->progress_bar, i * 50 / (ii - 1) + 50);
        int x = (int)random_next_int_range(&level_gen->random, 0, w - 1);
        int z = (int)random_next_int_range(&level_gen->random, 0, d - 1);
        for(int j = 0; j < 20; j++) {
            int xx = x;
            int zz = z;
            for(int k = 0; k < 20; k++) {
                xx += (int)random_next_int_range(&level_gen->random, 0, 5) - (int)random_next_int_range(&level_gen->random, 0, 5);
                zz += (int)random_next_int_range(&level_gen->random, 0, 5) - (int)random_next_int_range(&level_gen->random, 0, 5);
                if(xx >= 0 && zz >= 0 && xx < w && zz < d) {
                    int y = height_map[xx + zz * w] + 1;
                    if(random_next_int_range(&level_gen->random, 0, 3) == 0) level_maybe_grow_tree(level, xx, y, zz);
                }
            }
        }
    }

    for(int i = 0; i < sizeof(oct) / sizeof(oct[0]); i++) {
        noise_destroy(&oct[i]);
    }
    noise_destroy(&n1);
    noise_destroy(&n2);
    noise_destroy(&n3);
    free(level_gen->blocks);
    free(height_map);
    free(level_gen->flood_data);
}