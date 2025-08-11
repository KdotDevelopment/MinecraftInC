#pragma once

#include <item/item.h>

enum {
    ITEM_IRON_SHOVEL,
    ITEM_IRON_PICKAXE,
    ITEM_IRON_AXE,
    ITEM_FLINT_AND_STEEL,
    ITEM_APPLE,
    ITEM_BOW,
    ITEM_ARROW,
    ITEM_COAL,
    ITEM_DIAMOND,
    ITEM_IRON_INGOT,
    ITEM_GOLD_INGOT,
    ITEM_IRON_SWORD,
    ITEM_WOODEN_SWORD,
    ITEM_WOODEN_SHOVEL,
    ITEM_WOODEN_PICKAXE,
    ITEM_WOODEN_AXE,
    ITEM_STONE_SWORD,
    ITEM_STONE_SHOVEL,
    ITEM_STONE_PICKAXE,
    ITEM_STONE_AXE,
    ITEM_DIAMOND_SWORD,
    ITEM_DIAMOND_SHOVEL,
    ITEM_DIAMOND_PICKAXE,
    ITEM_DIAMOND_AXE,
    ITEM_STICK,
    ITEM_BOWL,
    ITEM_SOUP,
    ITEM_GOLD_SWORD,
    ITEM_GOLD_SHOVEL,
    ITEM_GOLD_PICKAXE,
    ITEM_GOLD_AXE,
    ITEM_STRING,
    ITEM_FEATHER,
    ITEM_GUNPOWDER,
    ITEM_WOODEN_HOE,
    ITEM_STONE_HOE,
    ITEM_IRON_HOE,
    ITEM_DIAMOND_HOE,
    ITEM_GOLD_HOE,
    ITEM_SEEDS,
    ITEM_WHEAT,
    ITEM_BREAD,
    ITEM_LEATHER_HELMET,
    ITEM_LEATHER_CHESTPLATE,
    ITEM_LEATHER_LEGGINGS,
    ITEM_LEATHER_BOOTS,
    ITEM_CHAINMAIL_HELMET,
    ITEM_CHAINMAIL_CHESTPLATE,
    ITEM_CHAINMAIL_LEGGINGS,
    ITEM_CHAINMAIL_BOOTS,
    ITEM_IRON_HELMET,
    ITEM_IRON_CHESTPLATE,
    ITEM_IRON_LEGGINGS,
    ITEM_IRON_BOOTS,
    ITEM_DIAMOND_HELMET,
    ITEM_DIAMOND_CHESTPLATE,
    ITEM_DIAMOND_LEGGINGS,
    ITEM_DIAMOND_BOOTS,
    ITEM_GOLD_HELMET,
    ITEM_GOLD_CHESTPLATE,
    ITEM_GOLD_LEGGINGS,
    ITEM_GOLD_BOOTS,
    ITEM_FLINT,
    ITEM_RAW_PORK,
    ITEM_COOKED_PORK,
    ITEM_PAINTING,
    ITEM_GOLDEN_APPLE,
    ITEM_COUNT
};

enum {
    TEXTURE_ITEM_IRON_SHOVEL = 82,
    TEXTURE_ITEM_IRON_PICKAXE = 98,
    TEXTURE_ITEM_IRON_AXE = 114,
    TEXTURE_ITEM_FLINT_AND_STEEL = 5,
    TEXTURE_ITEM_APPLE = 10,
    TEXTURE_ITEM_BOW = 21,
    TEXTURE_ITEM_ARROW = 37,
    TEXTURE_ITEM_COAL = 7,
    TEXTURE_ITEM_DIAMOND = 55,
    TEXTURE_ITEM_IRON_INGOT = 23,
    TEXTURE_ITEM_GOLD_INGOT = 39,
    TEXTURE_ITEM_IRON_SWORD = 66,
    TEXTURE_ITEM_WOODEN_SWORD = 64,
    TEXTURE_ITEM_WOODEN_SHOVEL = 80,
    TEXTURE_ITEM_WOODEN_PICKAXE = 96,
    TEXTURE_ITEM_WOODEN_AXE = 112,
    TEXTURE_ITEM_STONE_SWORD = 65,
    TEXTURE_ITEM_STONE_SHOVEL = 81,
    TEXTURE_ITEM_STONE_PICKAXE = 97,
    TEXTURE_ITEM_STONE_AXE = 113,
    TEXTURE_ITEM_DIAMOND_SWORD = 67,
    TEXTURE_ITEM_DIAMOND_SHOVEL = 83,
    TEXTURE_ITEM_DIAMOND_PICKAXE = 99,
    TEXTURE_ITEM_DIAMOND_AXE = 115,
    TEXTURE_ITEM_STICK = 53,
    TEXTURE_ITEM_BOWL = 71,
    TEXTURE_ITEM_SOUP = 72,
    TEXTURE_ITEM_GOLD_SWORD = 68,
    TEXTURE_ITEM_GOLD_SHOVEL = 84,
    TEXTURE_ITEM_GOLD_PICKAXE = 100,
    TEXTURE_ITEM_GOLD_AXE = 116,
    TEXTURE_ITEM_STRING = 8,
    TEXTURE_ITEM_FEATHER = 24,
    TEXTURE_ITEM_GUNPOWDER = 40,
    TEXTURE_ITEM_WOODEN_HOE = 128,
    TEXTURE_ITEM_STONE_HOE = 129,
    TEXTURE_ITEM_IRON_HOE = 130,
    TEXTURE_ITEM_DIAMOND_HOE = 131,
    TEXTURE_ITEM_GOLD_HOE = 132,
    TEXTURE_ITEM_SEEDS = 9,
    TEXTURE_ITEM_WHEAT = 25,
    TEXTURE_ITEM_BREAD = 41,
    TEXTURE_ITEM_LEATHER_HELMET = 0,
    TEXTURE_ITEM_LEATHER_CHESTPLATE = 16,
    TEXTURE_ITEM_LEATHER_LEGGINGS = 32,
    TEXTURE_ITEM_LEATHER_BOOTS = 48,
    TEXTURE_ITEM_CHAINMAIL_HELMET = 1,
    TEXTURE_ITEM_CHAINMAIL_CHESTPLATE = 17,
    TEXTURE_ITEM_CHAINMAIL_LEGGINGS = 33,
    TEXTURE_ITEM_CHAINMAIL_BOOTS = 49,
    TEXTURE_ITEM_IRON_HELMET = 2,
    TEXTURE_ITEM_IRON_CHESTPLATE = 18,
    TEXTURE_ITEM_IRON_LEGGINGS = 34,
    TEXTURE_ITEM_IRON_BOOTS = 50,
    TEXTURE_ITEM_DIAMOND_HELMET = 3,
    TEXTURE_ITEM_DIAMOND_CHESTPLATE = 19,
    TEXTURE_ITEM_DIAMOND_LEGGINGS = 35,
    TEXTURE_ITEM_DIAMOND_BOOTS = 51,
    TEXTURE_ITEM_GOLD_HELMET = 4,
    TEXTURE_ITEM_GOLD_CHESTPLATE = 20,
    TEXTURE_ITEM_GOLD_LEGGINGS = 36,
    TEXTURE_ITEM_GOLD_BOOTS = 52,
    TEXTURE_ITEM_FLINT = 6,
    TEXTURE_ITEM_RAW_PORK = 87,
    TEXTURE_ITEM_COOKED_PORK = 88,
    TEXTURE_ITEM_PAINTING = 26,
    TEXTURE_ITEM_GOLDEN_APPLE = 11,
};

enum {
    TYPE_TOOL_WOOD,
    TYPE_TOOL_STONE,
    TYPE_TOOL_IRON,
    TYPE_TOOL_DIAMOND,
    TYPE_TOOL_GOLD
};

enum {
    TYPE_ARMOR_LEATHER,
    TYPE_ARMOR_CHAIN,
    TYPE_ARMOR_IRON,
    TYPE_ARMOR_DIAMOND,
    TYPE_ARMOR_GOLD = 0 // not sure why ?
};

enum {
    PIECE_ARMOR_HELMET,
    PIECE_ARMOR_CHESTPLATE,
    PIECE_ARMOR_LEGGINGS,
    PIECE_ARMOR_BOOTS
};

enum {
    PIECE_TOOL_SHOVEL = 1,
    PIECE_TOOL_PICKAXE,
    PIECE_TOOL_AXE
};

extern struct items_s {
    item_t iron_shovel;
    item_t iron_pickaxe;
    item_t iron_axe;
    item_t flint_and_steel;
    item_t apple;
    item_t bow;
    item_t arrow;
    item_t coal;
    item_t diamond;
    item_t iron_ingot;
    item_t gold_ingot;
    item_t iron_sword;
    item_t wooden_sword;
    item_t wooden_shovel;
    item_t wooden_pickaxe;
    item_t wooden_axe;
    item_t stone_sword;
    item_t stone_shovel;
    item_t stone_pickaxe;
    item_t stone_axe;
    item_t diamond_sword;
    item_t diamond_shovel;
    item_t diamond_pickaxe;
    item_t diamond_axe;
    item_t stick;
    item_t bowl;
    item_t soup;
    item_t gold_sword;
    item_t gold_shovel;
    item_t gold_pickaxe;
    item_t gold_axe;
    item_t string;
    item_t feather;
    item_t gunpowder;
    item_t wooden_hoe;
    item_t stone_hoe;
    item_t iron_hoe;
    item_t diamond_hoe;
    item_t gold_hoe;
    item_t seeds;
    item_t wheat;
    item_t bread;
    item_t leather_helmet;
    item_t leather_chestplate;
    item_t leather_leggings;
    item_t leather_boots;
    item_t chainmail_helmet;
    item_t chainmail_chestplate;
    item_t chainmail_leggings;
    item_t chainmail_boots;
    item_t iron_helmet;
    item_t iron_chestplate;
    item_t iron_leggings;
    item_t iron_boots;
    item_t diamond_helmet;
    item_t diamond_chestplate;
    item_t diamond_leggings;
    item_t diamond_boots;
    item_t gold_helmet;
    item_t gold_chestplate;
    item_t gold_leggings;
    item_t gold_boots;
    item_t flint;
    item_t raw_pork;
    item_t cooked_pork;
    item_t painting;
    item_t golden_apple;
} items;

void items_init();