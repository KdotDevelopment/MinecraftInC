#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_long_create(int64_t value);
void nbt_tag_long_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_tag_long_write_contents(nbt_base_t *nbt, gzFile file);