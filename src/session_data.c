#include <session_data.h>

#include <level/block/blocks.h>

#include <util/array_list.h>

uint8_t *session_allowed_blocks = { 0 };

void session_data_initialize() {
    session_allowed_blocks = array_list_create(sizeof(uint8_t));

    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.stone.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.cobblestone.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.brick.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.dirt.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.wood.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.log.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.leaves.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.glass.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.slab.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.mossy_cobblestone.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.sapling.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.dandelion.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.rose.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.brown_mushroom.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.red_mushroom.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.sand.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.gravel.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.sponge.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.red_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.orange_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.yellow_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.lime_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.green_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.aqua_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.cyan_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.blue_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.purple_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.indigo_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.violet_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.magenta_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.pink_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.black_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.gray_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.white_wool.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.coal_ore.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.iron_ore.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.gold_ore.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.iron.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.gold.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.bookshelf.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.tnt.id);
    session_allowed_blocks = array_list_push(session_allowed_blocks, &blocks.obsidian.id);
}