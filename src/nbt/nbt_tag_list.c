#include <nbt/nbt_tag_list.h>

#include <util/array_list.h>

#include <string.h>

nbt_base_t nbt_tag_list_create() {
    nbt_base_t tag = { 0 };

    tag.type = NBT_TYPE_LIST;
    tag.tag_array = array_list_create(sizeof(nbt_base_t));
    
    return tag;
}

void nbt_tag_list_set_tag(nbt_base_t *base, nbt_base_t *tag) {
    base->array_list_type = tag->array_list_type;
    base->tag_array = array_list_push(base->tag_array, tag);
}

nbt_base_t *nbt_tag_list_get_tag(nbt_base_t *base, int index) {
    return array_list_get(base->tag_array, index);
}