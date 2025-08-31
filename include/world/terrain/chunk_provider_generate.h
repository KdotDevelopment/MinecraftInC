#pragma once

#include <world/chunk/chunk_provider.h>

#include <util/random.h>

struct world_s;

void chunk_provider_generate_create(chunk_provider_t *chunk_provider, struct world_s *world, int64_t seed);
chunk_t *chunk_provider_generate_provide_chunk(chunk_provider_t *chunk_provider, int chunk_x, int chunk_z);
void chunk_provider_generate_populate(chunk_provider_t *chunk_provider, chunk_provider_t *interface, int chunk_x, int chunk_z);