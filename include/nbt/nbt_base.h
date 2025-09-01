#pragma once

#include <stdint.h>
#include <zlib.h>

#define MAX_NBT_KEY_LENGTH 64

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
    uint8_t null;
    union {
        uint8_t byte_value;
        struct {
            uint8_t *byte_array;
            uint32_t byte_array_length;
        };
        double double_value;
        float float_value;
        int32_t int_value;
        int64_t long_value;
        int16_t short_value;
        char *string_value;
        struct {
            struct nbt_base_s *tag_array; // arraylist, nbt_tag_compound
            uint8_t array_list_type; // tag_list
        };
    };

    void (*read_contents)(struct nbt_base_s *base, gzFile file);
    void (*write_contents)(struct nbt_base_s *base, gzFile file);
} nbt_base_t;

nbt_base_t *nbt_set_key(nbt_base_t *base, char *key);
void nbt_base_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_base_write_contents(nbt_base_t *nbt, gzFile file);
nbt_base_t nbt_create_tag_of_type(int8_t tag_type);
int8_t gz_read_byte(gzFile file);
int16_t gz_read_short(gzFile file);
int32_t gz_read_int(gzFile file);
int64_t gz_read_long(gzFile file);
float gz_read_float(gzFile file);
double gz_read_double(gzFile file);
nbt_base_t nbt_read_named_tag(gzFile file);
void nbt_write_named_tag(gzFile file, nbt_base_t *nbt);