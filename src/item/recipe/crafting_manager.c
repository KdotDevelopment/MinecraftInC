#include <item/recipe/crafting_manager.h>

#include <item/items.h>
#include <item/recipe/recipes_armor.h>
#include <item/recipe/recipes_ingots.h>
#include <item/recipe/recipes_tools.h>
#include <item/recipe/recipes_weapons.h>
#include <world/block/blocks.h>

#include <util/array_list.h>

#include <string.h>

static int16_t recipe_lookup_item(const recipe_t *recipe, char key) {
    if(key == 0 || key == ' ') return -1;
    if(recipe->char1 && recipe->char1 == key) return recipe->item1;
    if(recipe->char2 && recipe->char2 == key) return recipe->item2;
    if(recipe->char3 && recipe->char3 == key) return recipe->item3;
    return -1;
}

crafting_manager_t crafting_manager_create() {
    crafting_manager_t manager = { 0 };

    manager.recipes = array_list_create(sizeof(recipe_t));

    recipes_tools_add(&manager);
    recipes_weapons_add(&manager);
    recipes_ingots_add(&manager);
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(items.soup.id, 1, 0), "Y", "X", "#", 'X', blocks.brown_mushroom.id, 'Y', blocks.red_mushroom.id, '#', items.bowl.id });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(items.soup.id, 1, 0), "Y", "X", "#", 'X', blocks.red_mushroom.id, 'Y', blocks.brown_mushroom.id, '#', items.bowl.id });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(blocks.chest.id, 1, 0), "###", "# #", "###", '#', blocks.wood.id, 0, 0, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(blocks.furnace.id, 1, 0), "###", "# #", "###", '#', blocks.cobblestone.id, 0, 0, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(blocks.workbench.id, 1, 0), "##", "##", "", '#', blocks.wood.id, 0, 0, 0, 0 });
    recipes_armor_add(&manager);
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(blocks.gray_wool.id, 1, 0), "###", "###", "###", '#', items.string.id, 0, 0, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(blocks.tnt.id, 1, 0), "X#X", "#X#", "X#X", 'X', items.gunpowder.id, '#', blocks.sand.id, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(blocks.slab.id, 3, 0), "###", "", "", '#', blocks.cobblestone.id, 0, 0, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(blocks.wood.id, 4, 0), "#", "", "", '#', blocks.log.id, 0, 0, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(items.stick.id, 4, 0), "#", "#", "", '#', blocks.wood.id, 0, 0, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(blocks.torch.id, 4, 0), "X", "#", "", 'X', items.coal.id, '#', items.stick.id, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(items.bowl.id, 4, 0), "# #", " # ", "", '#', blocks.wood.id, 0, 0, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(items.flint_and_steel.id, 1, 0), "A ", " B", "", 'A', items.iron_ingot.id, 'B', items.flint.id, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(items.bread.id, 1, 0), "###", "", "", '#', items.wheat.id, 0, 0, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(items.painting.id, 1, 0), "###", "#X#", "###", '#', items.stick.id, 'X', blocks.gray_wool.id, 0, 0 });
    crafting_manager_add_recipe(&manager, (recipe_t){ item_stack_create(items.golden_apple.id, 1, 0), "###", "#X#", "###", '#', blocks.gold.id, 'X', items.apple.id, 0, 0 });

    // this was in the original code
    printf("%d recipes\n", array_list_length(manager.recipes));

    return manager;
}

void crafting_manager_add_recipe(crafting_manager_t *manager, recipe_t recipe) {
    uint8_t width = 0;
    uint8_t height = 0;

    for(int i = 0; i < 3; i++) {
        char row[4] = { 0, 0, 0, 0 };
        if(i == 0) memcpy(row, recipe.row_1, 4);
        if(i == 1) memcpy(row, recipe.row_2, 4);
        if(i == 2) memcpy(row, recipe.row_3, 4);
        if(row[0] == 0) continue;
        int j = 0;
        while(row[j] != 0) {
            j++;
            if(width < j) width = j;
        }
        height++;
    }

    recipe.width = width;
    recipe.height = height;

    for(int i = 0; i < 9; i++) {
        recipe.ingredients[i] = -1;
    }

    char *rows[4] = { recipe.row_1, recipe.row_2, recipe.row_3 };

    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            char key = rows[row][col];
            if(key == 0) break;

            int16_t item_id = recipe_lookup_item(&recipe, key);
            recipe.ingredients[row * recipe.width + col] = item_id;
        }
    }

    manager->recipes = array_list_push(manager->recipes, &recipe);
}

// items must always be an array of 9
uint8_t crafting_manager_matches(recipe_t *recipe, int16_t *items, int x, int y, uint8_t mirror) {
    for(int grid_x = 0; grid_x < 3; grid_x++) {
        for(int grid_y = 0; grid_y < 3; grid_y++) {
            int xx = grid_x - x;
            int yy = grid_y - y;
            int expected = -1;

            if(xx >= 0 && yy >= 0 && xx < recipe->width && yy < recipe->height) {
                if(mirror) {
                    expected = recipe->ingredients[(recipe->width - xx - 1) + yy * recipe->width];
                }else {
                    expected = recipe->ingredients[yy * recipe->width + xx];
                }
            }

            if(items[grid_x + grid_y * 3] != expected) return 0;
        }
    }
    return 1;
}

// items must always be an array of 9
uint8_t crafting_manager_match_recipe(recipe_t *recipe, int16_t *items) {
    for(int x = 0; x <= 3 - recipe->width; x++) {
        for(int y = 0; y <= 3 - recipe->height; y++) {
            if(crafting_manager_matches(recipe, items, x, y, 0)) return 1;
            if(crafting_manager_matches(recipe, items, x, y, 1)) return 1;
        }
    }
    return 0;
}

// items must always be an array of 9
item_stack_t crafting_manager_craft(crafting_manager_t *manager, int16_t *items) {
    for(int i = 0; i < array_list_length(manager->recipes); i++) {
        recipe_t *recipe = (recipe_t *)array_list_get(manager->recipes, i);
        if(crafting_manager_match_recipe(recipe, items)) {
            return recipe->result;
        }
    }
    return item_stack_create(0, 0, 0);
}