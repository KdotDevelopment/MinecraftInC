#include <nbt/nbt_tag_float.h>

#include <string.h>

nbt_base_t nbt_tag_float_create(float value) {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

    tag.type = NBT_TYPE_FLOAT;
    tag.float_value = value;

    tag.read_contents = nbt_tag_float_read_contents;
    tag.write_contents = nbt_tag_float_write_contents;

    return tag;
}

void nbt_tag_float_read_contents(nbt_base_t *nbt, gzFile file) {
    nbt->float_value = gz_read_float(file);
}

void nbt_tag_float_write_contents(nbt_base_t *nbt, gzFile file) {
    gz_write_float(file, nbt->float_value);
}