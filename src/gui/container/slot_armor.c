#include <gui/container/slot_armor.h>

#include <item/item.h>

slot_t slot_armor_create(screen_t *screen, inventory_t *inventory, int index, int x, int y, int armor_type) {
    slot_t slot = slot_create(screen, inventory, index, x, y);
    
    slot.armor_type = armor_type;

    slot.can_put_item = slot_armor_can_put_item;
    slot.get_texture = slot_armor_get_texture;

    return slot;
}

uint8_t slot_armor_can_put_item(slot_t *slot, item_stack_t item) {
    return item_list[item.item_id].is_armor ? item_list[item.item_id].armor_piece == slot->armor_type : 0;
}

int slot_armor_get_texture(slot_t *slot) {
    return 15 + (slot->armor_type << 4);
}