#pragma once

#include <world/block/block.h>

// Corresponds to minecraft block IDs
enum {
    BLOCK_AIR = 0,
    BLOCK_STONE,
    BLOCK_GRASS,
    BLOCK_DIRT,
    BLOCK_COBBLESTONE,
    BLOCK_PLANKS,
    BLOCK_SAPLING,
    BLOCK_BEDROCK,
    BLOCK_WATER,
    BLOCK_STILL_WATER,
    BLOCK_LAVA,
    BLOCK_STILL_LAVA,
    BLOCK_SAND,
    BLOCK_GRAVEL,
    BLOCK_GOLD_ORE,
    BLOCK_IRON_ORE,
    BLOCK_COAL_ORE,
    BLOCK_LOG,
    BLOCK_LEAVES,
    BLOCK_SPONGE,
    BLOCK_GLASS,
    BLOCK_RED_WOOL,
    BLOCK_ORANGE_WOOL,
    BLOCK_YELLOW_WOOL,
    BLOCK_LIME_WOOL,
    BLOCK_GREEN_WOOL,
    BLOCK_AQUA_WOOL,
    BLOCK_CYAN_WOOL,
    BLOCK_BLUE_WOOL,
    BLOCK_PURPLE_WOOL,
    BLOCK_INDIGO_WOOL,
    BLOCK_VIOLET_WOOL,
    BLOCK_MAGENTA_WOOL,
    BLOCK_PINK_WOOL,
    BLOCK_BLACK_WOOL,
    BLOCK_GRAY_WOOL,
    BLOCK_WHITE_WOOL,
    BLOCK_DANDELION,
    BLOCK_ROSE,
    BLOCK_BROWN_MUSHROOM,
    BLOCK_RED_MUSHROOM,
    BLOCK_GOLD,
    BLOCK_IRON,
    BLOCK_DOUBLE_SLAB,
    BLOCK_SLAB,
    BLOCK_BRICK,
    BLOCK_TNT,
    BLOCK_BOOKSHELF,
    BLOCK_MOSSY_COBBLESTONE,
    BLOCK_OBSIDIAN,
    BLOCK_TORCH,
    BLOCK_FIRE,
    BLOCK_WATER_SOURCE,
    BLOCK_LAVA_SOURCE,
    BLOCK_CHEST,
    BLOCK_GEARS,
    BLOCK_DIAMOND_ORE,
    BLOCK_DIAMOND,
    BLOCK_WORKBENCH,
    BLOCK_CROPS,
    BLOCK_FARMLAND,
    BLOCK_FURNACE,
    BLOCK_FURNACE_LIT,
    BLOCK_COUNT,
};

enum {
    TEXTURE_GRASS = 0,
    TEXTURE_STONE = 1,
    TEXTURE_GRASS_SIDE = 3,
    TEXTURE_DIRT = 2,
    TEXTURE_COBBLESTONE = 16,
    TEXTURE_WOOD = 4,
    TEXTURE_SAPLING = 15,
    TEXTURE_BEDROCK = 17,
    TEXTURE_WATER = 14,
    //TEXTURE_STILL_WATER = 14,
    TEXTURE_LAVA = 30,
    //TEXTURE_STILL_LAVA = 30,
    TEXTURE_SAND = 18,
    TEXTURE_GRAVEL = 19,
    TEXTURE_GOLD_ORE = 32,
    TEXTURE_IRON_ORE = 33,
    TEXTURE_COAL_ORE = 34,
    TEXTURE_LOG_SIDE = 20,
    TEXTURE_LOG_TOP = 21,
    TEXTURE_LEAVES = 52,
    TEXTURE_SPONGE = 48,
    TEXTURE_GLASS = 49,
    TEXTURE_RED_WOOL = 64,
    TEXTURE_ORANGE_WOOL = 65,
    TEXTURE_YELLOW_WOOL = 66,
    TEXTURE_LIME_WOOL = 67,
    TEXTURE_GREEN_WOOL = 68,
    TEXTURE_AQUA_WOOL = 69,
    TEXTURE_CYAN_WOOL = 70,
    TEXTURE_BLUE_WOOL = 71,
    TEXTURE_PURPLE_WOOL = 72,
    TEXTURE_INDIGO_WOOL = 73,
    TEXTURE_VIOLET_WOOL = 74,
    TEXTURE_MAGENTA_WOOL = 75,
    TEXTURE_PINK_WOOL = 76,
    TEXTURE_BLACK_WOOL = 77,
    TEXTURE_GRAY_WOOL = 78,
    TEXTURE_WHITE_WOOL = 79,
    TEXTURE_DANDELION = 13,
    TEXTURE_ROSE = 12,
    TEXTURE_BROWN_MUSHROOM = 29,
    TEXTURE_RED_MUSHROOM = 28,
    TEXTURE_GOLD = 41,
    TEXTURE_IRON = 42,
    TEXTURE_DOUBLE_SLAB = 6,
    TEXTURE_SLAB = 5,
    TEXTURE_BRICK = 7,
    TEXTURE_TNT = 8,
    TEXTURE_BOOKSHELF = 35,
    TEXTURE_MOSSY_COBBLESTONE = 36,
    TEXTURE_OBSIDIAN = 37,
    TEXTURE_TORCH = 80,
    TEXTURE_FIRE = 31,
    TEXTURE_CHEST = 26,
    TEXTURE_GEARS = 62,
    TEXTURE_DIAMOND_ORE = 50,
    TEXTURE_DIAMOND_BLOCK = 40,
    TEXTURE_WORKBENCH = 59,
    TEXTURE_CROPS = 88,
    TEXTURE_FARMLAND = 87,
    TEXTURE_FURNACE = 45,
    TEXTURE_FURNACE_LIT = 45,
};

extern struct blocks_s {
    block_t air;
    block_t stone;
    block_t grass;
    block_t dirt;
    block_t cobblestone;
    block_t wood;
    block_t sapling;
    block_t bedrock;
    block_t water;
    block_t still_water;
    block_t lava;
    block_t still_lava;
    block_t sand;
    block_t gravel;
    block_t gold_ore;
    block_t iron_ore;
    block_t coal_ore;
    block_t log;
    block_t leaves;
    block_t sponge;
    block_t glass;
    block_t red_wool;
    block_t orange_wool;
    block_t yellow_wool;
    block_t lime_wool;
    block_t green_wool;
    block_t aqua_wool;
    block_t cyan_wool;
    block_t blue_wool;
    block_t purple_wool;
    block_t indigo_wool;
    block_t violet_wool;
    block_t magenta_wool;
    block_t pink_wool;
    block_t black_wool;
    block_t gray_wool;
    block_t white_wool;
    block_t dandelion;
    block_t rose;
    block_t brown_mushroom;
    block_t red_mushroom;
    block_t gold;
    block_t iron;
    block_t double_slab;
    block_t slab;
    block_t brick;
    block_t tnt;
    block_t bookshelf;
    block_t mossy_cobblestone;
    block_t obsidian;
    block_t torch;
    block_t fire;
    block_t chest;
    block_t gears;
    block_t diamond_ore;
    block_t workbench;
    block_t crops;
    block_t farmland;
    block_t furnace;
    block_t furnace_lit;
} blocks;

void blocks_init();