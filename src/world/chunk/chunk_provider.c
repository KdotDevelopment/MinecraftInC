#include <world/chunk/chunk_provider.h>

void chunk_provider_create(chunk_provider_t *chunk_provider) {
    *chunk_provider = (chunk_provider_t){ 0 };
    
    chunk_provider->chunk_exists = chunk_provider_chunk_exists;
    chunk_provider->chunk_provide = chunk_provider_provide_chunk;
    chunk_provider->populate = chunk_provider_populate;
    chunk_provider->save_chunks = chunk_provider_save_chunks;
    chunk_provider->unload_oldest_chunks = chunk_provider_unload_oldest_chunks;
}

uint8_t chunk_provider_chunk_exists(chunk_provider_t *chunk_provider, int x, int z) {
    return 1;
}

chunk_t *chunk_provider_provide_chunk(chunk_provider_t *chunk_provider, int x, int z) {
    return NULL;
}

void chunk_provider_populate(chunk_provider_t *chunk_provider, chunk_provider_t *interface, int x, int z) {
    return;
}

void chunk_provider_save_chunks(chunk_provider_t *chunk_provider, uint8_t save_entities) {
    return;
}

uint8_t chunk_provider_unload_oldest_chunks(chunk_provider_t *chunk_provider) {
    return 0;
}