#include <nbt/nbt_tag_float.h>

nbt_base_t nbt_tag_float_create(float value) {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_FLOAT;
    tag.float_value = value;

    return tag;
}