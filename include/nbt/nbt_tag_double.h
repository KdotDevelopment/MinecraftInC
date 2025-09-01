#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_double_create(double value);
void nbt_tag_double_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_tag_double_write_contents(nbt_base_t *nbt, gzFile file);