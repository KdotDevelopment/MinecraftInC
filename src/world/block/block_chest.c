#include <world/block/block_chest.h>

#include <world/block/block_container.h>
#include <world/block/blocks.h>
#include <world/material/materials.h>

block_t block_chest_create() {
    block_t block = block_container_create(BLOCK_CHEST, TEXTURE_CHEST, materials.wood);
    
    return block;
}