#include <nbt/nbt_tag_long.h>

nbt_base_t nbt_tag_long_create(int64_t value) {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_LONG;
    tag.long_value = value;

    return tag;
}