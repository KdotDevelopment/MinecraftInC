#include <nbt/nbt_tag_int.h>

nbt_base_t nbt_tag_int_create(int32_t value) {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_INT;
    tag.int_value = value;
    
    return tag;
}