#pragma once

#include <world/chunk/chunk.h>

struct world_s;

typedef struct chunk_provider_s {
    chunk_t chunks[1024];
    struct world_s *world;
    char *save_directory;
} chunk_provider_t;