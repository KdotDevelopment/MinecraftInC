#include <world/terrain/generate/generate_big_tree.h>

#include <world/block/block.h>
#include <world/block/blocks.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Paul Spooner big tree code

#define HEIGHT_ATTENUATION 0.618
#define BRANCH_SLOPE 0.381
#define SCALE_WIDTH 1.0
#define LEAF_DENSITY 1.0
#define TRUNK_SIZE 1
#define HEIGHT_LIMIT 12
#define LEAF_DISTANCE_LIMIT 4

static uint8_t coord_pairs[] = { 2, 0, 0, 1, 2, 1 };

// https://pastebin.com/XBLdGqXQ <-- Paul Spooner's code

int check_line(generate_big_tree_t *this, vec3_t start_vec, vec3_t end_vec) {
    int delta[] = { 0, 0, 0 };
    int start[] = { start_vec.x, start_vec.y, start_vec.z };
    int end[] = { end_vec.x, end_vec.y, end_vec.z };

    uint8_t prim_i = 0;
    for(int i = 0; i < 3; i++) {
        delta[i] = end[i] - start[i];
        if(abs(delta[i]) > abs(delta[prim_i])) {
            prim_i = i;
        }
    }

    if(delta[prim_i] == 0) return -1;

    uint8_t sec_1 = coord_pairs[prim_i];
    uint8_t sec_2 = coord_pairs[prim_i + 3];
    int sign;

    if(delta[prim_i] > 0) sign = 1;
    else sign = -1;

    double sec_fac_1 = (double)delta[sec_1] / (double)delta[prim_i];
    double sec_fac_2 = (double)delta[sec_2] / (double)delta[prim_i];

    int coordinate[] = { 0, 0, 0 };
    uint8_t block_id = 0;
    int i = 0;
    for(i = 0; i != delta[prim_i] + sign; i += sign) {
        coordinate[prim_i] = start[prim_i] + i;
        coordinate[sec_1] = (int)((double)start[sec_1] + (double)i * sec_fac_1);
        coordinate[sec_2] = (int)((double)start[sec_2] + (double)i * sec_fac_2);
        block_id = world_get_block(this->world, coordinate[0], coordinate[1], coordinate[2]);
        if(block_id != blocks.air.id && block_id != blocks.leaves.id) break;
    }

    if(i == delta[prim_i] + sign) return -1;
    return abs(i);
}

uint8_t check_position(generate_big_tree_t *this) {
    vec3_t base_pos = (vec3_t){ this->origin.x, this->origin.y, this->origin.z };
    vec3_t top_pos = (vec3_t){ this->origin.x, this->origin.y + this->height_limit - 1, this->origin.z };
    uint8_t ground_block_id = world_get_block(this->world, base_pos.x, base_pos.y - 1, base_pos.z);

    if(ground_block_id != blocks.grass.id && ground_block_id != blocks.dirt.id) {
        return 0;
    }

    int block_line_check = check_line(this, base_pos, top_pos);

    if(block_line_check == -1)
        return 1;
    else if(block_line_check < 6)
        return 0;
    else {
        this->height_limit = block_line_check;
        return 1;
    }
}

float tree_shape(generate_big_tree_t *this, int y) {
    if ((double)y < (double)this->height_limit * 0.3)
        return -1.618f;

    float half = (float)this->height_limit / 2.0f;
    float dy = half - (float)y;
    float result;

    if (dy == 0.0f)
        result = half;
    else if (fabsf(dy) >= half)
        result = 0.0f;
    else
        result = sqrtf(half * half - dy * dy);

    return result * 0.5f;
}

void prepare(generate_big_tree_t *this) {
    this->height = (int)((double)this->height_limit * this->trunk_height_scale);
    if(this->height >= this->height_limit)
        this->height = this->height_limit - 1;

    int clusters_per_y = (int)(1.382 + pow(this->foliage_density * this->height_limit / 13.0, 2.0));
    if(clusters_per_y < 1)
        clusters_per_y = 1;

    int max_clusters = clusters_per_y * this->height_limit;
    int (*temp_coords)[4] = malloc(sizeof(int[4]) * max_clusters);
    int cluster_count = 1;

    int foliage_y = this->origin.y + this->height_limit - this->foliage_height;
    int trunk_top = this->origin.y + this->height;
    int rel_y = foliage_y - this->origin.y;

    temp_coords[0][0] = this->origin.x;
    temp_coords[0][1] = foliage_y;
    temp_coords[0][2] = this->origin.z;
    temp_coords[0][3] = trunk_top;

    foliage_y--;

    while(rel_y >= 0) {
        float shapefac = tree_shape(this, rel_y);
        if (shapefac < 0.0f) {
            foliage_y--;
            rel_y--;
            continue;
        }

        for(int n = 0; n < clusters_per_y; n++) {
            double radius = this->width_scale * shapefac * (random_next_uniform(this->rand) + 0.328);
            double angle = random_next_uniform(this->rand) * 2.0 * M_PI;

            int x = (int)(radius * tsin(angle) + this->origin.x + 0.5);
            int z = (int)(radius * tcos(angle) + this->origin.z + 0.5);

            vec3_t check_start = { x, foliage_y, z };
            vec3_t check_end = { x, foliage_y + this->foliage_height, z };

            if(check_line(this, check_start, check_end) == -1) {
                double dist = sqrt(pow(this->origin.x - x, 2) + pow(this->origin.z - z, 2));
                double branch_drop = dist * this->branch_slope;

                vec3_t base = this->origin;
                if ((double)foliage_y - branch_drop > trunk_top)
                    base.y = trunk_top;
                else
                    base.y = (int)((double)foliage_y - branch_drop);

                if(check_line(this, base, check_start) == -1) {
                    temp_coords[cluster_count][0] = x;
                    temp_coords[cluster_count][1] = foliage_y;
                    temp_coords[cluster_count][2] = z;
                    temp_coords[cluster_count][3] = base.y;
                    cluster_count++;
                }
            }
        }

        foliage_y--;
        rel_y--;
    }

    this->coords = malloc(sizeof(int[4]) * cluster_count);
    this->coord_length = cluster_count;
    memcpy(this->coords, temp_coords, sizeof(int[4]) * cluster_count);
    free(temp_coords);
}

void cross_section(generate_big_tree_t *this, int x, int y, int z, float radius, uint8_t direction, uint8_t block_id) {
    int rad = (int)(radius + 0.618);
    for (int dx = -rad; dx <= rad; ++dx) {
        for (int dz = -rad; dz <= rad; ++dz) {
            double dist = pow(fabs(dx) + 0.5, 2) + pow(fabs(dz) + 0.5, 2);
            if (dist > radius * radius) continue;
            int px = x + dx;
            int pz = z + dz;
            uint8_t existing = world_get_block(this->world, px, y, pz);
            if (existing != blocks.air.id && existing != blocks.leaves.id) continue;
            world_set_block_no_update(this->world, px, y, pz, block_id);
        }
    }
}
void foliage_cluster(generate_big_tree_t *this, int x, int y, int z) {
    int top_y = y + this->foliage_height;
    for (int cy = y; cy < top_y; ++cy) {
        int local_y = cy - y;
        float radius;

        if (local_y < 0 || local_y >= this->foliage_height)
            radius = -1.0f;
        else if (local_y == 0 || local_y == this->foliage_height - 1)
            radius = 2.0f;
        else
            radius = 3.0f;

        if (radius < 0.0f)
            continue;

        cross_section(this, x, cy, z, radius, 0, blocks.leaves.id);
    }
}

void limb(generate_big_tree_t *this, vec3_t start_vec, vec3_t end_vec, uint8_t block_id) {
    int delta[] = { 0, 0, 0 };
    int start[] = { start_vec.x, start_vec.y, start_vec.z };
    int end[] = { end_vec.x, end_vec.y, end_vec.z };

    uint8_t prim_i = 0;
    for(int i = 0; i < 3; i++) {
        delta[i] = end[i] - start[i];
        if(abs(delta[i]) > abs(delta[prim_i])) {
            prim_i = i;
        }
    }

    if(delta[prim_i] == 0) return;

    uint8_t sec_1 = coord_pairs[prim_i];
    uint8_t sec_2 = coord_pairs[prim_i + 3];
    int sign;

    if(delta[prim_i] > 0) sign = 1;
    else sign = -1;

    double sec_fac_1 = (double)delta[sec_1] / (double)delta[prim_i];
    double sec_fac_2 = (double)delta[sec_2] / (double)delta[prim_i];

    int coordinate[] = { 0, 0, 0 };
    int i = 0;
    for(i = 0; i != delta[prim_i] + sign; i += sign) {
        coordinate[prim_i] = (int)floor((double)start[prim_i] + i + 0.5);
        coordinate[sec_1] = (int)floor((double)start[sec_1] + ((double)i * sec_fac_1) + 0.5);
        coordinate[sec_2] = (int)floor((double)start[sec_2] + ((double)i * sec_fac_2) + 0.5);
        world_set_block_no_update(this->world, coordinate[0], coordinate[1], coordinate[2], block_id);
    }
}

void make_foliage(generate_big_tree_t *this) {
    int i = 0;
    int finish = this->coord_length;

    while(i < finish) {
        int x = this->coords[i][0];
        int y = this->coords[i][1];
        int z = this->coords[i][2];
        foliage_cluster(this, x, y, z);
        i++;
    }
}

void make_trunk(generate_big_tree_t *this) {
    int x = this->origin.x;
    int start_y = this->origin.y;
    int z = this->origin.z;
    int top_y = this->origin.y + this->height;

    vec3_t start_coord = { x, start_y, z };
    vec3_t end_coord = { x, top_y, z };

    limb(this, start_coord, end_coord, blocks.log.id);

    if(this->trunk_width == 2) {
        start_coord.x += 1; end_coord.x += 1;
        limb(this, start_coord, end_coord, blocks.log.id);
        start_coord.z += 1; end_coord.z += 1;
        limb(this, start_coord, end_coord, blocks.log.id);
        start_coord.x -= 1; end_coord.x -= 1;
        limb(this, start_coord, end_coord, blocks.log.id);
    }
}

uint8_t trim_branches(generate_big_tree_t *this, int local_y) {
    if(local_y < (this->height_limit * 0.2)) return 0;
    return 1;
}

void make_branches(generate_big_tree_t *this) {
    vec3_t base = (vec3_t){ this->origin.x, this->origin.y, this->origin.z };
    for(int i = 0; i < this->coord_length; i++) {
        int coord_vals[4] = { this->coords[i][0], this->coords[i][1], this->coords[i][2], this->coords[i][3] };
        vec3_t end_coord = (vec3_t){ coord_vals[0], coord_vals[1], coord_vals[2] };
        base.y = coord_vals[3];
        int local_y = base.y - this->origin.y;
        if(trim_branches(this, local_y)) {
            limb(this, base, end_coord, blocks.log.id);
        }
    }
}

uint8_t generate_big_tree_gen(world_t *world, random_t *random, int x, int y, int z) {
    random_t rand = random_create(random_next_int(random));

    generate_big_tree_t this = { 0 };

    this.origin = (vec3_t){ x, y, z };
    this.rand = &rand;
    this.world = world;
    this.trunk_height_scale = HEIGHT_ATTENUATION;
    this.branch_density = LEAF_DENSITY;
    this.branch_slope = BRANCH_SLOPE;
    this.width_scale = SCALE_WIDTH;
    this.foliage_density = LEAF_DENSITY;
    this.trunk_width = TRUNK_SIZE;
    this.height_variance = HEIGHT_LIMIT;
    this.foliage_height = LEAF_DISTANCE_LIMIT;

    int height_limit = 0;
    height_limit = 5 + random_next_int_range(&rand, 0, this.height_variance);
    this.height_limit = height_limit;

    this.height = (int)((double)height_limit * this.trunk_height_scale);
    if(this.height >= height_limit) this.height = height_limit - 1;

    int saved_height = this.height;
    this.height = height_limit;
    if(!check_position(&this)) {
        return 0;
    }
    this.height = saved_height;

    prepare(&this);
    make_foliage(&this);
    make_trunk(&this);
    make_branches(&this);

    free(this.coords);

    return 1;
}