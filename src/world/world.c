#include <world/world.h>

#include <renderer/renderer_world.h>
#include <world/block/block.h>
#include <world/block/blocks.h>
#include <sound/sounds.h>
#include <minecraft.h>
#include <world/chunk/chunk.h>
#include <world/material/materials.h>
#include <world/chunk/chunk_provider_load.h>

#include <util/array_list.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define PATH_SEPARATOR '/'
#define mkdir_recursive _mkdir
#else
#include <unistd.h>
#include <libgen.h>
#define PATH_SEPARATOR '/'
#define mkdir_recursive mkdir
#endif

void world_create(world_t *world, struct minecraft_s *minecraft, char *saves_dir, char *world_name, int64_t seed) {
    memset(world, 0, sizeof(world_t));
    
    world->minecraft = minecraft;
    world->lighting_update_list = array_list_create(sizeof(uint64_t));
    world->loaded_entity_list = array_list_create(sizeof(uint64_t));
    world->next_tick_data_list = array_list_create(sizeof(uint64_t));
    world->loaded_tile_entity_list = array_list_create(sizeof(uint64_t));
    world->renderer_world_list = array_list_create(sizeof(uint64_t));
    world->world_time = 0;
    world->sky_color = 0xFF99CCFF;
    world->fog_color = 0xFFB0D0FF;
    world->cloud_color = 0xFFFFFFFF;
    world->skylight_subtracted = 0;
    world->random = random_create(seed);
    world->random_number = random_next_int(&world->random);
    world->random_seed = seed;
    world->size_on_disk = 0;
    world->is_new_world = 1;
    strncpy(world->save_file, saves_dir, sizeof(world->save_file));
    strcat(world->save_file, "/");
    strcat(world->save_file, world_name);
    world->spawn_x = 512;
    world->spawn_y = 64;
    world->spawn_z = 512;

    for(int i = 0; i <= 15; i++) {
        float value = 1.0 - (float)i / 15.0;
        world->light_brightness_table[i] = (1.0 - value) / (value * 3.0 + 1.0) * 0.95 + 0.05;
    }

    // checks if file exists already
    struct stat sb;
    char folder[290];
    snprintf(folder, sizeof(folder), "%s/level.dat", world->save_file);
    if(stat(folder, &sb) == 0) {
        world->is_new_world = 0;
    }

    if(!world->is_new_world) {
        FILE *file = fopen(folder, "rb");
        printf("Opening existing world: %s\n", folder);
        nbt_base_t nbt = progress_bar_read(file);
        fclose(file);
        nbt = nbt_tag_compound_get_compound_tag(&nbt, "Data");
        world->random_seed = nbt_tag_compound_get_int(&nbt, "RandomSeed");
        //world->spawn_x = nbt_tag_compound_get_int(&nbt, "SpawnX");
        //world->spawn_y = nbt_tag_compound_get_int(&nbt, "SpawnY");
        //world->spawn_z = nbt_tag_compound_get_int(&nbt, "SpawnZ");
        world->world_time = nbt_tag_compound_get_long(&nbt, "Time");
        world->size_on_disk = nbt_tag_compound_get_long(&nbt, "SizeOnDisk");
        world->player_nbt = nbt_tag_compound_get_compound_tag(&nbt, "Player");
    }

    if(world->random_seed == 0) {
        world->random_seed = seed;
        world->spawn_x = 0;
        world->spawn_y = 64;
        world->spawn_z = 0;
    }

    chunk_provider_generate_create(&world->chunk_provider_gen, world, world->random_seed);
    chunk_provider_load_create(&world->chunk_provider, &world->chunk_provider_gen, world, world->save_file);
    world_save(world, 0);
}

nbt_base_t world_get_nbt_tag(char *game_dir, char *world_name) {
    char path[256];
    snprintf(path, sizeof(path), "%s/saves/%s/level.dat", game_dir, world_name);
    FILE *file = fopen(path, "rb");
    if(!file) {
        return (nbt_base_t){ .null = 1 };
    }
    nbt_base_t nbt = progress_bar_read(file);
    nbt_base_t data_nbt = nbt_tag_compound_get_compound_tag(&nbt, "Data");

    fclose(file);

    return data_nbt;
}

void world_spawn_player(world_t *world) {
    
}

char *private_dirname(char *path) {
#ifdef _WIN32
    static char buffer[1024];
    strncpy(buffer, path, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *last_separator = strrchr(buffer, PATH_SEPARATOR);
    if(last_separator) {
        *last_separator = '\0';
    } else {
        return ".";
    }
    return buffer;
#else
    return dirname(path);
#endif
}

int private_create_directories_world(const char *path) {
    char temp_path[1024];
    strncpy(temp_path, path, sizeof(temp_path));
    temp_path[sizeof(temp_path) - 1] = '\0';

    char *current = temp_path;
    char *next = NULL;

    char *dir_path = private_dirname(temp_path);

    while((next = strchr(current, '/')) != NULL) {
        *next = '\0';

        if(strlen(dir_path) > 0) {
#ifdef _WIN32
            if(!CreateDirectory(dir_path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
                fprintf(stderr, "Failed to create world directory: %s\n", dir_path);
                return -1;
            }
#else
            if(mkdir(dir_path, 0755) != 0 && errno != EEXIST) {
                perror("mkdir");
                return -1;
            }
#endif
        }

        *next = '/';
        current = next + 1;
    }

#ifdef _WIN32
    if(!CreateDirectory(dir_path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        fprintf(stderr, "Failed to create world directory: %s\n", dir_path);
        return -1;
    }
#else
    if(mkdir(dir_path, 0755) != 0 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }
#endif

    return 0;
}

void world_save(world_t *world, uint8_t check_entities) {
    char path[290];
    snprintf(path, sizeof(path), "%s/level.dat", world->save_file);
    printf("%s\n", path);
    FILE *file = fopen(path, "wb");
    if(!file) {
        private_create_directories_world(path);
        file = fopen(path, "wb");
    }

    nbt_base_t nbt = nbt_tag_compound_create();
    nbt_tag_compound_set_long(&nbt, "RandomSeed", world->random_seed);
    nbt_tag_compound_set_int(&nbt, "SpawnX", world->spawn_x);
    nbt_tag_compound_set_int(&nbt, "SpawnY", world->spawn_y);
    nbt_tag_compound_set_int(&nbt, "SpawnZ", world->spawn_z);
    nbt_tag_compound_set_long(&nbt, "Time", world->world_time);
    nbt_tag_compound_set_long(&nbt, "SizeOnDisk", world->size_on_disk);
    nbt_tag_compound_set_long(&nbt, "LastPlayed", time(NULL));
    if(world->player != NULL) {
        nbt_base_t player_nbt = nbt_tag_compound_create();
        // TODO: world->player->write_to_nbt(world->player, &player_nbt);
        nbt_tag_compound_set_compound_tag(&nbt, "Player", &player_nbt);
    }

    nbt_base_t base_nbt = nbt_tag_compound_create();
    nbt_tag_compound_set_tag(&base_nbt, "Data", &nbt);

    progress_bar_write(file, &base_nbt);
    fclose(file);

    chunk_provider_load_save_chunks(&world->chunk_provider, check_entities);
    nbt_tag_compound_free(&base_nbt);
}

uint8_t world_get_block(world_t *world, int x, int y, int z) {
    if(x < -WORLD_MAX_SIZE || x >= WORLD_MAX_SIZE 
        || y >= CHUNK_SIZE_HEIGHT
        || z < -WORLD_MAX_SIZE || z > WORLD_MAX_SIZE) {
        return 0;
    }

    // Lake of lava at bottom of world
    if(y <= 0) return BLOCK_LAVA;

    return chunk_get_block_id(world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH), 
                              x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1));
}

uint8_t world_block_exists(world_t *world, int x, int y, int z) {
    return y >= 0 && y < CHUNK_SIZE_HEIGHT ? world_chunk_exists(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH) : 0;
}

uint8_t world_chunk_exists(world_t *world, int x, int z) {
    return world->chunk_provider.chunk_exists(&world->chunk_provider, x, z);
}

struct chunk_s *world_get_chunk(world_t *world, int x, int z) {
    return world->chunk_provider.chunk_provide(&world->chunk_provider, x, z);
}

uint8_t world_set_block_no_update(world_t *world, int x, int y, int z, uint8_t block_id) {
    if(x < -WORLD_MAX_SIZE || x >= WORLD_MAX_SIZE
        || y < 0 || y >= CHUNK_SIZE_HEIGHT
        || z < -WORLD_MAX_SIZE || z > WORLD_MAX_SIZE) {
        return 0;
    }
    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    return chunk_set_block(chunk, x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1), block_id);
}

material_t *world_get_block_material(world_t *world, int x, int y, int z) {
    uint8_t block_id = world_get_block(world, x, y, z);
    return block_id == 0 ? &materials.air : block_list[block_id].material;
}

uint8_t world_get_block_metadata(world_t *world, int x, int y, int z) {
    if(x < -WORLD_MAX_SIZE || x >= WORLD_MAX_SIZE
        || y < 0 || y >= CHUNK_SIZE_HEIGHT
        || z < -WORLD_MAX_SIZE || z > WORLD_MAX_SIZE) {
        return 0;
    }
    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    return chunk_get_block_metadata(chunk,x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1));
}

uint8_t world_set_block_metadata(world_t *world, int x, int y, int z, uint8_t data) {
    if(x < -WORLD_MAX_SIZE || x >= WORLD_MAX_SIZE
        || y < 0 || y >= CHUNK_SIZE_HEIGHT
        || z < -WORLD_MAX_SIZE || z > WORLD_MAX_SIZE) {
        return 0;
    }
    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    chunk_set_block_metadata(chunk, x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1), data);
    return 1;
}

uint8_t world_set_block_with_update(world_t *world, int x, int y, int z, uint8_t block_id) {
    if(!world_set_block_no_update(world, x, y, z, block_id)) return 0;

    for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
        renderer_world_t *renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, i);
        renderer_world_update_block(renderer, x, y, z);
    }

    world_update_neighbors_at(world, x, y, z, block_id);
    return 1;
}

void world_mark_blocks_dirty_vertical(world_t *world, int x, int z, int y1, int y2) {
    if(y1 > y2) {
        int swap = y2;
        y2 = y1;
        y1 = swap;
    }

    for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
        renderer_world_t *renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, i);
        renderer_world_update_blocks(renderer, x, y1, z, x, y2, z);
    }
}

void world_swap(world_t *world, int x0, int y0, int z0, int x1, int y1, int z1) {
    uint8_t t1 = world_get_block(world, x0, y0, z0);
    uint8_t m1 = world_get_block_metadata(world, x0, y0, z0);
    uint8_t t2 = world_get_block(world, x1, y1, z1);
    uint8_t m2 = world_get_block_metadata(world, x1, y1, z1);
    world_set_block_no_update(world, x0, y0, z0, t2);
    world_set_block_metadata(world, x0, y0, z0, m2);
    world_set_block_no_update(world, x1, y1, z1, t1);
    world_set_block_metadata(world, x1, y1, z1, m1);
    world_update_neighbors_at(world, x0, y0, z0, t2);
    world_update_neighbors_at(world, x1, y1, z1, t1);
}

void world_update_neighbors_at(world_t *world, int x, int y, int z, uint8_t block_id) {
    world_update_block(world, x - 1, y, z, block_id);
    world_update_block(world, x + 1, y, z, block_id);
    world_update_block(world, x, y - 1, z, block_id);
    world_update_block(world, x, y + 1, z, block_id);
    world_update_block(world, x, y, z - 1, block_id);
    world_update_block(world, x, y, z + 1, block_id);
}

void world_update_block(world_t *world, int x, int y, int z, uint8_t block_id) {
    block_t *block = &block_list[world_get_block(world, x, y, z)];
    if(block->id != blocks.air.id) { 
        block->on_neighbor_changed(block, (struct world_s *)world, x, y, z, block_id);
    }
}

uint8_t world_can_block_see_sky(world_t *world, int x, int y, int z) {
    return chunk_can_block_see_sky(world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH), x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1));
}

uint8_t private_world_get_block_light_value(world_t *world, int x, int y, int z, uint8_t check_neighbors) {
    if(x < -WORLD_MAX_SIZE || x >= WORLD_MAX_SIZE
        || z < -WORLD_MAX_SIZE || z > WORLD_MAX_SIZE) {
        return 15;
    }

    if(check_neighbors) {
        uint8_t block_id = world_get_block(world, x, y, z);
        if(block_id == BLOCK_SLAB || block_id == BLOCK_FARMLAND) {
            // I have no idea if these cardinal directions are right but here we are
            uint8_t light_top = private_world_get_block_light_value(world, x, y + 1, z, 0);
            uint8_t light_east = private_world_get_block_light_value(world, x + 1, y, z, 0);
            uint8_t light_west = private_world_get_block_light_value(world, x - 1, y, z, 0);
            uint8_t light_north = private_world_get_block_light_value(world, x, y, z + 1, 0);
            uint8_t light_south = private_world_get_block_light_value(world, x, y, z - 1, 0);

            if(light_east > light_top) light_top = light_east;
            if(light_west > light_top) light_top = light_west;
            if(light_north > light_top) light_top = light_north;
            if(light_south > light_top) light_top = light_south;

            return light_top;
        }
    }

    if(y < 0) return 0;
    if(y >= CHUNK_SIZE_HEIGHT) {
        int16_t light = 15 - world->skylight_subtracted;
        if(light < 0) light = 0;
        return light;
    }
    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    return chunk_get_block_light_value(chunk, x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1), world->skylight_subtracted);
}

uint8_t world_get_block_light_value(world_t *world, int x, int y, int z) {
    return private_world_get_block_light_value(world, x, y, z, 1);
}

uint8_t world_can_existing_block_see_sky(world_t *world, int x, int y, int z) {
    if(x < -WORLD_MAX_SIZE || x >= WORLD_MAX_SIZE
        || y < 0
        || z < -WORLD_MAX_SIZE || z > WORLD_MAX_SIZE) {
        return 0;
    }
    if(y >= CHUNK_SIZE_HEIGHT) return 1;
    if(!world_chunk_exists(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH)) return 0;

    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    return chunk_can_block_see_sky(chunk, x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1));
}

int world_get_height_value(world_t *world, int x, int z) {
    if(x < -WORLD_MAX_SIZE || x >= WORLD_MAX_SIZE
        || z < -WORLD_MAX_SIZE || z > WORLD_MAX_SIZE) {
        return 0;
    }
    if(!world_chunk_exists(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH)) return 0;

    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    return chunk_get_height_value(chunk, x & (CHUNK_SIZE_WIDTH - 1), z & (CHUNK_SIZE_WIDTH - 1));
}

void world_light_changed(world_t *world, uint8_t light_type, int x, int y, int z, int value) {
    if(!world_block_exists(world, x, y, z)) return;

    if(light_type == LIGHT_TYPE_SKY) {
        if(world_can_existing_block_see_sky(world, x, y, z)) {
            value = 15;
        }
    }else if(light_type == LIGHT_TYPE_BLOCK) {
        uint8_t block_id = world_get_block(world, x, y, z);
        if(block_list[block_id].light_value > value) {
            value = block_list[block_id].light_value;
        }
    }

    if(world_get_saved_light_value(world, light_type, x, y, z) != value) {
        world_schedule_light_update(world, light_type, x, y, z, x, y, z);
    }
}

int world_get_saved_light_value(world_t *world, uint8_t light_type, int x, int y, int z) {
    if(x < -WORLD_MAX_SIZE || x >= WORLD_MAX_SIZE
        || y < 0 || y >= CHUNK_SIZE_HEIGHT
        || z < -WORLD_MAX_SIZE || z > WORLD_MAX_SIZE) {
        return light_type;
    }

    if(!world_chunk_exists(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH)) return 0;

    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    return chunk_get_saved_light_value(chunk, light_type, x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1));
}

float world_get_brightness(world_t *world, int x, int y, int z) {
    int light_level = world_get_block_light_value(world, x, y, z);
    return world->light_brightness_table[light_level];
}

uint8_t world_is_daytime(world_t *world) {
    return world->skylight_subtracted < 8;
}

hit_result_t world_clip(world_t *world, vec3_t v0, vec3_t v1) {
    if(v0.x == NAN || v0.y == NAN || v0.z == NAN) return (hit_result_t){ .null = 1 };
    if(v1.x == NAN || v1.y == NAN || v1.z == NAN) return (hit_result_t){ .null = 1 };
    int i0x = v0.x, i0y = v0.y, i0z = v0.z;
    int i1x = v1.x, i1y = v1.y, i1z = v1.z;
    
    int i = 20;
    while(i-- >= 0) {
        if(v0.x == NAN || v0.y == NAN || v0.z == NAN) return (hit_result_t){ .null = 1 };
        if(i0x == i1x && i0y == i1y && i0z == i1z) return (hit_result_t){ .null = 1 };
        vec3_t a = { 999.0, 999.0, 999.0 };
        if(i1x > i0x) a.x = i0x + 1.0;
        if(i1x < i0x) a.x = i0x;
        if(i1y > i0y) a.y = i0y + 1.0;
        if(i1y < i0y) a.y = i0y;
        if(i1z > i0z) a.z = i0z + 1.0;
        if(i1z < i0z) a.z = i0z;
        vec3_t b = { 999.0, 999.0, 999.0 };
        vec3_t d = vec3_subtract(v1, v0);
        if(a.x != 999.0) b.x = (a.x - v0.x) / d.x;
        if(a.y != 999.0) b.y = (a.y - v0.y) / d.y;
        if(a.z != 999.0) b.z = (a.z - v0.z) / d.z;
        int c;
        if(b.x < b.y && b.x < b.z) {
            c = i1x > i0x ? 4 : 5;
            v0.x = a.x;
            v0.y += d.y * b.x;
            v0.z += d.z * b.x;
        }else if(b.y < b.z) {
            c = i1y > i0y ? 0 : 1;
            v0.y = a.y;
            v0.x += d.x * b.y;
            v0.z += d.z * b.y;
        }else {
            c = i1z > i0z ? 2 : 3;
            v0.z = a.z;
            v0.x += d.x * b.z;
            v0.y += d.y * b.z;
        }
        
        vec3_t v00 = { (int)v0.x, (int)v0.y, (int)v0.z };
        i0x = v00.x;
        i0y = v00.y;
        i0z = v00.z;
        if(c == 5) {
            i0x--;
            v00.x++;
        }
        if(c == 1) {
            i0y--;
            v00.y++;
        }
        if(c == 3) {
            i0z--;
            v00.z++;
        }
        uint8_t block_id = world_get_block(world, i0x, i0y, i0z);
        block_t *block = &block_list[block_id];
        if(block_id != blocks.air.id && block->is_collidable) {
            hit_result_t pos = block->clip(block, world, i0x, i0y, i0z, v0, v1);
            if(!pos.null) return pos;
        }
    }
    return (hit_result_t){ .null = 1 };
}

void world_play_sound_at_entity(world_t *world, entity_t *entity, uint8_t sound, float volume, float pitch) {
    for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
        float attenuation = 16;
        if(volume > 1) attenuation *= volume;

        if(entity_distance_to_sqr(&world->player->mob.entity, entity) < attenuation * attenuation) {
            renderer_world_t *renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, i);
            renderer_world_play_sound(renderer, sound, entity->x, entity->y - entity->y_slide_offset, entity->z, volume, pitch);
        }
    }
}

void world_play_sound(world_t *world, double x, double y, double z, uint8_t sound, float volume, float pitch) {
    for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
        float attenuation = 16;
        if(volume > 1) attenuation *= volume;

        double xx = x - world->player->x;
        double yy = y - world->player->y;
        double zz = z - world->player->z;
        if(xx * xx + yy * yy + zz * zz < attenuation * attenuation) {
            renderer_world_t *renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, i);
            renderer_world_play_sound(renderer, sound, x, y, z, volume, pitch);
        }
    }
}

void world_spawn_particle(world_t *world, uint8_t particle_type, double x, double y, double z, double x_vel, double y_vel, double z_vel) {
    for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
        renderer_world_t *renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, i);
        renderer_world_spawn_particle(renderer, particle_type, x, y, z, x_vel, y_vel, z_vel);
    }
}

void world_spawn_entity(world_t *world, entity_t *entity) {
    int x = floor_double(entity->x / CHUNK_SIZE_WIDTH);
    int z = floor_double(entity->z / CHUNK_SIZE_WIDTH);
    printf("Spawning entity in chunk %d, %d\n", x, z);
    if(!world_chunk_exists(world, x, z)) {
        printf("Failed to add entity\n");
        return;
    }
    chunk_t *chunk = world_get_chunk(world, x, z);
    chunk_add_entity(chunk, entity);
    world->loaded_entity_list = array_list_push(world->loaded_entity_list, &entity);

    for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
        renderer_world_t *renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, i);
        renderer_world_obtain_entity_skin(renderer, entity);
    }
}

void world_set_entity_dead(world_t *world, entity_t *entity) {
    entity->is_dead = 1;
}

void world_add_renderer(world_t *world, renderer_world_t *renderer) {
    world->renderer_world_list = array_list_push(world->renderer_world_list, &renderer);
}

void world_remove_renderer(world_t *world, renderer_world_t *renderer) {
    int index = array_list_index_of(world->renderer_world_list, renderer);
    if(index == -1) return;
    world->renderer_world_list = array_list_remove(world->renderer_world_list, index);
}

AABB_t *world_get_cubes(world_t *world, AABB_t box) {
    AABB_t *list = array_list_create(sizeof(AABB_t));
    int x0 = floor_double(box.x0);
    int y0 = floor_double(box.y0);
    int z0 = floor_double(box.z0);
    int x1 = floor_double(box.x1 + 1);
    int y1 = floor_double(box.y1 + 1);
    int z1 = floor_double(box.z1 + 1);

    for(int i = x0; i < x1; i++) {
        for(int j = y0; j < y1; j++) {
            for(int k = z0; k < z1; k++) {
                AABB_t bb = { 0 };
                
                block_t *block = &block_list[world_get_block(world, i, j, k)];
                if(block->id != blocks.air.id) {
                    bb = block->get_collision_aabb(block, i, j, k);
                    if(!bb.null && AABB_intersects_inner(box, bb)) {
                        list = array_list_push(list, &bb);
                    }
                }
            }
        }
    }
    return list;
}

vec3_t world_get_sky_color(world_t *world, float partial_tick) {
    float angle = world_get_celestial_angle(world, partial_tick);
    angle = tcos(angle * M_PI * 2.0) * 2.0 + 0.5;

    if(angle < 0.0) angle = 0.0;
    if(angle > 1.0) angle = 1.0;

    float r = (world->sky_color >> 16 & 0xFF) / 255.0;
    float g = (world->sky_color >> 8 & 0xFF) / 255.0;
    float b = (world->sky_color & 0xFF) / 255.0;

    r *= angle;
    g *= angle;
    b *= angle;

    return (vec3_t){ r, g, b };
}

float world_get_celestial_angle(world_t *world, float partial_tick) {
    int time = world->world_time % 24000;
    float angle = (time + partial_tick) / 24000.0 - 0.15;

    return angle;
}

vec3_t world_get_cloud_color(world_t *world, float partial_tick) {
    float angle = world_get_celestial_angle(world, partial_tick);
    angle = tcos(angle * M_PI * 2.0) * 2.0 + 0.5;

    if(angle < 0.0) angle = 0.0;
    if(angle > 1.0) angle = 1.0;

    float r = (world->cloud_color >> 16 & 0xFF) / 255.0;
    float g = (world->cloud_color >> 8 & 0xFF) / 255.0;
    float b = (world->cloud_color & 0xFF) / 255.0;

    r *= angle * 0.9 + 0.1;
    g *= angle * 0.9 + 0.1;
    b *= angle * 0.85 + 0.15;

    return (vec3_t){ r, g, b };
}

vec3_t world_get_fog_color(world_t *world, float partial_tick) {
    float angle = world_get_celestial_angle(world, partial_tick);
    angle = tcos(angle * M_PI * 2.0) * 2.0 + 0.5;

    if(angle < 0.0) angle = 0.0;
    if(angle > 1.0) angle = 1.0;

    float r = (world->fog_color >> 16 & 0xFF) / 255.0;
    float g = (world->fog_color >> 8 & 0xFF) / 255.0;
    float b = (world->fog_color & 0xFF) / 255.0;

    r *= angle * 0.94 + 0.06;
    g *= angle * 0.94 + 0.06;
    b *= angle * 0.91 + 0.09;

    return (vec3_t){ r, g, b };
}

float world_get_star_brightness(world_t *world, float partial_tick) {
    float angle = world_get_celestial_angle(world, partial_tick);
    angle = 1.0 - (tcos(angle * M_PI * 2.0) * 2.0 + (12.0 / 16.0));

    if(angle < 0.0) angle = 0.0;
    if(angle > 1.0) angle = 1.0;

    return angle * angle * 0.5;
}

void world_schedule_block_update(world_t *world, int x, int y, int z, uint8_t block_id) {
    next_tick_data_t next_tick = { 0 };
    next_tick.x = x;
    next_tick.y = y;
    next_tick.z = z;
    next_tick.block_id = block_id;
    if(block_id > 0) {
        next_tick.delay = block_list[block_id].tick_rate;
    }

    world->next_tick_data_list = array_list_push(world->next_tick_data_list, &next_tick);
}

void world_update_entities(world_t *world) {
    for(int i = 0; i < array_list_length(world->loaded_entity_list); i++) {
        entity_t *entity = *(entity_t **)array_list_get(world->loaded_entity_list, i);
        if(!entity->is_dead) {
            int cx = floor_double(entity->x / CHUNK_SIZE_WIDTH);
            int cy = floor_double(entity->y / CHUNK_SIZE_WIDTH);
            int cz = floor_double(entity->z / CHUNK_SIZE_WIDTH);

            entity->last_tick_x = entity->x;
            entity->last_tick_y = entity->y;
            entity->last_tick_z = entity->z;
            entity->x_roto = entity->x_rot;
            entity->y_roto = entity->y_rot;
            if(world_chunk_exists(world, cx, cz)) entity->tick(entity);

            int cx2 = floor_double(entity->x / CHUNK_SIZE_WIDTH);
            int cy2 = floor_double(entity->y / CHUNK_SIZE_WIDTH);
            int cz2 = floor_double(entity->z / CHUNK_SIZE_WIDTH);

            if(cx2 != cx || cy2 != cy || cz2 != cz) {
                if(world_chunk_exists(world, cx, cz)) {
                    chunk_t *old_chunk = world_get_chunk(world, cx, cz);
                    chunk_remove_entity_index(old_chunk, entity, cy2);
                }
                if(world_chunk_exists(world, cx2, cz2)) {
                    chunk_t *new_chunk = world_get_chunk(world, cx2, cz2);
                    chunk_add_entity(new_chunk, entity);
                }else {
                    entity->is_dead = 1;
                }
            }
        }

        if(entity->is_dead) {
            int cx = floor_double(entity->x / CHUNK_SIZE_WIDTH);
            int cz = floor_double(entity->z / CHUNK_SIZE_WIDTH);
            
            if(world_chunk_exists(world, cx, cz)) {
                chunk_t *chunk = world_get_chunk(world, cx, cz);
                chunk_remove_entity_index(chunk, entity, floor_double(entity->y / CHUNK_SIZE_WIDTH));
            }

            world->loaded_entity_list = array_list_remove(world->loaded_entity_list, i);
            i--;

            for(int j = 0; j < array_list_length(world->renderer_world_list); j++) {
                renderer_world_t *renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, j);
                renderer_world_release_entity_skin(renderer, entity);
            }
        }
    }

    for(int i = 0; i < array_list_length(world->loaded_tile_entity_list); i++) {
        tile_entity_t *tile_entity = *(tile_entity_t **)array_list_get(world->loaded_tile_entity_list, i);
        tile_entity->update(tile_entity);
    }
}

uint8_t world_is_aabb_clear(world_t *world, AABB_t box) {
    entity_t **entities = world_get_entities_excluding(world, NULL, box);
    for(int i = 0; i < array_list_length(entities); i++) {
        entity_t *entity = *(entity_t **)array_list_get(entities, i);
        if(entity->prevents_spawning) {
            array_list_free(entities);
            return 0;
        }
    }

    array_list_free(entities);

    return 1;
}

uint8_t world_is_liquid_in_range(world_t *world, AABB_t box) {
    int x0 = floor_double(box.x0);
    int y0 = floor_double(box.y0);
    int z0 = floor_double(box.z0);
    int x1 = floor_double(box.x1 + 1);
    int y1 = floor_double(box.y1 + 1);
    int z1 = floor_double(box.z1 + 1);

    if(box.x0 < 0.0) x0--;
    if(box.y0 < 0.0) y0--;
    if(box.z0 < 0.0) z0--;

    for(int i = x0; i < x1; i++) {
        for(int j = y0; j < y1; j++) {
            for(int k = z0; k < z1; k++) {
                block_t *block = &block_list[world_get_block(world, i, j, k)];
                if(block->id != blocks.air.id && block->material->is_liquid) return 1;
            }
        }
    }

    return 0;
}

uint8_t world_is_bounding_box_burning(world_t *world, AABB_t box) {
    int x0 = floor_double(box.x0);
    int y0 = floor_double(box.y0);
    int z0 = floor_double(box.z0);
    int x1 = floor_double(box.x1 + 1);
    int y1 = floor_double(box.y1 + 1);
    int z1 = floor_double(box.z1 + 1);

    for(int i = x0; i < x1; i++) {
        for(int j = y0; j < y1; j++) {
            for(int k = z0; k < z1; k++) {
                block_t *block = &block_list[world_get_block(world, i, j, k)];
                if(block->id == BLOCK_FIRE || block->id == BLOCK_LAVA || block->id == BLOCK_STILL_LAVA) return 1;
            }
        }
    }

    return 0;
}

uint8_t world_is_material_in_box(world_t *world, AABB_t box, material_t *material) {
    int x0 = floor_double(box.x0);
    int y0 = floor_double(box.y0);
    int z0 = floor_double(box.z0);
    int x1 = floor_double(box.x1 + 1);
    int y1 = floor_double(box.y1 + 1);
    int z1 = floor_double(box.z1 + 1);

    for(int i = x0; i < x1; i++) {
        for(int j = y0; j < y1; j++) {
            for(int k = z0; k < z1; k++) {
                block_t *block = &block_list[world_get_block(world, i, j, k)];
                if(block->id != blocks.air.id && block->material->id == material->id) return 1;
            }
        }
    }

    return 0;
}

void world_explode(world_t *world, entity_t *source_entity, double x, double y, double z, float radius) {
    world_play_sound(world, x, y, z, SOUND_RANDOM_EXPLODE, 4.0, (1.0 + (random_next_uniform(&world->random) - random_next_uniform(&world->random)) * 0.2) * 0.7);
    vec3_t *affected_blocks = array_list_create(sizeof(vec3_t));

    for(int x_offset = 0; x_offset < CHUNK_SIZE_WIDTH; x_offset++) {
        for(int y_offset = 0; y_offset < CHUNK_SIZE_WIDTH; y_offset++) {
            for(int z_offset = 0; z_offset < CHUNK_SIZE_WIDTH; z_offset++) {
                if(x_offset == 0 || x_offset == CHUNK_SIZE_WIDTH - 1
                   || y_offset == 0 || y_offset == CHUNK_SIZE_WIDTH - 1
                   || z_offset == 0 || z_offset == CHUNK_SIZE_WIDTH - 1) {
                    double x_norm = x_offset / (CHUNK_SIZE_WIDTH - 1) * 2.0 - 1.0;
                    double y_norm = y_offset / (CHUNK_SIZE_WIDTH - 1) * 2.0 - 1.0;
                    double z_norm = z_offset / (CHUNK_SIZE_WIDTH - 1) * 2.0 - 1.0;
                    double distance = sqrt(x_norm * x_norm + y_norm * y_norm + z_norm * z_norm);

                    x_norm /= distance;
                    y_norm /= distance;
                    z_norm /= distance;

                    float strength = radius * (0.7 + random_next_uniform(&world->random) * 0.6);
                    double x_ray = x;
                    double y_ray = y;
                    double z_ray = z;
                    
                    for(; strength > 0; strength -= 0.22500001) {
                        int block_x = floor_double(x_ray);
                        int block_y = floor_double(y_ray);
                        int block_z = floor_double(z_ray);

                        uint8_t block_id = world_get_block(world, block_x, block_y, block_z);
                        if(block_id > 0) {
                            strength -= ((block_list[block_id].resistance / 5.0) + 0.3) * 0.3;
                        }

                        if(strength > 0) {
                            vec3_t pos = { block_x, block_y, block_z };
                            affected_blocks = array_list_push(affected_blocks, &pos);
                        }

                        x_ray += x_norm * 0.3;
                        y_ray += y_norm * 0.3;
                        z_ray += z_norm * 0.3;
                    }
                }
            }
        }
    }

    radius *= 2;
    int x0 = floor_double(x - radius) - 1;
    int x1 = floor_double(x + radius) + 1;
    int y0 = floor_double(y - radius) - 1;
    int y1 = floor_double(y + radius) + 1;
    int z0 = floor_double(z - radius) - 1;
    int z1 = floor_double(z + radius) + 1;

    entity_t **affected_entities = world_get_entities_excluding(world, source_entity, (AABB_t){ x0, y0, z0, x1, y1, z1 });
    vec3_t origin = { x, y, z };

    for(int i = 0; i < array_list_length(affected_entities); i++) {
        entity_t *entity = *(entity_t **)array_list_get(affected_entities, i);
        double dx = entity->x - x;
        double dy = entity->y - y;
        double dz = entity->z - z;
        double distance = sqrt(dx * dx + dy * dy + dz * dz);

        if(distance <= 1) {
            dx /= distance;
            dy /= distance;
            dz /= distance;

            double density = world_get_block_density(world, origin, entity->bb);
            double impact = (1 - distance) * density;

            entity_hurt(entity, source_entity, (impact * impact + impact) / 2.0 * 8.0 * radius + 1);
            entity->xd += dx * impact;
            entity->yd += dy * impact;
            entity->zd += dz * impact;
        }
    }

    array_list_free(affected_entities);

    for(int i = 0; i < array_list_length(affected_blocks); i++) {
        vec3_t *pos = array_list_get(affected_blocks, i);
        int block_x = pos->x;
        int block_y = pos->y;
        int block_z = pos->z;

        uint8_t block_id = world_get_block(world, block_x, block_y, block_z);
        for(int j = 0; j < 1; j++) {
            double particle_x = block_x + random_next_uniform(&world->random);
            double particle_y = block_y + random_next_uniform(&world->random);
            double particle_z = block_z + random_next_uniform(&world->random);
            double vel_x = particle_x - x;
            double vel_y = particle_y - y;
            double vel_z = particle_z - z;

            double magnitude = sqrt(vel_x * vel_x + vel_y * vel_y + vel_z * vel_z);
            vel_x /= magnitude;
            vel_y /= magnitude;
            vel_z /= magnitude;

            double particle_vel = 0.5 / (magnitude / radius + 0.1);
            particle_vel *= (random_next_uniform(&world->random) * random_next_uniform(&world->random) + 0.3);
            vel_x *= particle_vel;
            vel_y *= particle_vel;
            vel_z *= particle_vel;

            world_spawn_particle(world, PARTICLE_EXPLODE, (particle_x + x) / 2.0, (particle_y + y) / 2.0, (particle_z + z) / 2.0, vel_x, vel_y, vel_z);
            world_spawn_particle(world, PARTICLE_SMOKE, particle_x, particle_y, particle_z, vel_x, vel_y, vel_z);
        }

        if(block_id > 0) {
            block_t *block = &block_list[block_id];
            block_spawn_items_chance(block, world, x, y, z, world_get_block_metadata(world, block_x, block_y, block_z), 0.3);
            world_set_block_with_update(world, block_x, block_y, block_z, blocks.air.id);
            block->on_exploded(block, world, x, y, z);
        }
    }

    array_list_free(affected_blocks);
}

float world_get_block_density(world_t *world, vec3_t pos, AABB_t box) {
    double x = 1.0 / ((box.x1 - box.x0) * 2.0 + 1);
    double y = 1.0 / ((box.y1 - box.y0) * 2.0 + 1);
    double z = 1.0 / ((box.z1 - box.z0) * 2.0 + 1);

    int unobstructed_points = 0;
    int total_points = 0;

    for(float x_offset = 0; x_offset <= 1; x_offset = x_offset + x) {
        for(float y_offset = 0; y_offset <= 1; y_offset = y_offset + y) {
            for(float z_offset = 0; z_offset <= 1; z_offset = z_offset + z) {
                vec3_t sample_pos = { box.x0 + x_offset, box.y0 + y_offset, box.z0 + z_offset };
                if(world_clip(world, sample_pos, pos).null == 1) {
                    unobstructed_points++;
                }
                total_points++;
            }
        }
    }
    return (float)unobstructed_points / (float)total_points;
}

void world_extinguish_fire(world_t *world, int x, int y, int z, uint8_t side) {
    if(side == 0) y--;
    if(side == 1) y++;
    if(side == 2) z--;
    if(side == 3) z++;
    if(side == 4) x--;
    if(side == 5) x++;

    if(world_get_block(world, x, y, z) == BLOCK_FIRE) {
        world_play_sound(world, x + 0.5, y + 0.5, z + 0.5, SOUND_RANDOM_FIZZ, 0.5, 2.6 + (random_next_uniform(&world->random) - random_next_uniform(&world->random)) * 0.8);
        world_set_block_with_update(world, x, y, z, blocks.air.id);
    }
}

tile_entity_t *world_get_tile_entity(world_t *world, int x, int y, int z) {
    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    return chunk != NULL ? chunk_get_tile_entity(chunk, x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1)) : NULL;
}

void world_set_tile_entity(world_t *world, int x, int y, int z, tile_entity_t *tile_entity) {
    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    if(chunk != NULL) {
        chunk_set_tile_entity(chunk, x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1), tile_entity);
    }
}

void world_remove_tile_entity(world_t *world, int x, int y, int z) {
    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
    if(chunk != NULL) {
        chunk_remove_tile_entity(chunk, x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1));
    }
}

uint8_t world_is_solid(world_t *world, int x, int y, int z) {
    uint8_t block_id = world_get_block(world, x, y, z);
    return block_list[block_id].material->is_solid;
}

uint8_t world_update_lighting(world_t *world) {
    int iterations = 100000;

    while(array_list_length(world->lighting_update_list) > 0) {
        int length = array_list_length(world->lighting_update_list);
        iterations--;
        if(iterations <= 0) return 1;

        chunk_metadata_t *metadata = *(chunk_metadata_t **)array_list_get(world->lighting_update_list, length - 1);
        world->lighting_update_list = array_list_remove(world->lighting_update_list, length - 1);
        /*if(length % 1000 == 0)*/ printf("LENGTH %d %d %d %d\n", length, metadata->x, metadata->y, metadata->z);

        for(int x = metadata->x; x <= metadata->max_x; x++) {
            for(int z = metadata->z; z <= metadata->max_z; z++) {
                if(world_block_exists(world, x, 0, z)) {
                    for(int y = metadata->y; y <= metadata->max_y; y++) {
                        if(y >= 0 && y < CHUNK_SIZE_HEIGHT) {
                            int current_light_value = world_get_saved_light_value(world, metadata->light_type, x, y, z);
                            uint8_t block_id = world_get_block(world, x, y, z);
                            uint8_t opacity = block_list[block_id].light_opacity;
                            if(opacity == 0) opacity = 1;

                            int new_light_value = 0;
                            if(metadata->light_type == LIGHT_TYPE_SKY) {
                                if(world_can_existing_block_see_sky(world, x, y, z)) {
                                    new_light_value = 15;
                                }
                            }else if(metadata->light_type == LIGHT_TYPE_BLOCK) {
                                new_light_value = block_list[block_id].light_value;
                            }

                            int max_light = 0;

                            if(opacity >= 15 && new_light_value == 0) {
                                max_light = 0;
                            }/*else {
                                int light_west = world_get_saved_light_value(world, metadata->light_type, x - 1, y, z);
                                int light_east = world_get_saved_light_value(world, metadata->light_type, x + 1, y, z);
                                int light_north = world_get_saved_light_value(world, metadata->light_type, x, y, z - 1);
                                int light_south = world_get_saved_light_value(world, metadata->light_type, x, y, z + 1);
                                int light_up = world_get_saved_light_value(world, metadata->light_type, x, y + 1, z);
                                int light_down = world_get_saved_light_value(world, metadata->light_type, x, y - 1, z);

                                max_light = light_west;
                                if(light_east > max_light) max_light = light_east;
                                if(light_north > max_light) max_light = light_north;
                                if(light_south > max_light) max_light = light_south;
                                if(light_up > max_light) max_light = light_up;
                                if(light_down > max_light) max_light = light_down;

                                max_light -= opacity;
                                if(max_light < 0) max_light = 0;

                                if(new_light_value > max_light) {
                                    max_light = new_light_value;
                                }
                            }*/

                            if(current_light_value != max_light) {
                                if(x >= -WORLD_MAX_SIZE && x < WORLD_MAX_SIZE
                                && y >= 0 && y < CHUNK_SIZE_HEIGHT
                                && z >= -WORLD_MAX_SIZE && z <= WORLD_MAX_SIZE
                                && world_chunk_exists(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH)) {
                                    chunk_t *chunk = world_get_chunk(world, x / CHUNK_SIZE_WIDTH, z / CHUNK_SIZE_WIDTH);
                                    chunk_set_light_value(chunk, metadata->light_type, x & (CHUNK_SIZE_WIDTH - 1), y, z & (CHUNK_SIZE_WIDTH - 1), new_light_value);

                                    for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
                                        renderer_world_t *renderer = array_list_get(world->renderer_world_list, i);
                                        renderer_world_update_block(renderer, x, y, z);
                                    }
                                }

                                max_light--;
                                if(max_light < 0) max_light = 0;
                                
                                world_light_changed(world, metadata->light_type, x - 1, y, z, max_light);
                                world_light_changed(world, metadata->light_type, x, y - 1, z, max_light);
                                world_light_changed(world, metadata->light_type, x, y, z - 1, max_light);
                                if(x + 1 >= metadata->max_x) world_light_changed(world, metadata->light_type, x + 1, y, z, max_light);
                                if(y + 1 >= metadata->max_y) world_light_changed(world, metadata->light_type, x, y + 1, z, max_light);
                                if(z + 1 >= metadata->max_z) world_light_changed(world, metadata->light_type, x, y, z + 1, max_light);
                            }
                        }
                    }
                }
            }
        }
        free(metadata);
    }

    return 0;
}

void world_schedule_light_update(world_t *world, uint8_t light_type, int x0, int y0, int z0, int x1, int y1, int z1) {
    int update_count = array_list_length(world->lighting_update_list);
    int max_checks = 4;

    if(max_checks > update_count) max_checks = update_count;

    for(int i = 0; i < max_checks; i++) {
        chunk_metadata_t *metadata = *(chunk_metadata_t **)array_list_get(world->lighting_update_list, array_list_length(world->lighting_update_list) - i - 1);

        if(metadata->light_type == light_type) {
            uint8_t is_merged;
            if(x0 >= metadata->x && y0 >= metadata->y && z0 >= metadata->z && x1 <= metadata->max_x && y1 <= metadata->max_y && z1 <= metadata->max_z) {
                is_merged = 1;
            }else if(x0 >= metadata->x - 1 && y0 >= metadata->y - 1 && z0 >= metadata->z - 1 && x1 <= metadata->max_x + 1 && y1 <= metadata->max_y + 1 && z1 <= metadata->max_z + 1) {
                if(x0 < metadata->x) metadata->x = x0;
                if(y0 < metadata->y) metadata->y = y0;
                if(z0 < metadata->z) metadata->z = z0;
                if(x1 > metadata->max_x) metadata->max_x = x1;
                if(y1 > metadata->max_y) metadata->max_y = y1;
                if(z1 > metadata->max_z) metadata->max_z = z1;

                is_merged = 1;
            }else {
                is_merged = 0;
            }

            if(is_merged) return;
        }
    }

    chunk_metadata_t *new_metadata = malloc(sizeof(chunk_metadata_t));
    *new_metadata = (chunk_metadata_t){ light_type, x0, y0, z0, x1, y1, z1 };
    world->lighting_update_list = array_list_push(world->lighting_update_list, &new_metadata);

    if(array_list_length(world->lighting_update_list) > 1000000) {
        while(array_list_length(world->lighting_update_list) > 500000) {
            world_update_lighting(world);
        }
    }
}

void world_restart_time_of_day(world_t *world) {
    world->chunk_provider.unload_oldest_chunks(&world->chunk_provider);
    if(!array_list_contains(world->loaded_entity_list, &world->player)) {
        world_spawn_entity(world, &world->player->mob.entity);
    }

    float angle = world_get_celestial_angle(world, 1.0);
    angle = 1.0 - (tcos(angle * M_PI * 2.0) * 2.0 + 0.5);
    if(angle < 0) angle = 0;
    if(angle > 1) angle = 1;

    int light = angle * 13;
    if(light != world->skylight_subtracted) {
        world->skylight_subtracted = light;
        
        for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
            renderer_world_t *renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, i);
            renderer_world_update_all(renderer);
        }
    }

    world->world_time+=5;
    if(world->world_time % 100 == 0) {
        //world_save(world, 0);
    }

    int next_tick_size = array_list_length(world->next_tick_data_list);
    if(next_tick_size > 200) {
        next_tick_size = 200;
    }

    for(int i = 0; i < next_tick_size; i++) {
        next_tick_data_t next_tick = *(next_tick_data_t *)array_list_get(world->next_tick_data_list, 0);
        world->next_tick_data_list = array_list_remove(world->next_tick_data_list, 0);
        if(next_tick.delay > 0) {
            next_tick.delay--;
            world->next_tick_data_list = array_list_push(world->next_tick_data_list, &next_tick);
        }else if(world_block_exists(world, next_tick.x, next_tick.y, next_tick.z)) {
            uint8_t block_id = world_get_block(world, next_tick.x, next_tick.y, next_tick.z);
            if(block_id == next_tick.block_id && block_id > 0) {
                block_t *block = &block_list[block_id];
                block->update(block, world, next_tick.x, next_tick.y, next_tick.z, &world->random);
            }
        }
    }

    int x = floor_double(world->player->x);
    int z = floor_double(world->player->z);

    for(int i = 0; i < 32000; i++) {
        world->random_number = world->random_number * 3 + HASH_MAGIC;
        int yy = world->random_number >> 2;
        int xx = (yy & 0xFF) - 128 + x;
        int zz = (yy >> 8 & 0xFF) - 128 + z;
        yy = yy >> 16 & 127;
        if(!world_chunk_exists(world, xx / CHUNK_SIZE_WIDTH, zz / CHUNK_SIZE_WIDTH)) continue;
        uint8_t block_id = world_get_block(world, xx, yy, zz);
        if(block_list[block_id].should_tick) {
            block_list[block_id].update(&block_list[block_id], world, xx, yy, zz, &world->random);
        }
    }
}

void world_visual_update(world_t *world, int x, int y, int z) {
    random_t random = random_create(time(NULL));

    for(int i = 0; i < 1000; i++) {
        int xx = x + random_next_int_range(&random, 0, CHUNK_SIZE_WIDTH) - random_next_int_range(&random, 0, CHUNK_SIZE_WIDTH);
        int yy = y + random_next_int_range(&random, 0, CHUNK_SIZE_WIDTH) - random_next_int_range(&random, 0, CHUNK_SIZE_WIDTH);
        int zz = z + random_next_int_range(&random, 0, CHUNK_SIZE_WIDTH) - random_next_int_range(&random, 0, CHUNK_SIZE_WIDTH);
        uint8_t block_id = world_get_block(world, xx, yy, zz);
        if(block_id > 0) {
            block_list[block_id].visual_update(&block_list[block_id], world, xx, yy, zz, &random);
        }
    }
}

entity_t **world_get_entities_excluding(world_t *world, entity_t *entity, AABB_t box) {
    int x0 = floor_double((box.x0 - 2) / CHUNK_SIZE_WIDTH);
    int x1 = floor_double((box.x1 + 2) / CHUNK_SIZE_WIDTH);
    int z0 = floor_double((box.z0 - 2) / CHUNK_SIZE_WIDTH);
    int z1 = floor_double((box.z1 + 2) / CHUNK_SIZE_WIDTH);
    entity_t **entity_list = array_list_create(sizeof(entity_t *));

    for(int x = x0; x <= x1; x++) {
        for(int z = z0; z <= z1; z++) {
            if(world_chunk_exists(world, x, z)) {
                chunk_t *chunk = world_get_chunk(world, x, z);
                chunk_get_entities(chunk, entity, box, &entity_list);
            }
        }
    }

    return entity_list;
}

void world_add_loaded_entities(world_t *world, entity_t **entity_list) {
    for(int i = 0; i < array_list_length(entity_list); i++) {
        entity_t *entity = *(entity_t **)array_list_get(entity_list, i);
        world->loaded_entity_list = array_list_push(world->loaded_entity_list, &entity);
    }

    for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
        renderer_world_t *world_renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, i);
        for(int j = 0; j < array_list_length(entity_list); j++) {
            entity_t *entity = *(entity_t **)array_list_get(entity_list, j);
            renderer_world_obtain_entity_skin(world_renderer, entity);
        }
    }
}

void world_unload_entities(world_t *world, entity_t **entity_list) {
    for(int i = 0; i < array_list_length(entity_list); i++) {
        entity_t *entity = *(entity_t **)array_list_get(entity_list, i);
        int index = array_list_index_of(world->loaded_entity_list, &entity);
        world->loaded_entity_list = array_list_remove(world->loaded_entity_list, index);
    }

    for(int i = 0; i < array_list_length(world->renderer_world_list); i++) {
        renderer_world_t *world_renderer = *(renderer_world_t **)array_list_get(world->renderer_world_list, i);
        for(int j = 0; j < array_list_length(entity_list); j++) {
            entity_t *entity = *(entity_t **)array_list_get(entity_list, j);
            renderer_world_release_entity_skin(world_renderer, entity);
        }
    }
}

void world_set_spawn_position(world_t *world, int x, int y, int z) {
    world->spawn_x = x;
    world->spawn_y = y;
    world->spawn_z = z;
}

void world_destroy(world_t *world) {
    array_list_free(world->lighting_update_list);
    array_list_free(world->loaded_entity_list);
    array_list_free(world->next_tick_data_list);
    array_list_free(world->loaded_tile_entity_list);
    array_list_free(world->renderer_world_list);
}