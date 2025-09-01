#pragma once

#include <nbt/nbt_base.h>

nbt_base_t nbt_tag_list_create();
void nbt_tag_list_set_tag(nbt_base_t *base, nbt_base_t *tag);
nbt_base_t *nbt_tag_list_get_tag(nbt_base_t *base, int index);