#include <world/block/block_torch.h>

#include <world/block/blocks.h>
#include <world/material/materials.h>

block_t block_torch_create() {
    block_t block = block_create(BLOCK_TORCH, TEXTURE_TORCH, &block_sounds.wood, 0, 0, &materials.wood);

    block.is_opaque = 0;
    block.is_cube = 0;
    block.is_solid = 0;

    block_list[BLOCK_TORCH] = block;

    return block;
}