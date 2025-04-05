#include <level/block/blocks.h>
#include <level/block/block.h>
#include <level/block/block_grass.h>
#include <level/block/block_sapling.h>
#include <level/block/block_liquid.h>
#include <level/block/block_still_liquid.h>
#include <level/block/block_sand.h>
#include <level/block/block_log.h>
#include <level/block/block_leaves.h>
#include <level/block/block_sponge.h>
#include <level/block/block_glass.h>
#include <level/block/block_flower.h>
#include <level/block/block_mushroom.h>
#include <level/block/block_metal.h>
#include <level/block/block_slab.h>
#include <level/block/block_tnt.h>
#include <level/block/block_bookshelf.h>

struct blocks_s blocks = { 0 };

void blocks_init() {
    blocks.air = block_create(BLOCK_AIR, -1, block_sounds.none, 0, 0, 0);
    blocks.stone = block_create(BLOCK_STONE, TEXTURE_STONE, block_sounds.stone, 1, 1, 0);
    blocks.grass = block_grass_create();
    blocks.dirt = block_create(BLOCK_DIRT, TEXTURE_DIRT, block_sounds.grass, 1, 0.5, 1);
    blocks.cobblestone = block_create(BLOCK_COBBLESTONE, TEXTURE_COBBLESTONE, block_sounds.stone, 1, 1.5, 0);
    blocks.wood = block_create(BLOCK_WOOD, TEXTURE_WOOD, block_sounds.wood, 1, 1.5, 1);
    blocks.sapling = block_sapling_create();
    blocks.bedrock = block_create(BLOCK_BEDROCK, TEXTURE_BEDROCK, block_sounds.stone, 1, 999, 0);
    blocks.water = block_liquid_create(BLOCK_WATER, LIQUID_WATER);
    blocks.still_water = block_still_liquid_create(BLOCK_STILL_WATER, LIQUID_WATER);
    blocks.lava = block_liquid_create(BLOCK_LAVA, LIQUID_LAVA);
    blocks.still_lava = block_still_liquid_create(BLOCK_STILL_LAVA, LIQUID_LAVA);
    blocks.sand = block_sand_create(BLOCK_SAND, TEXTURE_SAND);
    blocks.gravel = block_sand_create(BLOCK_GRAVEL, TEXTURE_GRAVEL);
    blocks.gold_ore = block_create(BLOCK_GOLD_ORE, TEXTURE_GOLD_ORE, block_sounds.stone, 1, 3, 0);
    blocks.iron_ore = block_create(BLOCK_IRON_ORE, TEXTURE_IRON_ORE, block_sounds.stone, 1, 3, 0);
    blocks.coal_ore = block_create(BLOCK_COAL_ORE, TEXTURE_COAL_ORE, block_sounds.stone, 1, 3, 0);
    blocks.log = block_log_create();
    blocks.leaves = block_leaves_create();
    blocks.sponge = block_sponge_create();
    blocks.glass = block_glass_create();
    blocks.red_wool = block_create(BLOCK_RED_WOOL, TEXTURE_RED_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.orange_wool = block_create(BLOCK_ORANGE_WOOL, TEXTURE_ORANGE_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.yellow_wool = block_create(BLOCK_YELLOW_WOOL, TEXTURE_YELLOW_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.lime_wool = block_create(BLOCK_LIME_WOOL, TEXTURE_LIME_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.green_wool = block_create(BLOCK_GREEN_WOOL, TEXTURE_GREEN_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.aqua_wool = block_create(BLOCK_AQUA_WOOL, TEXTURE_AQUA_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.cyan_wool = block_create(BLOCK_CYAN_WOOL, TEXTURE_CYAN_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.blue_wool = block_create(BLOCK_BLUE_WOOL, TEXTURE_BLUE_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.purple_wool = block_create(BLOCK_PURPLE_WOOL, TEXTURE_PURPLE_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.indigo_wool = block_create(BLOCK_INDIGO_WOOL, TEXTURE_INDIGO_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.violet_wool = block_create(BLOCK_VIOLET_WOOL, TEXTURE_VIOLET_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.magenta_wool = block_create(BLOCK_MAGENTA_WOOL, TEXTURE_MAGENTA_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.pink_wool = block_create(BLOCK_PINK_WOOL, TEXTURE_PINK_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.black_wool = block_create(BLOCK_BLACK_WOOL, TEXTURE_BLACK_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.gray_wool = block_create(BLOCK_GRAY_WOOL, TEXTURE_GRAY_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.white_wool = block_create(BLOCK_WHITE_WOOL, TEXTURE_WHITE_WOOL, block_sounds.wool, 1, 0.8, 1);
    blocks.dandelion = block_flower_create(BLOCK_DANDELION, TEXTURE_DANDELION);
    blocks.rose = block_flower_create(BLOCK_ROSE, TEXTURE_ROSE);
    blocks.brown_mushroom = block_mushroom_create(BLOCK_BROWN_MUSHROOM, TEXTURE_BROWN_MUSHROOM);
    blocks.red_mushroom = block_mushroom_create(BLOCK_RED_MUSHROOM, TEXTURE_RED_MUSHROOM);
    blocks.gold = block_metal_create(BLOCK_GOLD, TEXTURE_GOLD);
    blocks.iron = block_metal_create(BLOCK_IRON, TEXTURE_IRON);
    blocks.double_slab = block_slab_create(BLOCK_DOUBLE_SLAB, 1);
    blocks.slab = block_slab_create(BLOCK_SLAB, 0);
    blocks.brick = block_create(BLOCK_BRICK, TEXTURE_BRICK, block_sounds.stone, 1, 2, 0);
    blocks.tnt = block_tnt_create();
    blocks.bookshelf = block_bookshelf_create();
    blocks.mossy_cobblestone = block_create(BLOCK_MOSSY_COBBLESTONE, TEXTURE_MOSSY_COBBLESTONE, block_sounds.stone, 1, 1, 0);
    blocks.obsidian = block_create(BLOCK_OBSIDIAN, TEXTURE_OBSIDIAN, block_sounds.stone, 1, 10, 0);
}