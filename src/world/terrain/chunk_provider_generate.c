#include <world/terrain/chunk_provider_generate.h>

#include <util/array_list.h>
#include <world/block/blocks.h>
#include <world/terrain/generate/generate_big_tree.h>
#include <world/terrain/generate/generate_mineable.h>
#include <world/terrain/noise/noise.h>
#include <world/terrain/noise/noise_octave.h>
#include <world/world.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

void chunk_provider_generate_create(chunk_provider_t *chunk_provider, world_t *world, int64_t seed){
    *chunk_provider = (chunk_provider_t){ 0 };

    chunk_provider->world = world;

    chunk_provider->random = random_create(seed);

    chunk_provider->noise_1 = noise_octave_create(chunk_provider->random, 16);
    chunk_provider->noise_2 = noise_octave_create(chunk_provider->random, 16);
    chunk_provider->noise_3 = noise_octave_create(chunk_provider->random, 8);
    chunk_provider->noise_4 = noise_octave_create(chunk_provider->random, 4);
    chunk_provider->noise_5 = noise_octave_create(chunk_provider->random, 4);
    chunk_provider->tree_noise = noise_octave_create(chunk_provider->random, 5);

    chunk_provider->chunk_provide = chunk_provider_generate_provide_chunk;
    chunk_provider->populate = chunk_provider_generate_populate;
}

chunk_t *chunk_provider_generate_provide_chunk(chunk_provider_t *chunk_provider, int chunk_x, int chunk_z) {
    chunk_provider->random.seed = ((int64_t)chunk_x * 341873128712 + (int64_t)chunk_z * 132897987541);

    uint8_t chunk_data[32768];
    memset(chunk_data, 0, sizeof(chunk_data));
    chunk_t *chunk = malloc(sizeof(chunk_t));
    chunk_create_from(chunk, chunk_provider->world, chunk_data, chunk_x, chunk_z);

    int noise_start_x = chunk_x * 4;
    int noise_start_z = chunk_z * 4;

    if(chunk_provider->noise_array == NULL) {
        chunk_provider->noise_array = malloc(sizeof(double) * 425);
    }

    chunk_provider->noise_array_1 = noise_octave_generate_octaves(chunk_provider->noise_array_1, chunk_provider->noise_1, noise_start_x, 0, noise_start_z, 5, 17, 5, 684.412, 684.412, 684.412);
    chunk_provider->noise_array_2 = noise_octave_generate_octaves(chunk_provider->noise_array_2, chunk_provider->noise_2, noise_start_x, 0, noise_start_z, 5, 17, 5, 684.412, 684.412, 684.412);
    chunk_provider->noise_array_3 = noise_octave_generate_octaves(chunk_provider->noise_array_3, chunk_provider->noise_3, noise_start_x, 0, noise_start_z, 5, 17, 5, 8.555150000000001, 4.277575000000001, 8.555150000000001);

    int noise_index = 0;

    for(int x = 0; x < 5; x++) {
        for(int z = 0; z < 5; z++) {
            for(int y = 0; y < 17; y++) {
                double height_adjustment = ((double)y - 8.5) * 12.0;
                if(height_adjustment < 0) height_adjustment *= 2;

                double noise_value_1 = chunk_provider->noise_1[noise_index] / 512.0;
                double noise_value_2 = chunk_provider->noise_2[noise_index] / 512.0;
                double noise_blend = (chunk_provider->noise_3[noise_index] / 10.0 + 1.0) / 2.0;
                double final_noise_value = 0;

                if(noise_blend < 0.0) final_noise_value = noise_value_1;
                else if(noise_blend > 1.0) final_noise_value = noise_value_2;
                else final_noise_value = noise_value_1 + (noise_value_2 - noise_value_1) * noise_blend;

                final_noise_value -= height_adjustment;
                chunk_provider->noise_array[noise_index] = final_noise_value;
                noise_index++;
            }
        }
    }

    // Generates stone and water
    for(int x = 0; x < 4; x++) {
        for(int z = 0; z < 4; z++) {
            for(int y = 0; y < CHUNK_SIZE_WIDTH; y++) {
                double noise000 = chunk_provider->noise_array[(x * 5 + z) * 17 + y];
                double noise001 = chunk_provider->noise_array[(x * 5 + z + 1) * 17 + y];
                double noise100 = chunk_provider->noise_array[((x + 1) * 5 + z) * 17 + y];
                double noise101 = chunk_provider->noise_array[((x + 1) * 5 + z + 1) * 17 + y];
                double noise010 = chunk_provider->noise_array[(x * 5 + z) * 17 + y + 1];
                double noise011 = chunk_provider->noise_array[(x * 5 + z + 1) * 17 + y + 1];
                double noise110 = chunk_provider->noise_array[((x + 1) * 5 + z) * 17 + y + 1];
                double noise111 = chunk_provider->noise_array[((x + 1) * 5 + z + 1) * 17 + y + 1];

                for(int yy = 0; yy < 8; yy++) {
                    double y_lerp = (double)yy / 8.0;
                    double noise_x00 = noise000 + (noise010 - noise000) * y_lerp;
                    double noise_x01 = noise001 + (noise011 - noise001) * y_lerp;
                    double noise_x10 = noise100 + (noise110 - noise100) * y_lerp;
                    double noise_x11 = noise101 + (noise111 - noise101) * y_lerp;

                    for(int xx = 0; xx < 4; xx++) {
                        double x_lerp = (double)xx / 4.0;
                        double noise_z0 = noise_x00 + (noise_x10 - noise_x00) * x_lerp;
                        double noise_z1 = noise_x01 + (noise_x11 - noise_x01) * x_lerp;

                        for(int zz = 0; zz < 4; zz++) {
                            double z_lerp = (double)zz / 4.0;
                            double final_noise = noise_z0 + (noise_z1 - noise_z0) * z_lerp;

                            int block_index = xx + (x << 2) << 11 | zz + (z << 2) << 7 | (y << 3) + yy;
                            uint8_t block_id = 0;

                            if((y << 3) + yy < 64) {
                                block_id = blocks.still_water.id;
                            }

                            if(final_noise > 0.0) {
                                block_id = blocks.stone.id;
                            }

                            chunk_data[block_index] = block_id;
                        }
                    }
                }
            }
        }
    }

    // Generate surface blocks (grass, dirt, gravel, sand, more water)
    for(int x = 0; x < CHUNK_SIZE_WIDTH; x++) {
        for(int z = 0; z < CHUNK_SIZE_WIDTH; z++) {
            double surface_x = (double)((chunk_x * CHUNK_SIZE_WIDTH) + x);
            double surface_z = (double)((chunk_z * CHUNK_SIZE_WIDTH) + z);
            uint8_t sand = chunk_provider->noise_4.get(&chunk_provider->noise_4, surface_x * (1.0 / 32.0), surface_z * (1.0 / 32.0), 0) + random_next_uniform(chunk_provider->random) * 0.2 > 0.0;
            uint8_t gravel = chunk_provider->noise_4.get(&chunk_provider->noise_4, surface_z * (1.0 / 32.0), 109.0134, surface_x * (1.0 / 32.0)) + random_next_uniform(chunk_provider->random) * 0.2 > 3.0;
            int surface_depth = (int)(chunk_provider->noise_5.get(&chunk_provider->noise_5, surface_x * (1.0 / 32.0) * 2.0, surface_z * (1.0 / 32.0) * 2.0, 0) / 3.0 + 3.0 + random_next_uniform(chunk_provider->random) * 0.25);
            int block_index = x << 11 | z << 7 | (CHUNK_SIZE_HEIGHT - 1);
            uint8_t top_block = blocks.grass.id;
            uint8_t filler_block = blocks.dirt.id;

            for(int y = CHUNK_SIZE_HEIGHT - 1; y >= 0; y--) {
                if(chunk_data[block_index] == 0) {
                    surface_depth = -1;
                }else if(chunk_data[block_index] == blocks.stone.id) {
                    if(surface_depth == -1) {
                        if(surface_depth <= 0) {
                            top_block = 0;
                            filler_block = blocks.stone.id;
                        }else if(y >= 60 && y <= 65) {
                            top_block = blocks.grass.id;
                            filler_block = blocks.dirt.id;
                            if(gravel) {
                                top_block = 0;
                            }
                            if(sand) {
                                top_block = blocks.sand.id;
                                filler_block = blocks.sand.id;
                            }
                        }

                        if(y < 64 && top_block == 0) {
                            top_block = blocks.still_water.id;
                        }

                        if(y >= 63) {
                            chunk_data[block_index] = top_block;
                        }else {
                            chunk_data[block_index] = filler_block;
                        }
                    }else if(surface_depth > 0) {
                        surface_depth--;
                        chunk_data[block_index] = filler_block;
                    }
                }

                block_index--;
            }
        }
    }

    chunk_generate_height_map(chunk);
    return chunk;
}

void chunk_provider_generate_populate(chunk_provider_t *chunk_provider, chunk_provider_t *interface, int chunk_x, int chunk_z) {
    chunk_provider->random.seed = ((int64_t)chunk_x * 318279123 + (int64_t)chunk_z * 919871212);

    int chunk_start_x = chunk_x / CHUNK_SIZE_WIDTH;
    int chunk_start_z = chunk_z / CHUNK_SIZE_WIDTH;

    // Coal Ore
    for(int i = 0; i < 20; i++) {
        int x = chunk_start_x + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH);
        int y = random_next_int_range(&chunk_provider->random, 0, 127);
        int z = chunk_start_z + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH);
        generate_mineable_gen(chunk_provider->world, &chunk_provider->random, x, y, z, blocks.coal_ore.id);
    }

    // Iron Ore
    for(int i = 0; i < 10; i++) {
        int x = chunk_start_x + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH);
        int y = random_next_int_range(&chunk_provider->random, 0, 63);
        int z = chunk_start_z + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH);
        generate_mineable_gen(chunk_provider->world, &chunk_provider->random, x, y, z, blocks.iron_ore.id);
    }

    // Gold Ore
    if(random_next_int_range(&chunk_provider->random, 0, 1) == 0) {
        int x = chunk_start_x + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH);
        int y = random_next_int_range(&chunk_provider->random, 0, 31);
        int z = chunk_start_z + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH);
        generate_mineable_gen(chunk_provider->world, &chunk_provider->random, x, y, z, blocks.gold_ore.id);
    }

    // Diamond Ore
    if(random_next_int_range(&chunk_provider->random, 0, 7) == 0) {
        int x = chunk_start_x + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH);
        int y = random_next_int_range(&chunk_provider->random, 0, 15);
        int z = chunk_start_z + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH);
        generate_mineable_gen(chunk_provider->world, &chunk_provider->random, x, y, z, blocks.diamond_ore.id);
    }

    // Trees
    int tree_count = (int)(chunk_provider->tree_noise.get(&chunk_provider->tree_noise, ((double)chunk_start_x * 0.05, ((double)chunk_start_z * 0.05), 0) - random_next_uniform(&chunk_provider->random)));
    if(tree_count < 0) tree_count = 0;

    if(random_next_int_range(&chunk_provider->random, 0, 99) == 0) {
        tree_count++;
    }

    for(int i = 0; i < tree_count; i++) {
        int x = chunk_start_x + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH) + 8;
        int z = chunk_start_z + random_next_int_range(&chunk_provider->random, 0, CHUNK_SIZE_WIDTH) + 8;
        int y = world_get_height_value(chunk_provider->world, x, z);
        generate_big_tree_gen(chunk_provider->world, &chunk_provider->random, x, y, z);
    }
}