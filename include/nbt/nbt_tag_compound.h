#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_compound_create();

void nbt_tag_compound_set_tag(nbt_base_t *base, char *key, nbt_base_t *tag);
void nbt_tag_compound_set_byte(nbt_base_t *base, char *key, int8_t value);
void nbt_tag_compound_set_short(nbt_base_t *base, char *key, int16_t value);
void nbt_tag_compound_set_int(nbt_base_t *base, char *key, int32_t value);
void nbt_tag_compound_set_long(nbt_base_t *base, char *key, int64_t value);
void nbt_tag_compound_set_float(nbt_base_t *base, char *key, float value);
void nbt_tag_compound_set_string(nbt_base_t *base, char *key, char *value);
void nbt_tag_compound_set_byte_array(nbt_base_t *base, char *key, uint8_t *value, uint32_t length);
void nbt_tag_compound_set_compound_tag(nbt_base_t *base, char *key, nbt_base_t *tag);
void nbt_tag_compound_set_boolean(nbt_base_t *base, char *key, int8_t value);

uint8_t nbt_tag_compound_has_key(nbt_base_t *base, char *key);
int8_t nbt_tag_compound_get_byte(nbt_base_t *base, char *key);
int16_t nbt_tag_compound_get_short(nbt_base_t *base, char *key);
int32_t nbt_tag_compound_get_int(nbt_base_t *base, char *key);
int64_t nbt_tag_compound_get_long(nbt_base_t *base, char *key);
float nbt_tag_compound_get_float(nbt_base_t *base, char *key);
char *nbt_tag_compound_get_string(nbt_base_t *base, char *key);
int8_t *nbt_tag_compound_get_byte_array(nbt_base_t *base, char *key, uint32_t length);
nbt_base_t nbt_tag_compound_get_compound_tag(nbt_base_t *base, char *key);
nbt_base_t nbt_tag_compound_get_tag_list(nbt_base_t *base, char *key);
int8_t nbt_tag_compound_get_boolean(nbt_base_t *base, char *key);

char *nbt_tag_compound_to_string(nbt_base_t *base);

void nbt_tag_compound_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_tag_compound_write_contents(nbt_base_t *nbt, gzFile file);

void nbt_tag_compound_free(nbt_base_t *base);