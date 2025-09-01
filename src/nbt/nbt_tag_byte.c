#include <nbt/nbt_tag_byte.h>

nbt_base_t nbt_tag_byte_create(int8_t value) {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_BYTE;
    tag.byte_value = value;

    return tag;
}