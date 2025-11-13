#include <nbt/nbt_tag_byte.h>

#include <string.h>

nbt_base_t nbt_tag_byte_create(int8_t value) {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

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
    gz_write_byte(file, nbt->byte_value);
}