#include <item/recipe/recipes_weapons.h>

#include <item/items.h>
#include <world/block/blocks.h>

void recipes_weapons_add(crafting_manager_t *manager) {
    int16_t list[][2] = {
        { blocks.wood.id, items.wooden_sword.id },
        { blocks.cobblestone.id, items.stone_sword.id },
        { items.iron_ingot.id, items.iron_sword.id },
        { items.diamond.id, items.diamond_sword.id },
        { items.gold_ingot.id, items.gold_sword.id }
    };

    for(int i = 0; i < sizeof(list) / (2 * sizeof(int16_t)); i++) {
        int16_t ingredient = list[i][0];
        int16_t sword = list[i][1];
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(sword, 1, 0), "X", "X", "#", 'X', ingredient, '#', items.stick.id, 0, 0 });
    }

    crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(items.bow.id, 1, 0), " #X", "# X", " #X", 'X', items.string.id, '#', items.stick.id, 0, 0 });
    crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(items.arrow.id, 1, 0), "X", "#", "Y", 'X', items.iron_ingot.id, '#', items.stick.id, 'Y', items.feather.id });
}