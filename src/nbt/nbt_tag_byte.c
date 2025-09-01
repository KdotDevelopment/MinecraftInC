#include <nbt/nbt_tag_byte.h>

nbt_base_t nbt_tag_byte_create(int8_t value) {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_BYTE;
    tag.byte_value = value;

    tag.read_contents = nbt_tag_byte_read_contents;
    tag.write_contents = nbt_tag_byte_write_contents;

    return tag;
}

void nbt_tag_byte_read_contents(nbt_base_t *nbt, gzFile file) {
    nbt->byte_value = gz_read_byte(file);
}

void nbt_tag_byte_write_contents(nbt_base_t *nbt, gzFile file) {
    gzwrite(file, &nbt->byte_value, sizeof(nbt->byte_value));
}