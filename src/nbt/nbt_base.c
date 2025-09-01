#include <nbt/nbt_base.h>

#include <nbt/nbt_tag_byte_array.h>
#include <nbt/nbt_tag_byte.h>
#include <nbt/nbt_tag_compound.h>
#include <nbt/nbt_tag_double.h>
#include <nbt/nbt_tag_float.h>
#include <nbt/nbt_tag_int.h>
#include <nbt/nbt_tag_list.h>
#include <nbt/nbt_tag_long.h>
#include <nbt/nbt_tag_short.h>
#include <nbt/nbt_tag_string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

nbt_base_t nbt_base_create() {
    nbt_base_t base = { 0 };
    memset(&base, 0, sizeof(nbt_base_t));

    base.read_contents = nbt_base_read_contents;
    base.write_contents = nbt_base_write_contents;

    return base;
}

nbt_base_t *nbt_set_key(nbt_base_t *base, char *key) {
    strncpy(base->key, key, MAX_NBT_KEY_LENGTH - 2);
    base->key[MAX_NBT_KEY_LENGTH - 1] = '\0';
    return base;
}

void nbt_base_read_contents(nbt_base_t *nbt, gzFile file) {
    return;
}

void nbt_base_write_contents(nbt_base_t *nbt, gzFile file) {
    return;
}

nbt_base_t nbt_create_tag_of_type(int8_t tag_type) {
    nbt_base_t nbt = { .null = 1 };
    switch(tag_type) {
        case NBT_TYPE_END:
            nbt = (nbt_base_t){ .type = NBT_TYPE_END };
            break;
        case NBT_TYPE_BYTE:
            nbt = nbt_tag_byte_create(0);
            break;
        case NBT_TYPE_SHORT:
            nbt = nbt_tag_short_create(0);
            break;
        case NBT_TYPE_INT:
            nbt = nbt_tag_int_create(0);
            break;
        case NBT_TYPE_LONG:
            nbt = nbt_tag_long_create(0);
            break;
        case NBT_TYPE_FLOAT:
            nbt = nbt_tag_float_create(0);
            break;
        case NBT_TYPE_DOUBLE:
            nbt = nbt_tag_double_create(0);
            break;
        case NBT_TYPE_BYTE_ARRAY:
            nbt = nbt_tag_byte_array_create();
            break;
        case NBT_TYPE_STRING:
            nbt = nbt_tag_string_create();
            break;
        case NBT_TYPE_LIST:
            nbt = nbt_tag_list_create();
            break;
        case NBT_TYPE_COMPOUND:
            nbt = nbt_tag_compound_create();
            break;
    }
    return nbt;
}

int8_t gz_read_byte(gzFile file) {
    int8_t value = 0;
    if(gzread(file, &value, sizeof(value)) != sizeof(value)) {
        fprintf(stderr, "Failed to read byte\n");
    }
    return value;
}

int16_t gz_read_short(gzFile file) {
    int16_t value = 0;
    if(gzread(file, &value, sizeof(value)) != sizeof(value)) {
        fprintf(stderr, "Failed to read short\n");
    }
    return value;
}

int32_t gz_read_int(gzFile file) {
    int32_t value = 0;
    if(gzread(file, &value, sizeof(value)) != sizeof(value)) {
        fprintf(stderr, "Failed to read int\n");
    }
    return value;
}

int64_t gz_read_long(gzFile file) {
    int64_t value = 0;
    if(gzread(file, &value, sizeof(value)) != sizeof(value)) {
        fprintf(stderr, "Failed to read long\n");
    }
    return value;
}

float gz_read_float(gzFile file) {
    float value = 0;
    if(gzread(file, &value, sizeof(value)) != sizeof(value)) {
        fprintf(stderr, "Failed to read float\n");
    }
    return value;
}

double gz_read_double(gzFile file) {
    double value = 0;
    if(gzread(file, &value, sizeof(value)) != sizeof(value)) {
        fprintf(stderr, "Failed to read double\n");
    }
    return value;
}

void gz_read_fully(gzFile file, void *buffer, size_t size) {
    if(gzread(file, buffer, size) != size) {
        fprintf(stderr, "Failed to read fully\n");
    }
}

nbt_base_t nbt_read_named_tag(gzFile file) {
    int8_t tag_type = gz_read_byte(file);
    if(tag_type == NBT_TYPE_END) return (nbt_base_t){ 0 };

    nbt_base_t nbt = nbt_create_tag_of_type(tag_type);
    int16_t length = gz_read_short(file);
    if(length == 0) { // root tag handling
        nbt_set_key(&nbt, "");
        nbt.read_contents(&nbt, file);
        return nbt;
    }
    char *key = malloc(sizeof(char) * length + 1);
    gz_read_fully(file, key, length);
    key[length] = '\0';
    nbt_set_key(&nbt, key);
    free(key);
    nbt.read_contents(&nbt, file);

    return nbt;
}

void nbt_write_named_tag(gzFile file, nbt_base_t *nbt) {
    gzwrite(file, &nbt->type, sizeof(nbt->type));
    if(nbt->type != 0) {
        int16_t length = strlen(nbt->key);
        gzwrite(file, &length, sizeof(length));
        gzwrite(file, nbt->key, length);
        nbt->write_contents(nbt, file);
    }
}