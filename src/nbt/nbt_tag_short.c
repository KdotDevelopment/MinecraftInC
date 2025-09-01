#include <nbt/nbt_tag_short.h>

nbt_base_t nbt_tag_short_create(int16_t value) {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_SHORT;
    tag.short_value = value;
    
    return tag;
}