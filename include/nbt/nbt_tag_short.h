#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_short_create(int16_t value);
void nbt_tag_short_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_tag_short_write_contents(nbt_base_t *nbt, gzFile file);