#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_float_create(float value);
void nbt_tag_float_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_tag_float_write_contents(nbt_base_t *nbt, gzFile file);