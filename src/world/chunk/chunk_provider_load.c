#include <world/chunk/chunk_provider_load.h>

#include <world/chunk/chunk_provider.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

void chunk_provider_load_create(chunk_provider_t *chunk_provider_load, chunk_provider_t *chunk_provider_gen, world_t *world, const char **save_directory) {
    chunk_provider_load->world = world;
    chunk_provider_load->save_directory = save_directory;

    chunk_provider_load->chunk_exists = chunk_provider_load_chunk_exists;
    chunk_provider_load->chunk_provide = chunk_provider_load_provide_chunk;
    chunk_provider_load->populate = chunk_provider_load_populate;
    chunk_provider_load->save_chunks = chunk_provider_load_save_chunks;
    chunk_provider_load->unload_oldest_chunks = chunk_provider_load_unload_oldest_chunks;
}

uint8_t chunk_provider_load_chunk_exists(chunk_provider_t *chunk_provider, int x, int z) {
    int index = x & 31 | (z & 31) << 5;
    if(chunk_provider->chunks[index] != NULL) {
        chunk_t *chunk = chunk_provider->chunks[index];
        if(chunk->x_pos == x && chunk->z_pos == z) {
            return 1;
        }
    }
    return 0;
}

chunk_t *chunk_provider_load_provide_chunk(chunk_provider_t *chunk_provider, int x, int z) {
    int index = x & 31 | (z & 31) << 5;
    if(chunk_provider_load_chunk_exists(chunk_provider, x, z)) {
        return chunk_provider->chunks[index];
    }

    if(chunk_provider->chunks[index] != NULL) {
        chunk_unload_entities(chunk_provider->chunks[index]);
        chunk_provider_load_save_chunk(chunk_provider, chunk_provider->chunks[index]);
    }

    chunk_t *chunk = chunk_provider_load_load_chunk(chunk_provider, x, z);
    if(chunk == NULL) {
        chunk = chunk_provider->chunk_provider_gen->chunk_provide(chunk_provider->chunk_provider_gen, x, z);
    }

    chunk_provider->chunks[index] = chunk;
    if(chunk_provider->chunks[index] != NULL) {
        chunk_load_entities(chunk_provider->chunks[index]);
    }

    if(!chunk_provider->chunks[index]->is_terrain_populated && chunk_provider_load_chunk_exists(chunk_provider, x + 1, z + 1) 
       && chunk_provider_load_chunk_exists(chunk_provider, x, z + 1) && chunk_provider_load_chunk_exists(chunk_provider, x + 1, z)) {
        chunk_provider_load_populate(chunk_provider, chunk_provider, x, z);
    }

    if(chunk_provider_load_chunk_exists(chunk_provider, x - 1, z) && !chunk_provider_load_provide_chunk(chunk_provider, x - 1, z)->is_terrain_populated
       && chunk_provider_load_chunk_exists(chunk_provider, x - 1, z + 1) && chunk_provider_load_chunk_exists(chunk_provider, x, z + 1)
       && chunk_provider_load_chunk_exists(chunk_provider, x - 1, z)) {
        chunk_provider_load_populate(chunk_provider, chunk_provider, x - 1, z);
    }

    if(chunk_provider_load_chunk_exists(chunk_provider, x, z - 1) && !chunk_provider_load_provide_chunk(chunk_provider, x, z - 1)->is_terrain_populated
       && chunk_provider_load_chunk_exists(chunk_provider, x + 1, z - 1) && chunk_provider_load_chunk_exists(chunk_provider, x, z - 1)
       && chunk_provider_load_chunk_exists(chunk_provider, x + 1, z)) {
        chunk_provider_load_populate(chunk_provider, chunk_provider, x, z - 1);
    }

    if(chunk_provider_load_chunk_exists(chunk_provider, x - 1, z - 1) && !chunk_provider_load_provide_chunk(chunk_provider, x - 1, z - 1)->is_terrain_populated
       && chunk_provider_load_chunk_exists(chunk_provider, x - 1, z - 1) && chunk_provider_load_chunk_exists(chunk_provider, x, z - 1)
       && chunk_provider_load_chunk_exists(chunk_provider, x - 1, z)) {
        chunk_provider_load_populate(chunk_provider, chunk_provider, x - 1, z - 1);
    }
}

char *private_integer_to_string(int32_t value, int radix) {
    if (radix < 2 || radix > 36) {
        return NULL;
    }

    static char buffer[35]; 
    char *ptr = &buffer[34];
    *ptr = '\0';

    int is_negative = value < 0;
    uint32_t abs_value = is_negative ? -value : value;

    do {
        int digit = abs_value % radix;
        *ptr-- = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        abs_value /= radix;
    } while (abs_value > 0);

    if (is_negative) {
        *ptr-- = '-';
    }

    return ptr;
}

int private_create_directories(const char *path) {
    char temp_path[1024];
    strncpy(temp_path, path, sizeof(temp_path));
    temp_path[sizeof(temp_path) - 1] = '\0';

    char *current = temp_path;
    char *next = NULL;

    while ((next = strchr(current, '/')) != NULL) {
        *next = '\0';
        if (strlen(temp_path) > 0 && mkdir(temp_path, 0755) != 0 && errno != EEXIST) {
            perror("mkdir");
            return -1;
        }
        *next = '/';
        current = next + 1;
    }

    if (mkdir(temp_path, 0755) != 0 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }

    return 0;
}

// Should be free'd after use
char *chunk_provider_load_chunk_file_for_xz(const char *save_directory, int x, int z) {
    char chunk_filename[256];
    char subdir_x[3];
    char subdir_z[3];
    char full_path[1024];

    snprintf(chunk_filename, sizeof(chunk_filename), "c.%s.%s.dat",
             private_integer_to_string(x, 36), private_integer_to_string(z, 36));

    snprintf(subdir_x, sizeof(subdir_x), "%s", private_integer_to_string(x & 63, 36));
    snprintf(subdir_z, sizeof(subdir_z), "%s", private_integer_to_string(z & 63, 36));

    snprintf(full_path, sizeof(full_path), "%s/%s/%s", save_directory, subdir_x, subdir_z);

    if(private_create_directories(full_path) != 0) {
        return NULL;
    }

    strncat(full_path, "/", sizeof(full_path) - strlen(full_path) - 1);
    strncat(full_path, chunk_filename, sizeof(full_path) - strlen(full_path) - 1);

    char *result = malloc(strlen(full_path) + 1);
    strcpy(result, full_path);

    return result;
}

chunk_t *chunk_provider_load_load_chunk(chunk_provider_t *chunk_provider, int x, int z) {
    char *chunk_file = chunk_provider_load_chunk_file_for_xz(chunk_provider->save_directory, x, z);
    if (!chunk_file) {
        return NULL;
    }

    nbt_base_t nbt = { 0 }; // loading_screen_renderer_read(&chunk_file);
    free(chunk_file);

    chunk_t *chunk = malloc(sizeof(chunk_t));
    *chunk = (chunk_t){ 0 };
    *chunk = chunk_read_nbt_data(chunk, &nbt);

    return chunk;
}

void chunk_provider_load_save_chunk(chunk_provider_t *chunk_provider, chunk_t *chunk) {
    char *chunk_file = chunk_provider_load_chunk_file_for_xz(chunk_provider->save_directory, chunk->x_pos, chunk->z_pos);

    struct stat sb;
    if(stat(chunk_file, &sb) == 0) {
        chunk_provider->world->size_on_disk -= sb.st_size;
    }

    nbt_base_t nbt_base = { 0 };
    nbt_tag_compound_create(&nbt_base);
    nbt_base_t nbt = { 0 };
    nbt_tag_compound_create(&nbt);
    nbt_tag_compound_set_tag(&nbt_base, "Level", &nbt);
    chunk_write_nbt_data(chunk, &nbt);
    // loading_screen_renderer_write(&nbt_base, chunk_file);
    chunk_provider->world->size_on_disk += sb.st_size;
}

void chunk_provider_load_populate(chunk_provider_t *chunk_provider, chunk_provider_t *interface, int x, int z) {
    chunk_t *chunk = chunk_provider_provide_chunk(chunk_provider, x, z);
    if(!chunk->is_terrain_populated) {
        chunk->is_terrain_populated = 1;
        chunk_provider->chunk_provider_gen->populate(chunk_provider->chunk_provider_gen, interface, x, z);
    }
}

void chunk_provider_load_save_chunks(chunk_provider_t *chunk_provider, uint8_t save_entities) {
    int needs_saving_index = 0;
    for(int i = 0; i < CHUNK_PROVIDER_ARRAY_SIZE; i++) {
        chunk_t *chunk = chunk_provider->chunks[i];
        if(chunk != NULL && chunk_needs_saving(chunk, save_entities)) {
            chunk_provider_load_save_chunk(chunk_provider, chunk);
            chunk->is_modified = 0;
            needs_saving_index++;
            if(needs_saving_index == 2 && !save_entities) {
                return;
            }
        }
    }
}

uint8_t chunk_provider_load_unload_oldest_chunks(chunk_provider_t *chunk_provider) {
    chunk_provider->chunk_provider_gen->unload_oldest_chunks(chunk_provider->chunk_provider_gen);
    // This does nothing
    // The original code has a list but it is never used
    
    return 1;
}