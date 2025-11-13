#include <nbt/nbt_tag_int.h>

#include <string.h>

nbt_base_t nbt_tag_int_create(int32_t value) {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

    tag.type = NBT_TYPE_INT;
    tag.int_value = value;

    tag.read_contents = nbt_tag_int_read_contents;
    tag.write_contents = nbt_tag_int_write_contents;

    return tag;
}

void nbt_tag_int_read_contents(nbt_base_t *nbt, gzFile file) {
    nbt->int_value = gz_read_int(file);
}

void nbt_tag_int_write_contents(nbt_base_t *nbt, gzFile file) {
    gz_write_int(file, nbt->int_value);
}