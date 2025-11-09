#include <gui/container/slot_crafting.h>

slot_t slot_crafting_create(screen_t *screen, inventory_t *crafting_matrix, inventory_t *inventory, int index, int x, int y) {
    slot_t slot = slot_create(screen, inventory, index, x, y);

    slot.crafting_matrix = crafting_matrix;

    slot.can_put_item = slot_crafting_can_put_item;
    slot.on_pickup = slot_crafting_on_pickup;

    return slot;
}

uint8_t slot_crafting_can_put_item(slot_t *slot, item_stack_t item) {
    return 0;
}

void slot_crafting_on_pickup(slot_t *slot) {
    for(int i = 0; i < slot->crafting_matrix->size; i++) {
        if(slot->crafting_matrix->get_slot(slot->crafting_matrix, i).item_id != 0) {
            slot->crafting_matrix->remove_item(slot->crafting_matrix, i, 1);
        }
    }
}