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

nbt_base_t nbt_tag_string_create(char *value) {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_STRING;
    tag.string_value = strdup(value);

    return tag;
}