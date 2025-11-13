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
#include <stdint.h>

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
    uint8_t buf[2] = {0};
    if(gzread(file, buf, 2) != 2) {
        fprintf(stderr, "Failed to read short\n");
        return 0;
    }
    return (int16_t)((buf[0] << 8) | buf[1]);
}

int32_t gz_read_int(gzFile file) {
    uint8_t buf[4] = {0};
    if(gzread(file, buf, 4) != 4) {
        fprintf(stderr, "Failed to read int\n");
        return 0;
    }
    return (int32_t)((uint32_t)buf[0] << 24 | (uint32_t)buf[1] << 16 | (uint32_t)buf[2] << 8 | (uint32_t)buf[3]);
}

int64_t gz_read_long(gzFile file) {
    uint8_t buf[8] = {0};
    if(gzread(file, buf, 8) != 8) {
        fprintf(stderr, "Failed to read long\n");
        return 0;
    }
    uint64_t v = ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) | ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32)
        | ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) | ((uint64_t)buf[6] << 8) | (uint64_t)buf[7];
    return (int64_t)v;
}

float gz_read_float(gzFile file) {
    uint32_t bits = (uint32_t)gz_read_int(file);
    float value = 0.0f;
    memcpy(&value, &bits, sizeof(bits));
    return value;
}

double gz_read_double(gzFile file) {
    uint8_t buf[8] = {0};
    if(gzread(file, buf, 8) != 8) {
        fprintf(stderr, "Failed to read double\n");
        return 0.0;
    }
    uint64_t bits = ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) | ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32)
        | ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) | ((uint64_t)buf[6] << 8) | (uint64_t)buf[7];
    double value = 0.0;
    memcpy(&value, &bits, sizeof(bits));
    return value;
}

void gz_read_fully(gzFile file, void *buffer, size_t size) {
    if(gzread(file, buffer, size) != (int)size) {
        fprintf(stderr, "Failed to read fully\n");
    }
}

void gz_write_byte(gzFile file, int8_t value) {
    if(gzwrite(file, &value, 1) != 1) {
        fprintf(stderr, "Failed to write byte\n");
    }
}

void gz_write_short(gzFile file, int16_t value) {
    uint8_t buf[2];
    buf[0] = (value >> 8) & 0xFF;
    buf[1] = value & 0xFF;
    if(gzwrite(file, buf, 2) != 2) {
        fprintf(stderr, "Failed to write short\n");
    }
}

void gz_write_int(gzFile file, int32_t value) {
    uint8_t buf[4];
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8) & 0xFF;
    buf[3] = value & 0xFF;
    if(gzwrite(file, buf, 4) != 4) {
        fprintf(stderr, "Failed to write int\n");
    }
}

void gz_write_long(gzFile file, int64_t value) {
    uint8_t buf[8];
    uint64_t v = (uint64_t)value;
    buf[0] = (v >> 56) & 0xFF;
    buf[1] = (v >> 48) & 0xFF;
    buf[2] = (v >> 40) & 0xFF;
    buf[3] = (v >> 32) & 0xFF;
    buf[4] = (v >> 24) & 0xFF;
    buf[5] = (v >> 16) & 0xFF;
    buf[6] = (v >> 8) & 0xFF;
    buf[7] = v & 0xFF;
    if(gzwrite(file, buf, 8) != 8) {
        fprintf(stderr, "Failed to write long\n");
    }
}

void gz_write_float(gzFile file, float value) {
    uint32_t bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    gz_write_int(file, (int32_t)bits);
}

void gz_write_double(gzFile file, double value) {
    uint64_t bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    gz_write_long(file, (int64_t)bits);
}

void gz_write_fully(gzFile file, void *buffer, size_t size) {
    if(gzwrite(file, buffer, (unsigned)size) != (int)size) {
        fprintf(stderr, "Failed to write fully\n");
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
    gz_write_byte(file, nbt->type);
    if(nbt->type != 0) {
        int16_t length = strlen(nbt->key);
        gz_write_short(file, length);
        if(length > 0) gz_write_fully(file, nbt->key, length);
        nbt->write_contents(nbt, file);
    }
}