#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_string_create();
nbt_base_t nbt_tag_string_create_from(char *value);
void nbt_tag_string_read_contents(nbt_base_t *nbt, gzFile file);
void nbt_tag_string_write_contents(nbt_base_t *nbt, gzFile file);