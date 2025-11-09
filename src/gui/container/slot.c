#include <gui/container/slot.h>

#include <gui/screen.h>

#include <string.h>

slot_t slot_create(screen_t *screen, inventory_t *inventory, int index, int x, int y) {
    slot_t slot = { 0 };
    memset(&slot, 0, sizeof(slot_t));

    slot.screen = screen;
    slot.inventory = inventory;
    slot.index = index;
    slot.x = x;
    slot.y = y;
    slot.background_texture = -1;

    slot.on_changed = slot_on_changed;
    slot.get_texture = slot_get_texture;
    slot.can_put_item = slot_can_put_item;
    slot.on_pickup = slot_on_pickup;

    return slot;
}

uint8_t slot_is_under_cursor(slot_t *slot, int mx, int my) {
    int x = (slot->screen->width - slot->screen->container_x) / 2;
    int y = (slot->screen->height - slot->screen->container_y) / 2;
    mx -= x;
    my -= y;
    return mx >= slot->x - 1 && mx < slot->x + 16 + 1 && my >= slot->y - 1 && my < slot->y + 16 + 1;
}

void slot_put_stack(slot_t *slot, item_stack_t item) {
    slot->inventory->set_slot(slot->inventory, slot->index, item);
    slot->on_changed(slot);
}

void slot_on_changed(slot_t *slot) {
    slot->inventory->on_changed(slot->inventory);
}

int slot_get_texture(slot_t *slot) {
    return -1;
}

uint8_t slot_can_put_item(slot_t *slot, item_stack_t item) {
    return 1;
}

void slot_on_pickup(slot_t *slot) {
    slot->on_changed(slot);
}