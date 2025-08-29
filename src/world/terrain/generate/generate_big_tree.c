#include <world/terrain/generate/generate_big_tree.h>

#include <world/block/block.h>
#include <world/block/blocks.h>

#include <math.h>
#include <stdint.h>

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

    uint8_t prim_i;
    for(int i = 0; i < 3; i++) {
        delta[i] = end[i] - start[i];
        if(fabs(delta[i]) > fabs(delta[prim_i])) {
            prim_i = i;
        }
    }

    if(delta[prim_i] == 0) return -1;

    uint8_t sec_1 = coord_pairs[prim_i];
    uint8_t sec_2 = coord_pairs[prim_i + 3];
    uint8_t sign;

    if(delta[prim_i] > 0) sign = 1;
    else sign = -1;

    double sec_fac_1 = delta[sec_1] / delta[prim_i];
    double sec_fac_2 = delta[sec_2] / delta[prim_i];

    int coordinate[] = { 0, 0, 0 };
    uint8_t block_id = 0;
    int i = 0;
    for(i = 0; i != delta[prim_i] + sign; i += sign) {
        coordinate[prim_i] = start[prim_i] + i;
        coordinate[sec_1] = floor(start[sec_1] + (i * sec_fac_1));
        coordinate[sec_2] = floor(start[sec_2] + (i * sec_fac_2));
        block_id = world_get_block(this->world, coordinate[0], coordinate[1], coordinate[2]);
        if(block_id != blocks.air.id && block_id != blocks.leaves.id) break;
    }

    if(i != delta[prim_i] + sign) return -1;
    return fabs(i);
}

uint8_t check_position(generate_big_tree_t *this) {
    vec3_t base_pos = (vec3_t){ this->origin.x, this->origin.y, this->origin.z };
    vec3_t top_pos = (vec3_t){ this->origin.x, this->origin.y + this->height - 1, this->origin.z };
    uint8_t ground_block_id = world_get_block(this->world, base_pos.x, base_pos.y - 1, base_pos.z);

    if(ground_block_id != blocks.grass.id && ground_block_id != blocks.dirt.id) {
        return 0;
    }
    int block_line_check = check_line(this, base_pos, top_pos);
    if(block_line_check == -1) return 1;
    else if(block_line_check < 6) return 0;
    else {
        this->height = block_line_check;
        return 1;
    }
}

float tree_shape(generate_big_tree_t *this, int y) {
    if(y < (((float)this->height) * 0.3)) return -1.618;

    float radius = ((float)this->height) / 2.0;
    float adjacent = (((float)this->height) / 2.0) - y;
    float distance = 0;

    if(adjacent == 0) distance = radius;
    else if(fabs(adjacent) >= radius) distance = 0;
    else distance = sqrt(radius * radius - adjacent * adjacent);

    distance *= 0.5;

    return distance;
}

void prepare(generate_big_tree_t *this) {
    this->trunk_height = this->height * this->trunk_height_scale;
    if(this->trunk_height >= this->height) this->trunk_height = this->height - 1;

    int clusters_per_y = (int)(1.382 + pow(this->foliage_density * this->height / 13.0, 2));
    if(clusters_per_y < 1) clusters_per_y = 1;

    int (*temp_coords)[4] = malloc(sizeof(int[4]) * this->height);
    this->coords = malloc(sizeof(int[4]) * this->height);
    this->coord_length = 4 * this->height;
    int y = this->origin.y + this->height - this->foliage_height;
    int cluster_count = 1;
    int trunk_top = this->origin.y + this->trunk_height;
    int relative_y = y - this->origin.y;
    temp_coords[0][0] = this->origin.x;
    temp_coords[0][1] = y;
    temp_coords[0][2] = this->origin.z;
    temp_coords[0][3] = trunk_top;
    y--;

    while(relative_y >= 0) {
        int num = 0;
        float shapefac = tree_shape(this, relative_y);

        if(shapefac < 0) {
            y--;
            relative_y--;
            continue;
        }

        // middle of block offset
        double origin_offset = 0.5;
        while(num < clusters_per_y) {
            double radius = this->width_scale * (shapefac * (random_next_uniform(this->rand) + 0.328));
            double angle = random_next_uniform(this->rand) * 2.0 * M_PI;
            int x = floor(radius * tsin(angle) + this->origin.x + origin_offset);
            int z = floor(radius * tcos(angle) + this->origin.z + origin_offset);
            vec3_t check_start = (vec3_t){ x, y, z };
            vec3_t check_end = (vec3_t){ x, y + this->foliage_height, z };

            if(check_line(this, check_start, check_end) == -1) {
                vec3_t check_branch_base = (vec3_t){ this->origin.x, this->origin.y, this->origin.z };
                double distance = sqrt(pow(this->origin.x - check_start.x, 2) + pow(this->origin.z - check_start.z, 2));
                double branch_height = distance * this->branch_slope;

                if((check_start.y - branch_height) > trunk_top) {
                    check_branch_base.y = trunk_top;
                }else {
                    check_branch_base.y = (int)(check_start.y - branch_height);
                }

                if(check_line(this, check_branch_base, check_start) == -1) {
                    temp_coords[cluster_count][0] = x;
                    temp_coords[cluster_count][1] = y;
                    temp_coords[cluster_count][2] = z;
                    temp_coords[cluster_count][3] = (int)check_branch_base.y;
                    cluster_count++;
                }
            }
            num++;
        }
        y--;
        relative_y--;
    }
    memcpy(this->coords, temp_coords, sizeof(int[4]) * cluster_count);
    free(temp_coords);
}

float foliage_shape(generate_big_tree_t *this, int y) {
    if((y < 0) || (y >= this->foliage_height)) return -1.0;
    else if((y == 0) || (y == (this->foliage_height - 1))) return 2.0;
    else return 3.0;
}

void cross_section(generate_big_tree_t *this, int x, int y, int z, float radius, uint8_t direction, uint8_t block_id) {
    int rad = radius + 0.618;
    uint8_t sec_1 = coord_pairs[direction];
    uint8_t sec_2 = coord_pairs[direction + 3];
    int center[] = { x, y, z };
    int position[] = { 0, 0, 0 };
    uint8_t block_id = 0;
    
    for(int x = -rad; x <= rad; x++) {
        position[sec_1] = center[sec_1] + x;
        for(int z = -rad; z <= rad; z++) {
            double distance = pow(fabs(x) + 0.5, 2) + pow(fabs(z) + 0.5, 2);
            if(distance > radius * radius) continue;
            position[sec_2] = center[sec_2] + z;
            block_id = world_get_block(this->world, position[0], position[1], position[2]);
            if(block_id != blocks.air.id && block_id != blocks.leaves.id) continue;
            world_set_block(this->world, position[0], position[1], position[2], block_id);
        }
    }
}

void foliage_cluster(generate_big_tree_t *this, int x, int y, int z) {
    int current_y = y;
    int top_y = y + this->foliage_height;
    float radius = 0;
    while(current_y < top_y) {
        radius = foliage_shape(this, current_y - y);
        cross_section(this, x, current_y, z, radius, 1, blocks.leaves.id);
        current_y++;
    }
}

void limb(generate_big_tree_t *this, vec3_t start_vec, vec3_t end_vec, uint8_t block_id) {
    int delta[] = { 0, 0, 0 };
    int start[] = { start_vec.x, start_vec.y, start_vec.z };
    int end[] = { end_vec.x, end_vec.y, end_vec.z };

    uint8_t prim_i;
    for(int i = 0; i < 3; i++) {
        delta[i] = end[i] - start[i];
        if(fabs(delta[i]) > fabs(delta[prim_i])) {
            prim_i = i;
        }
    }

    if(delta[prim_i] == 0) return -1;

    uint8_t sec_1 = coord_pairs[prim_i];
    uint8_t sec_2 = coord_pairs[prim_i + 3];
    uint8_t sign;

    if(delta[prim_i] > 0) sign = 1;
    else sign = -1;

    double sec_fac_1 = delta[sec_1] / delta[prim_i];
    double sec_fac_2 = delta[sec_2] / delta[prim_i];

    int coordinate[] = { 0, 0, 0 };
    int i = 0;
    for(i = 0; i != delta[prim_i] + sign; i += sign) {
        coordinate[prim_i] = floor(start[prim_i] + i + 0.5);
        coordinate[sec_1] = floor(start[sec_1] + (i * sec_fac_1) + 0.5);
        coordinate[sec_2] = floor(start[sec_2] + (i * sec_fac_2) + 0.5);
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
    int top_y = this->origin.y + this->trunk_height;
    
    vec3_t start_coord = (vec3_t){ x, start_y, z };
    vec3_t end_coord = (vec3_t){ x, top_y, z };
    limb(this, start_coord, end_coord, blocks.log.id);
    if(this->trunk_width == 2) {
        start_coord.x += 1;
        end_coord.x += 1;
        limb(this, start_coord, end_coord, blocks.log.id);
        start_coord.z += 1;
        end_coord.z += 1;
        limb(this, start_coord, end_coord, blocks.log.id);
        start_coord.x -= 1;
        end_coord.x -= 1;
        limb(this, start_coord, end_coord, blocks.log.id);
    }
}

uint8_t trim_branches(generate_big_tree_t *this, int local_y) {
    if(local_y < (this->height * 0.2)) return 0;
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

// random only makes a new seed for the current random
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

    if(this.height == 0) {
        this.height = 5 + random_next_int_range(&rand, 0, this.height);
    }

    if(!check_position(&this)) return 0;

    prepare(&this);
    make_foliage(&this);
    make_trunk(&this);
    make_branches(&this);

    free(this.coords);

    return 1;
}