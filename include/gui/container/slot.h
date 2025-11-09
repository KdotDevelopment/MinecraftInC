#pragma once

#include <gui/container/inventory.h>

struct screen_s;

typedef struct slot_s {
    inventory_t *inventory;
    struct screen_s *screen;
    int index;
    int x;
    int y;
    int background_texture;

    union {
        uint8_t armor_type;
        inventory_t *crafting_matrix;
    };

    void (*on_changed)(struct slot_s *slot);
    int (*get_texture)(struct slot_s *slot);
    uint8_t (*can_put_item)(struct slot_s *slot, item_stack_t item);
    void (*on_pickup)(struct slot_s *slot);
} slot_t;

slot_t slot_create(struct screen_s *screen, inventory_t *inventory, int index, int x, int y);
uint8_t slot_is_under_cursor(slot_t *slot, int mx, int my);
void slot_put_stack(slot_t *slot, item_stack_t item);
void slot_on_changed(slot_t *slot);
int slot_get_texture(slot_t *slot);
uint8_t slot_can_put_item(slot_t *slot, item_stack_t item);
void slot_on_pickup(slot_t *slot);