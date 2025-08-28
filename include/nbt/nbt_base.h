#pragma once

#include <stdint.h>

#define MAX_NBT_KEY_LENGTH 32

enum {
    NBT_TYPE_END,
    NBT_TYPE_BYTE,
    NBT_TYPE_SHORT,
    NBT_TYPE_INT,
    NBT_TYPE_LONG,
    NBT_TYPE_FLOAT,
    NBT_TYPE_DOUBLE,
    NBT_TYPE_BYTE_ARRAY,
    NBT_TYPE_STRING,
    NBT_TYPE_LIST,
    NBT_TYPE_COMPOUND
};

typedef struct nbt_base_s {
    char key[MAX_NBT_KEY_LENGTH];
    uint8_t type;
    union {
        uint8_t byte_value;
        uint8_t *byte_array; // arraylist
        double double_value;
        float float_value;
        int32_t int_value;
        int64_t long_value;
        int16_t short_value;
        char *string_value;
        struct {
            nbt_base_t *tag_array; // arraylist, nbt_tag_compound
            uint8_t array_list_type; // tag_list
        };
    };
} nbt_base_t;