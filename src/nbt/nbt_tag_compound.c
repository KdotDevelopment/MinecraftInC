#include <nbt/nbt_tag_compound.h>

#include <nbt/nbt_base.h>
#include <nbt/nbt_tag_byte_array.h>
#include <nbt/nbt_tag_byte.h>
#include <nbt/nbt_tag_float.h>
#include <nbt/nbt_tag_int.h>
#include <nbt/nbt_tag_list.h>
#include <nbt/nbt_tag_long.h>
#include <nbt/nbt_tag_short.h>
#include <nbt/nbt_tag_string.h>
#include <util/array_list.h>

#include <string.h>
#include <stdlib.h>

nbt_base_t nbt_tag_compound_create() {
    nbt_base_t base = { 0 };
    memset(&base, 0, sizeof(nbt_base_t));

    base.type = NBT_TYPE_COMPOUND;
    base.tag_array = array_list_create(sizeof(nbt_base_t));

    base.read_contents = nbt_tag_compound_read_contents;
    base.write_contents = nbt_tag_compound_write_contents;

    return base;
}

void nbt_tag_compound_set_tag(nbt_base_t *base, char *key, nbt_base_t *tag) {
    base->tag_array = array_list_push(base->tag_array, nbt_set_key(tag, key));
}

void nbt_tag_compound_set_byte(nbt_base_t *base, char *key, int8_t value) {
    nbt_base_t tag = nbt_tag_byte_create(value);

    base->tag_array = array_list_push(base->tag_array, nbt_set_key(&tag, key));
}

void nbt_tag_compound_set_short(nbt_base_t *base, char *key, int16_t value) {
    nbt_base_t tag = nbt_tag_short_create(value);
    base->tag_array = array_list_push(base->tag_array, nbt_set_key(&tag, key));
}

void nbt_tag_compound_set_int(nbt_base_t *base, char *key, int32_t value) {
    nbt_base_t tag = nbt_tag_int_create(value);
    base->tag_array = array_list_push(base->tag_array, nbt_set_key(&tag, key));
}

void nbt_tag_compound_set_long(nbt_base_t *base, char *key, int64_t value) {
    nbt_base_t tag = nbt_tag_long_create(value);
    base->tag_array = array_list_push(base->tag_array, nbt_set_key(&tag, key));
}

void nbt_tag_compound_set_float(nbt_base_t *base, char *key, float value) {
    nbt_base_t tag = nbt_tag_float_create(value);
    base->tag_array = array_list_push(base->tag_array, nbt_set_key(&tag, key));
}

void nbt_tag_compound_set_string(nbt_base_t *base, char *key, char *value) {
    nbt_base_t tag = nbt_tag_string_create_from(value);
    base->tag_array = array_list_push(base->tag_array, nbt_set_key(&tag, key));
}

void nbt_tag_compound_set_byte_array(nbt_base_t *base, char *key, uint8_t *value, uint32_t length) {
    nbt_base_t tag = nbt_tag_byte_array_create_from(value, length);
    base->tag_array = array_list_push(base->tag_array, nbt_set_key(&tag, key));
}

void nbt_tag_compound_set_compound_tag(nbt_base_t *base, char *key, nbt_base_t *tag) {
    base->tag_array = array_list_push(base->tag_array, nbt_set_key(tag, key));
}

void nbt_tag_compound_set_boolean(nbt_base_t *base, char *key, int8_t value) {
    nbt_tag_compound_set_byte(base, key, !!value);
}

uint8_t private_has_key(nbt_base_t *base, char *key) {
    for(int i = 0; i < array_list_length(base->tag_array); i++) {
        nbt_base_t *tag = (nbt_base_t *)array_list_get(base->tag_array, i);
        if(strcmp(tag->key, key) == 0) return 1;
    }
    return 0;
}

nbt_base_t private_get_tag(nbt_base_t *base, char *key) {
    for(int i = 0; i < array_list_length(base->tag_array); i++) {
        nbt_base_t *tag = (nbt_base_t *)array_list_get(base->tag_array, i);
        if(strcmp(tag->key, key) == 0) return *tag;
    }
    nbt_base_t null_tag = { .null = 1 };
    return null_tag;
}

int8_t nbt_tag_compound_get_byte(nbt_base_t *base, char *key) {
    return !private_has_key(base, key) ? 0 : private_get_tag(base, key).byte_value;
}

int16_t nbt_tag_compound_get_short(nbt_base_t *base, char *key) {
    return !private_has_key(base, key) ? 0 : private_get_tag(base, key).short_value;
}

int32_t nbt_tag_compound_get_int(nbt_base_t *base, char *key) {
    return !private_has_key(base, key) ? 0 : private_get_tag(base, key).int_value;
}

int64_t nbt_tag_compound_get_long(nbt_base_t *base, char *key) {
    return !private_has_key(base, key) ? 0 : private_get_tag(base, key).long_value;
}

float nbt_tag_compound_get_float(nbt_base_t *base, char *key) {
    return !private_has_key(base, key) ? 0 : private_get_tag(base, key).float_value;
}

char *nbt_tag_compound_get_string(nbt_base_t *base, char *key) {
    return !private_has_key(base, key) ? NULL : private_get_tag(base, key).string_value;
}

int8_t *nbt_tag_compound_get_byte_array(nbt_base_t *base, char *key, uint32_t length) {
    if(!private_has_key(base, key)) return NULL;

    nbt_base_t tag = private_get_tag(base, key);

    int8_t *array = calloc(length, sizeof(int8_t));
    if(!array) {
        return NULL;
    }

    uint32_t copy_length = tag.byte_array_length;
    if(copy_length > length) {
        copy_length = length;
    }

    if(tag.byte_array != NULL && copy_length > 0) {
        memcpy(array, tag.byte_array, copy_length);
    }

    return array;
}

nbt_base_t nbt_tag_compound_get_compound_tag(nbt_base_t *base, char *key) {
    if(!private_has_key(base, key)) return (nbt_base_t){ .null = 1 };

    return private_get_tag(base, key);
}

nbt_base_t nbt_tag_compound_get_tag_list(nbt_base_t *base, char *key) {
    return !private_has_key(base, key) ? (nbt_base_t){ .null = 1 } : private_get_tag(base, key);
}

int8_t nbt_tag_compound_get_boolean(nbt_base_t *base, char *key) {
    return !private_has_key(base, key) ? 0 : private_get_tag(base, key).byte_value;
}

char *nbt_tag_compound_to_string(nbt_base_t *base) {
    return "";
}

void nbt_tag_compound_read_contents(nbt_base_t *nbt, gzFile file) {
    nbt->tag_array = array_list_clear(nbt->tag_array);

    for(;;) {
        nbt_base_t tag = nbt_read_named_tag(file);
        if(tag.null) return;
        if(tag.type == 0) return; // end tag
        if(!nbt->tag_array) nbt->tag_array = array_list_create(sizeof(nbt_base_t));
        nbt->tag_array = array_list_push(nbt->tag_array, &tag);
    }
}

void nbt_tag_compound_write_contents(nbt_base_t *nbt, gzFile file) {
    for(int i = 0; i < array_list_length(nbt->tag_array); i++) {
        nbt_base_t *tag = (nbt_base_t *)array_list_get(nbt->tag_array, i);
        nbt_write_named_tag(file, tag);
    }
    int8_t end_tag = 0;
    gzwrite(file, &end_tag, sizeof(end_tag));
}

void nbt_tag_compound_free(nbt_base_t *base) {
    if(!base || base->type != NBT_TYPE_COMPOUND) {
        return;
    }

    for(int i = 0; i < array_list_length(base->tag_array); i++) {
        nbt_base_t *tag = (nbt_base_t *)array_list_get(base->tag_array, i);

        if(tag->type == NBT_TYPE_COMPOUND) {
            nbt_tag_compound_free(tag);
        }else if (tag->type == NBT_TYPE_LIST) {
            if (tag->byte_array) {
                free(tag->byte_array);
            }
        }else if (tag->type == NBT_TYPE_STRING) {
            if (tag->string_value) {
                free(tag->string_value);
            }
        }else if (tag->type == NBT_TYPE_BYTE_ARRAY) {
            if (tag->byte_array) {
                free(tag->byte_array);
            }
        }
    }
    array_list_free(base->tag_array);

    base->tag_array = NULL;
}