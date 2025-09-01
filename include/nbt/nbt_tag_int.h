#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_int_create(int32_t value);
void nbt_tag_int_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_tag_int_write_contents(nbt_base_t *nbt, gzFile file);