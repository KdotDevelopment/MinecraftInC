#include <nbt/nbt_tag_double.h>

nbt_base_t nbt_tag_double_create(double value) {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_DOUBLE;
    tag.double_value = value;

    tag.read_contents = nbt_tag_double_read_contents;
    tag.write_contents = nbt_tag_double_write_contents;

    return tag;
}

void nbt_tag_double_read_contents(nbt_base_t *nbt, gzFile file) {
    nbt->double_value = gz_read_double(file);
}

void nbt_tag_double_write_contents(nbt_base_t *nbt, gzFile file) {
    gzwrite(file, &nbt->double_value, sizeof(nbt->double_value));
}