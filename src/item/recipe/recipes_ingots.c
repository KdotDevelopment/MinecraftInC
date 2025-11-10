#include <item/recipe/recipes_ingots.h>

#include <item/items.h>
#include <world/block/blocks.h>

void recipes_ingots_add(crafting_manager_t *manager) {
    int16_t list[][2] = {
        { blocks.gold.id, items.gold_ingot.id },
        { blocks.iron.id, items.iron_ingot.id },
        { blocks.diamond.id, items.diamond.id }
    };

    for(int i = 0; i < sizeof(list) / (2 * sizeof(int16_t)); i++) {
        int16_t block_id = list[i][0];
        int16_t item_id = list[i][1];
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(block_id, 1, 0), "###", "###", "###", '#', item_id, 0, 0, 0, 0 });
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(item_id, 9, 0), "#", "", "", '#', block_id, 0, 0, 0, 0 });
    }
}