#include <nbt/nbt_tag_list.h>

#include <util/array_list.h>

#include <string.h>
#include <stdlib.h>

nbt_base_t nbt_tag_list_create() {
    nbt_base_t tag = { 0 };
    memset(&tag, 0, sizeof(nbt_base_t));

    tag.type = NBT_TYPE_LIST;
    tag.tag_array = array_list_create(sizeof(nbt_base_t));

    tag.read_contents = nbt_tag_list_read_contents;
    tag.write_contents = nbt_tag_list_write_contents;

    return tag;
}

void nbt_tag_list_set_tag(nbt_base_t *base, nbt_base_t *tag) {
    base->array_list_type = tag->array_list_type;
    base->tag_array = array_list_push(base->tag_array, tag);
}

nbt_base_t *nbt_tag_list_get_tag(nbt_base_t *base, int index) {
    return array_list_get(base->tag_array, index);
}

void nbt_tag_list_read_contents(nbt_base_t *nbt, gzFile file) {
    int tag_type = gz_read_byte(file);
    int length = gz_read_int(file);
    if(!nbt->tag_array) {
        nbt->tag_array = array_list_create(sizeof(nbt_base_t));
    }
    for(int i = 0; i < length; i++) {
        nbt_base_t *tag = malloc(sizeof(nbt_base_t));
        *tag = nbt_create_tag_of_type(tag_type);
        tag->read_contents(tag, file);
        nbt->tag_array = array_list_push(nbt->tag_array, tag);
    }
}

void nbt_tag_list_write_contents(nbt_base_t *nbt, gzFile file) {
    if(array_list_length(nbt->tag_array) > 0) {
        nbt->array_list_type = ((nbt_base_t *)array_list_get(nbt->tag_array, 0))->type;
    }else {
        nbt->array_list_type = NBT_TYPE_BYTE;
    }

    gzwrite(file, &nbt->array_list_type, sizeof(nbt->array_list_type));
    int length = array_list_length(nbt->tag_array);
    gzwrite(file, &length, sizeof(int));
    for(int i = 0; i < length; i++) {
        nbt_base_t *tag = array_list_get(nbt->tag_array, i);
        tag->write_contents(tag, file);
    }
}