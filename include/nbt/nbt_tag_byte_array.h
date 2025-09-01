#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_byte_array_create();
nbt_base_t nbt_tag_byte_array_create_from(uint8_t *value, uint32_t length);
void nbt_tag_byte_array_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_tag_byte_array_write_contents(nbt_base_t *nbt, gzFile file);