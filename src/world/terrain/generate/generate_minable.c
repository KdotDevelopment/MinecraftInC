#include <world/terrain/generate/generate_mineable.h>

#include <world/block/blocks.h>

uint8_t generate_mineable_gen(world_t *world, random_t *random, int gen_x, int gen_y, int gen_z, uint8_t block_id) {
    float angle = random_next_uniform(random) * M_PI;

    double start_x = (float)(gen_x + 8) + tsin(angle) * 2.0;
    double end_x = (float)(gen_x + 8) - tsin(angle) * 2.0;
    double start_z = (float)(gen_z + 8) + tcos(angle) * 2.0;
    double end_z = (float)(gen_z + 8) - tcos(angle) * 2.0;
    double start_y = gen_y + random_next_int_range(random, 2, 4);
    double end_y = gen_y + random_next_int_range(random, 2, 4);

    for(int i = 0; i <= 16; i++) {
        double current_x = start_x + (end_x - start_x) * (double)i / 16.0;
        double current_y = start_y + (end_y - start_y) * (double)i / 16.0;
        double current_z = start_z + (end_z - start_z) * (double)i / 16.0;

        double size = random_next_uniform(random);
        double radius_x = (tsin((float)i / 16.0 * M_PI) + 1.0) * size + 1.0;
        double radius_y = (tcos((float)i / 16.0 * M_PI) + 1.0) * size + 1.0;

        for(int x = (current_x - radius_x / 2.0); x <= (current_x + radius_x / 2.0); x++) {
            for(int y = (current_y - radius_y / 2.0); y <= (current_y + radius_y / 2.0); y++) {
                for(int z = (current_z - radius_x / 2.0); z <= (current_z + radius_x / 2.0); z++) {
                    double norm_x = (x + 0.5 - current_x) / (radius_x / 2.0);
                    double norm_y = (y + 0.5 - current_y) / (radius_y / 2.0);
                    double norm_z = (z + 0.5 - current_z) / (radius_x / 2.0);

                    if (norm_x * norm_x + norm_y * norm_y + norm_z * norm_z <= 1.0 && world_get_block(world, x, y, z) == blocks.stone.id) {
                        world_set_block_no_update(world, x, y, z, block_id);
                    }
                }
            }
        }
    }

    return 1;
}