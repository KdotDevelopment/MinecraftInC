#include <item/item_armor.h>

item_t item_armor_create(uint8_t id, uint8_t texture_id, uint8_t armor_type, uint8_t piece) {
    item_t item = item_create(id, texture_id);
    
    return item;
}