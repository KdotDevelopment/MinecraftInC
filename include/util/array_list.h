#pragma once

#include <stdint.h>

typedef struct {
    uint32_t element_size;
    uint32_t capacity; // Amount of memory (in increments of element_size) allocated to the list, not necessarily how much is used
    uint32_t length; // Number of elements in array
} array_list_t;

void *array_list_create(uint32_t element_size);
uint32_t array_list_length(void *array_list);
uint32_t array_list_element_size(void *array_list);
uint32_t array_list_capacity(void *array_list);
void *array_list_insert(void *array_list, void *element, int32_t index);
void *array_list_remove(void *array_list, int32_t index);
void *array_list_push(void *array_list, void *element);
void *array_list_pop(void *array_list, void *element);
void *array_list_remove_all(void *array_list, void *element);
void array_list_set(void *array_list, int32_t index, void *element);
void *array_list_get(void *array_list, int32_t index);
int32_t array_list_index_of(void *array_list, void *element);
uint8_t array_list_contains(void *array_list, void *element);
void *array_list_clear(void *array_list);
void *array_list_clone(void *array_list);
void array_list_free(void *array_list);