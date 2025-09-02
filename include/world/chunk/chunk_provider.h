#pragma once

#include <util/random.h>
#include <world/chunk/chunk.h>
#include <world/terrain/noise/noise.h>

#define CHUNK_PROVIDER_ARRAY_SIZE 1024

struct world_s;

typedef struct chunk_provider_s {
    struct world_s *world;

    union{
        // chunk_provider_load
        struct {
            chunk_t *chunks[CHUNK_PROVIDER_ARRAY_SIZE];
            char *save_directory;
            struct chunk_provider_s *chunk_provider_gen;
        };

        // chunk_provider_generate
        struct {
            random_t random;
            noise_t noise_1;
            noise_t noise_2;
            noise_t noise_3;
            noise_t noise_4;
            noise_t noise_5;
            noise_t tree_noise;
            double *noise_array;
            double *noise_array_1;
            double *noise_array_2;
            double *noise_array_3;
        };
    };

    uint8_t (*chunk_exists)(struct chunk_provider_s *chunk_provider, int x, int z);
    chunk_t *(*chunk_provide)(struct chunk_provider_s *chunk_provider, int x, int z);
    void (*populate)(struct chunk_provider_s *chunk_provider, struct chunk_provider_s *interface, int x, int z);
    void (*save_chunks)(struct chunk_provider_s *chunk_provider, uint8_t save_entities);
    uint8_t (*unload_oldest_chunks)(struct chunk_provider_s *chunk_provider);
} chunk_provider_t;

uint8_t chunk_provider_chunk_exists(chunk_provider_t *chunk_provider, int x, int z);
chunk_t *chunk_provider_provide_chunk(chunk_provider_t *chunk_provider, int x, int z);
void chunk_provider_populate(chunk_provider_t *chunk_provider, chunk_provider_t *interface, int x, int z);
void chunk_provider_save_chunks(chunk_provider_t *chunk_provider, uint8_t save_entities);
uint8_t chunk_provider_unload_oldest_chunks(chunk_provider_t *chunk_provider);