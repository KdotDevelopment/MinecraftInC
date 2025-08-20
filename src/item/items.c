#include <item/items.h>

#include <item/item_armor.h>
#include <item/item_axe.h>
#include <item/item_bow.h>
#include <item/item_flint_and_steel.h>
#include <item/item_food.h>
#include <item/item_hoe.h>
#include <item/item_painting.h>
#include <item/item_pickaxe.h>
#include <item/item_seeds.h>
#include <item/item_shovel.h>
#include <item/item_soup.h>
#include <item/item_sword.h>
#include <world/block/blocks.h>

struct items_s items = { 0 };

void items_init() {
    items.iron_shovel = item_shovel_create(ITEM_IRON_SHOVEL, TEXTURE_ITEM_IRON_SHOVEL, TYPE_TOOL_IRON);
    items.iron_pickaxe = item_pickaxe_create(ITEM_IRON_PICKAXE, TEXTURE_ITEM_IRON_PICKAXE, TYPE_TOOL_IRON);
    items.iron_axe = item_axe_create(ITEM_IRON_AXE, TEXTURE_ITEM_IRON_AXE, TYPE_TOOL_IRON);
    items.flint_and_steel = item_flint_and_steel_create(ITEM_FLINT_AND_STEEL, TEXTURE_ITEM_FLINT_AND_STEEL);
    items.apple = item_food_create(ITEM_APPLE, TEXTURE_ITEM_APPLE, 4);
    items.bow = item_bow_create(ITEM_BOW, TEXTURE_ITEM_BOW);
    items.arrow = item_create(ITEM_ARROW, TEXTURE_ITEM_ARROW);
    items.coal = item_create(ITEM_COAL, TEXTURE_ITEM_COAL);
    items.diamond = item_create(ITEM_DIAMOND, TEXTURE_ITEM_DIAMOND);
    items.iron_ingot = item_create(ITEM_IRON_INGOT, TEXTURE_ITEM_IRON_INGOT);
    items.gold_ingot = item_create(ITEM_GOLD_INGOT, TEXTURE_ITEM_GOLD_INGOT);
    items.iron_sword = item_sword_create(ITEM_IRON_SWORD, TEXTURE_ITEM_IRON_SWORD, TYPE_TOOL_IRON);
    items.wooden_sword = item_sword_create(ITEM_WOODEN_SWORD, TEXTURE_ITEM_WOODEN_SWORD, TYPE_TOOL_WOOD);
    items.wooden_shovel = item_shovel_create(ITEM_WOODEN_SHOVEL, TEXTURE_ITEM_WOODEN_SHOVEL, TYPE_TOOL_WOOD);
    items.wooden_pickaxe = item_pickaxe_create(ITEM_WOODEN_PICKAXE, TEXTURE_ITEM_WOODEN_PICKAXE, TYPE_TOOL_WOOD);
    items.wooden_axe = item_axe_create(ITEM_WOODEN_AXE, TEXTURE_ITEM_WOODEN_AXE, TYPE_TOOL_WOOD);
    items.stone_sword = item_sword_create(ITEM_STONE_SWORD, TEXTURE_ITEM_STONE_SWORD, TYPE_TOOL_STONE);
    items.stone_shovel = item_shovel_create(ITEM_STONE_SHOVEL, TEXTURE_ITEM_STONE_SHOVEL, TYPE_TOOL_STONE);
    items.stone_pickaxe = item_pickaxe_create(ITEM_STONE_PICKAXE, TEXTURE_ITEM_STONE_PICKAXE, TYPE_TOOL_STONE);
    items.stone_axe = item_axe_create(ITEM_STONE_AXE, TEXTURE_ITEM_STONE_AXE, TYPE_TOOL_STONE);
    items.diamond_sword = item_sword_create(ITEM_DIAMOND_SWORD, TEXTURE_ITEM_DIAMOND_SWORD, TYPE_TOOL_DIAMOND);
    items.diamond_shovel = item_shovel_create(ITEM_DIAMOND_SHOVEL, TEXTURE_ITEM_DIAMOND_SHOVEL, TYPE_TOOL_DIAMOND);
    items.diamond_pickaxe = item_pickaxe_create(ITEM_DIAMOND_PICKAXE, TEXTURE_ITEM_DIAMOND_PICKAXE, TYPE_TOOL_DIAMOND);
    items.diamond_axe = item_axe_create(ITEM_DIAMOND_AXE, TEXTURE_ITEM_DIAMOND_AXE, TYPE_TOOL_DIAMOND);
    items.stick = item_create(ITEM_STICK, TEXTURE_ITEM_STICK);
    items.bowl = item_create(ITEM_BOWL, TEXTURE_ITEM_BOWL);
    items.soup = item_soup_create(ITEM_SOUP, TEXTURE_ITEM_SOUP, 10);
    items.gold_sword = item_sword_create(ITEM_GOLD_SWORD, TEXTURE_ITEM_GOLD_SWORD, TYPE_TOOL_GOLD);
    items.gold_shovel = item_shovel_create(ITEM_GOLD_SHOVEL, TEXTURE_ITEM_GOLD_SHOVEL, TYPE_TOOL_GOLD);
    items.gold_pickaxe = item_pickaxe_create(ITEM_GOLD_PICKAXE, TEXTURE_ITEM_GOLD_PICKAXE, TYPE_TOOL_GOLD);
    items.gold_axe = item_axe_create(ITEM_GOLD_AXE, TEXTURE_ITEM_GOLD_AXE, TYPE_TOOL_GOLD);
    items.string = item_create(ITEM_STRING, TEXTURE_ITEM_STRING);
    items.feather = item_create(ITEM_FEATHER, TEXTURE_ITEM_FEATHER);
    items.gunpowder = item_create(ITEM_GUNPOWDER, TEXTURE_ITEM_GUNPOWDER);
    items.wooden_hoe = item_hoe_create(ITEM_WOODEN_HOE, TEXTURE_ITEM_WOODEN_HOE, TYPE_TOOL_WOOD);
    items.stone_hoe = item_hoe_create(ITEM_STONE_HOE, TEXTURE_ITEM_STONE_HOE, TYPE_TOOL_STONE);
    items.iron_hoe = item_hoe_create(ITEM_IRON_HOE, TEXTURE_ITEM_IRON_HOE, TYPE_TOOL_IRON);
    items.diamond_hoe = item_hoe_create(ITEM_DIAMOND_HOE, TEXTURE_ITEM_DIAMOND_HOE, TYPE_TOOL_DIAMOND);
    items.gold_hoe = item_hoe_create(ITEM_GOLD_HOE, TEXTURE_ITEM_GOLD_HOE, TYPE_TOOL_GOLD);
    items.seeds = item_seeds_create(ITEM_SEEDS, TEXTURE_ITEM_SEEDS, BLOCK_CROPS);
    items.wheat = item_create(ITEM_WHEAT, TEXTURE_ITEM_WHEAT);
    items.bread = item_food_create(ITEM_BREAD, TEXTURE_ITEM_BREAD, 5);
    items.leather_helmet = item_armor_create(ITEM_LEATHER_HELMET, TEXTURE_ITEM_LEATHER_HELMET, TYPE_ARMOR_LEATHER, PIECE_ARMOR_HELMET);
    items.leather_chestplate = item_armor_create(ITEM_LEATHER_CHESTPLATE, TEXTURE_ITEM_LEATHER_CHESTPLATE, TYPE_ARMOR_LEATHER, PIECE_ARMOR_CHESTPLATE);
    items.leather_leggings = item_armor_create(ITEM_LEATHER_LEGGINGS, TEXTURE_ITEM_LEATHER_LEGGINGS, TYPE_ARMOR_LEATHER, PIECE_ARMOR_LEGGINGS);
    items.leather_boots = item_armor_create(ITEM_LEATHER_BOOTS, TEXTURE_ITEM_LEATHER_BOOTS, TYPE_ARMOR_LEATHER, PIECE_ARMOR_BOOTS);
    items.chainmail_helmet = item_armor_create(ITEM_CHAINMAIL_HELMET, TEXTURE_ITEM_CHAINMAIL_HELMET, TYPE_ARMOR_CHAIN, PIECE_ARMOR_HELMET);
    items.chainmail_chestplate = item_armor_create(ITEM_CHAINMAIL_CHESTPLATE, TEXTURE_ITEM_CHAINMAIL_CHESTPLATE, TYPE_ARMOR_CHAIN, PIECE_ARMOR_CHESTPLATE);
    items.chainmail_leggings = item_armor_create(ITEM_CHAINMAIL_LEGGINGS, TEXTURE_ITEM_CHAINMAIL_LEGGINGS, TYPE_ARMOR_CHAIN, PIECE_ARMOR_LEGGINGS);
    items.chainmail_boots = item_armor_create(ITEM_CHAINMAIL_BOOTS, TEXTURE_ITEM_CHAINMAIL_BOOTS, TYPE_ARMOR_CHAIN, PIECE_ARMOR_BOOTS);
    items.iron_helmet = item_armor_create(ITEM_IRON_HELMET, TEXTURE_ITEM_IRON_HELMET, TYPE_ARMOR_IRON, PIECE_ARMOR_HELMET);
    items.iron_chestplate = item_armor_create(ITEM_IRON_CHESTPLATE, TEXTURE_ITEM_IRON_CHESTPLATE, TYPE_ARMOR_IRON, PIECE_ARMOR_CHESTPLATE);
    items.iron_leggings = item_armor_create(ITEM_IRON_LEGGINGS, TEXTURE_ITEM_IRON_LEGGINGS, TYPE_ARMOR_IRON, PIECE_ARMOR_LEGGINGS);
    items.iron_boots = item_armor_create(ITEM_IRON_BOOTS, TEXTURE_ITEM_IRON_BOOTS, TYPE_ARMOR_IRON, PIECE_ARMOR_BOOTS);
    items.diamond_helmet = item_armor_create(ITEM_DIAMOND_HELMET, TEXTURE_ITEM_DIAMOND_HELMET, TYPE_ARMOR_DIAMOND, PIECE_ARMOR_HELMET);
    items.diamond_chestplate = item_armor_create(ITEM_DIAMOND_CHESTPLATE, TEXTURE_ITEM_DIAMOND_CHESTPLATE, TYPE_ARMOR_DIAMOND, PIECE_ARMOR_CHESTPLATE);
    items.diamond_leggings = item_armor_create(ITEM_DIAMOND_LEGGINGS, TEXTURE_ITEM_DIAMOND_LEGGINGS, TYPE_ARMOR_DIAMOND, PIECE_ARMOR_LEGGINGS);
    items.diamond_boots = item_armor_create(ITEM_DIAMOND_BOOTS, TEXTURE_ITEM_DIAMOND_BOOTS, TYPE_ARMOR_DIAMOND, PIECE_ARMOR_BOOTS);
    items.gold_helmet = item_armor_create(ITEM_GOLD_HELMET, TEXTURE_ITEM_GOLD_HELMET, TYPE_ARMOR_GOLD, PIECE_ARMOR_HELMET);
    items.gold_chestplate = item_armor_create(ITEM_GOLD_CHESTPLATE, TEXTURE_ITEM_GOLD_CHESTPLATE, TYPE_ARMOR_GOLD, PIECE_ARMOR_CHESTPLATE);
    items.gold_leggings = item_armor_create(ITEM_GOLD_LEGGINGS, TEXTURE_ITEM_GOLD_LEGGINGS, TYPE_ARMOR_GOLD, PIECE_ARMOR_LEGGINGS);
    items.gold_boots = item_armor_create(ITEM_GOLD_BOOTS, TEXTURE_ITEM_GOLD_BOOTS, TYPE_ARMOR_GOLD, PIECE_ARMOR_BOOTS);
    items.flint = item_create(ITEM_FLINT, TEXTURE_ITEM_FLINT);
    items.raw_pork = item_food_create(ITEM_RAW_PORK, TEXTURE_ITEM_RAW_PORK, 3);
    items.cooked_pork = item_food_create(ITEM_COOKED_PORK, TEXTURE_ITEM_COOKED_PORK, 8);
    items.painting = item_painting_create(ITEM_PAINTING, TEXTURE_ITEM_PAINTING);
    items.golden_apple = item_food_create(ITEM_GOLDEN_APPLE, TEXTURE_ITEM_GOLDEN_APPLE, 42);
}