#include <world/block/blocks.h>

#include <item/item.h>
#include <item/item_block.h>
#include <world/block/block.h>
#include <world/block/block_bookshelf.h>
#include <world/block/block_chest.h>
#include <world/block/block_crops.h>
#include <world/block/block_farmland.h>
#include <world/block/block_fire.h>
#include <world/block/block_flower.h>
#include <world/block/block_furnace.h>
#include <world/block/block_gears.h>
#include <world/block/block_glass.h>
#include <world/block/block_grass.h>
#include <world/block/block_leaves.h>
#include <world/block/block_liquid.h>
#include <world/block/block_log.h>
#include <world/block/block_metal.h>
#include <world/block/block_mushroom.h>
#include <world/block/block_ore.h>
#include <world/block/block_sand.h>
#include <world/block/block_sapling.h>
#include <world/block/block_slab.h>
#include <world/block/block_sponge.h>
#include <world/block/block_still_liquid.h>
#include <world/block/block_stone.h>
#include <world/block/block_tnt.h>
#include <world/block/block_torch.h>
#include <world/block/block_workbench.h>
#include <world/material/materials.h>

struct blocks_s blocks = { 0 };

void blocks_init() {
    blocks.air = block_create(BLOCK_AIR, -1, &block_sounds.none, 0, 0, 0);
    blocks.stone = block_stone_create(BLOCK_STONE, TEXTURE_STONE, 1.5, 10);
    blocks.grass = block_grass_create();
    blocks.dirt = block_create(BLOCK_DIRT, TEXTURE_DIRT, &block_sounds.gravel, 0.6, 0, &materials.ground);
    blocks.cobblestone = block_create(BLOCK_COBBLESTONE, TEXTURE_COBBLESTONE, &block_sounds.stone, 2, 10, &materials.rock);
    blocks.wood = block_create(BLOCK_PLANKS, TEXTURE_WOOD, &block_sounds.wood, 2, 5, &materials.wood);
    blocks.sapling = block_sapling_create();
    blocks.bedrock = block_create(BLOCK_BEDROCK, TEXTURE_BEDROCK, &block_sounds.stone, -1, 6000000, &materials.rock);
    blocks.water = block_liquid_create(BLOCK_WATER, LIQUID_WATER);
    blocks.still_water = block_still_liquid_create(BLOCK_STILL_WATER, LIQUID_WATER);
    blocks.lava = block_liquid_create(BLOCK_LAVA, LIQUID_LAVA);
    blocks.still_lava = block_still_liquid_create(BLOCK_STILL_LAVA, LIQUID_LAVA);
    blocks.sand = block_sand_create(BLOCK_SAND, TEXTURE_SAND, 0.5, 0);
    blocks.gravel = block_sand_create(BLOCK_GRAVEL, TEXTURE_GRAVEL, 0.6, 0);
    blocks.gold_ore = block_ore_create(BLOCK_GOLD_ORE, TEXTURE_GOLD_ORE, 3, 5);
    blocks.iron_ore = block_ore_create(BLOCK_IRON_ORE, TEXTURE_IRON_ORE, 3, 5);
    blocks.coal_ore = block_ore_create(BLOCK_COAL_ORE, TEXTURE_COAL_ORE, 3, 5);
    blocks.log = block_log_create();
    blocks.leaves = block_leaves_create();
    blocks.sponge = block_sponge_create();
    blocks.glass = block_glass_create();
    blocks.red_wool = block_create(BLOCK_RED_WOOL, TEXTURE_RED_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.orange_wool = block_create(BLOCK_ORANGE_WOOL, TEXTURE_ORANGE_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.yellow_wool = block_create(BLOCK_YELLOW_WOOL, TEXTURE_YELLOW_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.lime_wool = block_create(BLOCK_LIME_WOOL, TEXTURE_LIME_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.green_wool = block_create(BLOCK_GREEN_WOOL, TEXTURE_GREEN_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.aqua_wool = block_create(BLOCK_AQUA_WOOL, TEXTURE_AQUA_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.cyan_wool = block_create(BLOCK_CYAN_WOOL, TEXTURE_CYAN_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.blue_wool = block_create(BLOCK_BLUE_WOOL, TEXTURE_BLUE_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.purple_wool = block_create(BLOCK_PURPLE_WOOL, TEXTURE_PURPLE_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.indigo_wool = block_create(BLOCK_INDIGO_WOOL, TEXTURE_INDIGO_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.violet_wool = block_create(BLOCK_VIOLET_WOOL, TEXTURE_VIOLET_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.magenta_wool = block_create(BLOCK_MAGENTA_WOOL, TEXTURE_MAGENTA_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.pink_wool = block_create(BLOCK_PINK_WOOL, TEXTURE_PINK_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.black_wool = block_create(BLOCK_BLACK_WOOL, TEXTURE_BLACK_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.gray_wool = block_create(BLOCK_GRAY_WOOL, TEXTURE_GRAY_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.white_wool = block_create(BLOCK_WHITE_WOOL, TEXTURE_WHITE_WOOL, &block_sounds.wool, 0.8, 0, &materials.wool);
    blocks.dandelion = block_flower_create(BLOCK_DANDELION, TEXTURE_DANDELION);
    blocks.rose = block_flower_create(BLOCK_ROSE, TEXTURE_ROSE);
    blocks.brown_mushroom = block_mushroom_create(BLOCK_BROWN_MUSHROOM, TEXTURE_BROWN_MUSHROOM);
    blocks.red_mushroom = block_mushroom_create(BLOCK_RED_MUSHROOM, TEXTURE_RED_MUSHROOM);
    blocks.gold = block_metal_create(BLOCK_GOLD, TEXTURE_GOLD, 3, 10);
    blocks.iron = block_metal_create(BLOCK_IRON, TEXTURE_IRON, 5, 10);
    blocks.double_slab = block_slab_create(BLOCK_DOUBLE_SLAB, 1, 2, 10);
    blocks.slab = block_slab_create(BLOCK_SLAB, 0, 2, 10);
    blocks.brick = block_create(BLOCK_BRICK, TEXTURE_BRICK, &block_sounds.stone, 2, 10, &materials.rock);
    blocks.tnt = block_tnt_create();
    blocks.bookshelf = block_bookshelf_create();
    blocks.mossy_cobblestone = block_create(BLOCK_MOSSY_COBBLESTONE, TEXTURE_MOSSY_COBBLESTONE, &block_sounds.stone, 2, 10, &materials.rock);
    blocks.obsidian = block_stone_create(BLOCK_OBSIDIAN, TEXTURE_OBSIDIAN, 10, 10);
    blocks.torch = block_torch_create();
    blocks.fire = block_fire_create(); //hardness 0
    //source
    //source
    blocks.chest = block_chest_create(); //hardness 2.5
    blocks.gears = block_gears_create(); //hardness 0.5
    blocks.diamond_ore = block_ore_create(BLOCK_DIAMOND_ORE, TEXTURE_DIAMOND_ORE, 3, 5);
    blocks.workbench = block_workbench_create(); //hardness 2.5
    blocks.crops = block_crops_create(); //hardness 0
    blocks.farmland = block_farmland_create(); //hardness 0.6
    blocks.furnace = block_furnace_create(0); //hardness 3.5
    blocks.furnace_lit = block_furnace_create(1); //hardness 3.5

    for(int i = 0; i < 256; i++) {
        if(block_list[i].id == BLOCK_AIR) continue;
        item_list[i] = item_block_create(i - 256);
    }
}