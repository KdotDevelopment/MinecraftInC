#include <nbt/nbt_tag_short.h>

nbt_base_t nbt_tag_short_create(int16_t value) {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

    tag.type = NBT_TYPE_SHORT;
    tag.short_value = value;

    tag.read_contents = nbt_tag_short_read_contents;
    tag.write_contents = nbt_tag_short_write_contents;

    return tag;
}

void nbt_tag_short_read_contents(nbt_base_t *nbt, gzFile file) {
    nbt->short_value = gz_read_short(file);
}

void nbt_tag_short_write_contents(nbt_base_t *nbt, gzFile file) {
    gzwrite(file, &nbt->short_value, sizeof(nbt->short_value));
}