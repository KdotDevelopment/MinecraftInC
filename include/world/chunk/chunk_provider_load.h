#pragma once

#include <world/chunk/chunk.h>
#include <world/chunk/chunk_provider.h>

struct world_s;

void chunk_provider_load_create(chunk_provider_t *chunk_provider_load, chunk_provider_t *gen, struct world_s *world, char **save_directory);
uint8_t chunk_provider_load_chunk_exists(chunk_provider_t *chunk_provider, int x, int z);
chunk_t *chunk_provider_load_provide_chunk(chunk_provider_t *chunk_provider, int x, int z);
char *chunk_provider_load_chunk_file_for_xz(char **save_directory, int x, int z);
chunk_t *chunk_provider_load_load_chunk(chunk_provider_t *chunk_provider, int x, int z);
void chunk_provider_load_save_chunk(chunk_provider_t *chunk_provider, chunk_t *chunk);
void chunk_provider_load_populate(chunk_provider_t *chunk_provider, chunk_provider_t *gen, int x, int z);
void chunk_provider_load_save_chunks(chunk_provider_t *chunk_provider, uint8_t save_entities);
uint8_t chunk_provider_load_unload_oldest_chunks(chunk_provider_t *chunk_provider);