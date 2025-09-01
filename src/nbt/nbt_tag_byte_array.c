#include <nbt/nbt_tag_byte_array.h>

#include <string.h>
#include <stdlib.h>

nbt_base_t nbt_tag_byte_array_create(uint8_t *value, uint64_t length) {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_BYTE_ARRAY;
    tag.byte_array = malloc(sizeof(uint8_t) * length);
    memcpy(tag.byte_array, value, sizeof(uint8_t) * length);
    tag.byte_array_length = length;

    return tag;
}
