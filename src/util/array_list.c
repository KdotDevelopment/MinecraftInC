#include <util/array_list.h>

#include <stdlib.h>
#include <string.h>

void *array_list_create(uint32_t element_size) {
    // Allocate array of only one element right now
    array_list_t *list = malloc(sizeof(array_list_t) + element_size);
    list->element_size = element_size;
    list->capacity = 1;
    list->length = 0;
    // Returns the data after the list info so the struct is invisible
    // [ struct ] [ element_1 ]
    //           ^ return this address
    return list + 1;
}

uint32_t array_list_length(void *array_list) {
    return ((array_list_t *)array_list - 1)->length;
}

uint32_t array_list_element_size(void *array_list) {
    return ((array_list_t *)array_list - 1)->element_size;
}

uint32_t array_list_capacity(void *array_list) {
    return ((array_list_t *)array_list - 1)->capacity;
}

void *array_list_insert(void *array_list, void *element, int32_t index) {
    array_list_t *meta = (array_list_t *)array_list - 1;
    meta->length++;

    // Expand memory allocation as needed
    if(meta->length == meta->capacity) {
        meta->capacity *= 2;
        meta = realloc(meta, sizeof(array_list_t) + meta->capacity * meta->element_size);
        array_list = meta + 1;
    }

    memmove((uint8_t *)array_list + (index + 1) * meta->element_size, (uint8_t *)array_list + index * meta->element_size, (meta->length - 1 - index) * meta->element_size);
    memcpy((uint8_t *)array_list + index * meta->element_size, element, meta->element_size);
    return array_list;
}

void *array_list_remove(void *array_list, int32_t index) {
    array_list_t *meta = (array_list_t *)array_list - 1;

    // Pull back all elements ahead by one
    for(uint32_t i = (index + 1) * meta->element_size; i < meta->length * meta->element_size; i++) {
        ((uint8_t *)array_list)[i - meta->element_size] = ((uint8_t *)array_list)[i];
    }

    meta->length--;

    // Reduce memory allocation as needed
    if(meta->length == meta->capacity / 2 - 1) {
        meta->capacity /= 2;
        meta = realloc(meta, sizeof(array_list_t) + meta->capacity * meta->element_size);
        array_list = meta + 1;
    }

    return array_list;
}

void *array_list_push(void *array_list, void *element) {
    return array_list_insert(array_list, element, array_list_length(array_list));
}

void *array_list_pop(void *array_list, void *element) {
    return array_list_remove(array_list, array_list_length(array_list) - 1);
}

void *array_list_remove_all(void *array_list, void *element) {
    for(uint32_t i = 0; i < array_list_length(array_list); i++) {
        // If the element at this position is the element we are looking for
        if(memcmp((uint8_t *)array_list + i * array_list_element_size(array_list), element, array_list_element_size(array_list)) == 0) {
            array_list = array_list_remove(array_list, i);
            i--;
        }
    }

    return array_list;
}

void array_list_set(void *array_list, int32_t index, void *element) {
    array_list_t *meta = (array_list_t *)array_list - 1;
    memcpy((uint8_t *)array_list + index * meta->element_size, element, meta->element_size);
}

void *array_list_get(void *array_list, int32_t index) {
    array_list_t *meta = (array_list_t *)array_list - 1;
    return (uint8_t *)array_list + index * meta->element_size;
}

int32_t array_list_index_of(void *array_list, void *element) {
    for(uint32_t i = 0; i < array_list_length(array_list); i++) {
        if(memcmp((uint8_t *)array_list + i * array_list_element_size(array_list), element, array_list_element_size(array_list)) == 0) {
            return i;
        }
    }

    return -1; // Not Found
}

uint8_t array_list_contains(void *array_list, void *element) {
    return array_list_index_of(array_list, element) >= 0;
}

void *array_list_clear(void *array_list) {
    void *new_list = array_list_create(array_list_element_size(array_list));
    array_list_free(array_list);
    return new_list;
}

void *array_list_clone(void *array_list) {
    uint64_t size = sizeof(array_list_t) + array_list_element_size(array_list) * array_list_capacity(array_list);
    array_list_t *clone = calloc(size, 1);
    memcpy(clone, (array_list_t *)array_list - 1, size);
    return clone + 1;
}

void array_list_free(void *array_list) {
    free((array_list_t *)array_list - 1);
}