#include <nbt/nbt_tag_byte_array.h>

#include <string.h>
#include <stdlib.h>

nbt_base_t nbt_tag_byte_array_create() {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

    tag.type = NBT_TYPE_BYTE_ARRAY;
    tag.byte_array = NULL;
    tag.byte_array_length = 0;

    tag.read_contents = nbt_tag_byte_array_read_contents;
    tag.write_contents = nbt_tag_byte_array_write_contents;

    return tag;
}

nbt_base_t nbt_tag_byte_array_create_from(uint8_t *value, uint32_t length) {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

    tag.type = NBT_TYPE_BYTE_ARRAY;
    tag.byte_array = malloc(sizeof(uint8_t) * length);
    memcpy(tag.byte_array, value, sizeof(uint8_t) * length);
    tag.byte_array_length = length;

    tag.read_contents = nbt_tag_byte_array_read_contents;
    tag.write_contents = nbt_tag_byte_array_write_contents;

    return tag;
}

void nbt_tag_byte_array_read_contents(nbt_base_t *nbt, gzFile file) {
    int length = gz_read_int(file);
    nbt->byte_array = malloc(sizeof(uint8_t) * length);
    nbt->byte_array_length = length;
    gz_read_fully(file, nbt->byte_array, sizeof(uint8_t) * nbt->byte_array_length);
}

void nbt_tag_byte_array_write_contents(nbt_base_t *nbt, gzFile file) {
    gz_write_int(file, nbt->byte_array_length);
    if(nbt->byte_array_length > 0) gz_write_fully(file, nbt->byte_array, sizeof(uint8_t) * nbt->byte_array_length);
}