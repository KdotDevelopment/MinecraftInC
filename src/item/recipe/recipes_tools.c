#include <item/recipe/recipes_tools.h>

#include <item/items.h>
#include <world/block/blocks.h>

void recipes_tools_add(crafting_manager_t *manager) {
    int16_t list[][5] = {
        { blocks.wood.id, items.wooden_pickaxe.id, items.wooden_shovel.id, items.wooden_axe.id, items.wooden_hoe.id },
        { blocks.cobblestone.id, items.stone_pickaxe.id, items.stone_shovel.id, items.stone_axe.id, items.stone_hoe.id },
        { items.iron_ingot.id, items.iron_pickaxe.id, items.iron_shovel.id, items.iron_axe.id, items.iron_hoe.id },
        { items.diamond.id, items.diamond_pickaxe.id, items.diamond_shovel.id, items.diamond_axe.id, items.diamond_hoe.id },
        { items.gold_ingot.id, items.gold_pickaxe.id, items.gold_shovel.id, items.gold_axe.id, items.gold_hoe.id }
    };

    for(int i = 0; i < sizeof(list) / (5 * sizeof(int16_t)); i++) {
        int16_t ingredient = list[i][0];
        int16_t pickaxe = list[i][1];
        int16_t shovel = list[i][2];
        int16_t axe = list[i][3];
        int16_t hoe = list[i][4];
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(pickaxe, 1, 0), "XXX", " # ", " # ", 'X', ingredient, '#', items.stick.id, 0, 0 });
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(shovel, 1, 0), "X", "#", "#", 'X', ingredient, '#', items.stick.id, 0, 0 });
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(axe, 1, 0), "XX", "X#", " #", 'X', ingredient, '#', items.stick.id, 0, 0 });
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(hoe, 1, 0), "XX", " #", " #", 'X', ingredient, '#', items.stick.id, 0, 0 });
    }
}