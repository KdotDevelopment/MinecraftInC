#include <nbt/nbt_tag_long.h>

#include <string.h>

nbt_base_t nbt_tag_long_create(int64_t value) {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

    tag.type = NBT_TYPE_LONG;
    tag.long_value = value;

    tag.read_contents = nbt_tag_long_read_contents;
    tag.write_contents = nbt_tag_long_write_contents;

    return tag;
}

void nbt_tag_long_read_contents(nbt_base_t *nbt, gzFile file) {
    nbt->long_value = gz_read_long(file);
}

void nbt_tag_long_write_contents(nbt_base_t *nbt, gzFile file) {
    gzwrite(file, &nbt->long_value, sizeof(nbt->long_value));
}