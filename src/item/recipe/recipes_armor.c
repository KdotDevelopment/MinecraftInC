#include <item/recipe/recipes_armor.h>

#include <item/items.h>
#include <world/block/blocks.h>

void recipes_armor_add(crafting_manager_t *manager) {
    int16_t list[][5] = {
        { blocks.gray_wool.id, items.leather_helmet.id, items.leather_chestplate.id, items.leather_leggings.id, items.leather_boots.id },
        { blocks.fire.id, items.chainmail_helmet.id, items.chainmail_chestplate.id, items.chainmail_leggings.id, items.chainmail_boots.id },
        { items.iron_ingot.id, items.iron_helmet.id, items.iron_chestplate.id, items.iron_leggings.id, items.iron_boots.id },
        { items.diamond.id, items.diamond_helmet.id, items.diamond_chestplate.id, items.diamond_leggings.id, items.diamond_boots.id },
        { items.gold_ingot.id, items.gold_helmet.id, items.gold_chestplate.id, items.gold_leggings.id, items.gold_boots.id }
    };

    for(int i = 0; i < sizeof(list) / (5 * sizeof(int16_t)); i++) {
        int16_t ingredient = list[i][0];
        int16_t helmet = list[i][1];
        int16_t chestplate = list[i][2];
        int16_t leggings = list[i][3];
        int16_t boots = list[i][4];
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(helmet, 1, 0), "###", "# #", "", '#', ingredient, 0, 0, 0, 0 });
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(chestplate, 1, 0), "# #", "###", "###", '#', ingredient, 0, 0, 0, 0 });
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(leggings, 1, 0), "###", "# #", "# #", '#', ingredient, 0, 0, 0, 0 });
        crafting_manager_add_recipe(manager, (recipe_t){ item_stack_create(boots, 1, 0), "# #", "# #", "", '#', ingredient, 0, 0, 0, 0 });
    }
}