#include <nbt/nbt_tag_string.h>

#include <string.h>
#include <stdlib.h>

#ifndef strdup
char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}
#endif

nbt_base_t nbt_tag_string_create() {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

    tag.type = NBT_TYPE_STRING;
    tag.string_value = strdup("");

    tag.read_contents = nbt_tag_string_read_contents;
    tag.write_contents = nbt_tag_string_write_contents;

    return tag;
}

nbt_base_t nbt_tag_string_create_from(char *value) {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

    tag.type = NBT_TYPE_STRING;
    tag.string_value = strdup(value);

    tag.read_contents = nbt_tag_string_read_contents;
    tag.write_contents = nbt_tag_string_write_contents;

    return tag;
}

void nbt_tag_string_read_contents(nbt_base_t *nbt, gzFile file) {
    int16_t length = gz_read_short(file);
    nbt->string_value = malloc(length + 1);
    gzread(file, nbt->string_value, length);
    nbt->string_value[length] = '\0';
}

void nbt_tag_string_write_contents(nbt_base_t *nbt, gzFile file) {
    int16_t length = strlen(nbt->string_value);
    gzwrite(file, &length, sizeof(length));
    gzwrite(file, nbt->string_value, length);
}