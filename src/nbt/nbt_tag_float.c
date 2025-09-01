#include <nbt/nbt_tag_float.h>

nbt_base_t nbt_tag_float_create(float value) {
    nbt_base_t tag = { 0 };

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
    gzwrite(file, &nbt->float_value, sizeof(nbt->float_value));
}