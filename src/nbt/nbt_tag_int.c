#include <nbt/nbt_tag_int.h>

nbt_base_t nbt_tag_int_create(int32_t value) {
    nbt_base_t tag = { 0 };

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
    gzwrite(file, &nbt->int_value, sizeof(nbt->int_value));
}