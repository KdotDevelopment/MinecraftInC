#pragma once

#include <item/item_stack.h>

#include <stdint.h>

typedef struct recipe_s {
    item_stack_t result;
    char row_1[4];
    char row_2[4];
    char row_3[4];

    char char1;
    int16_t item1;

    char char2;
    int16_t item2;

    char char3;
    int16_t item3;

    uint8_t width;
    uint8_t height;
    int16_t ingredients[9];
} recipe_t;

typedef struct crafting_manager_s {
    recipe_t *recipes; // arraylist
} crafting_manager_t;

crafting_manager_t crafting_manager_create();
void crafting_manager_add_recipe(crafting_manager_t *manager, recipe_t recipe);
item_stack_t crafting_manager_craft(crafting_manager_t *manager, int16_t *items);