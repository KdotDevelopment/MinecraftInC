#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_byte_create(int8_t value);
void nbt_tag_byte_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_tag_byte_write_contents(nbt_base_t *nbt, gzFile file);