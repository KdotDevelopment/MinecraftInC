#pragma once

#include <stdint.h>

int8_t *nibble_array_create(int size);
int8_t *nibble_array_create_from(int8_t *data, int size);
int8_t nibble_array_get(int8_t *nibble_array, int x, int y, int z);
void nibble_array_set(int8_t *nibble_array, int x, int y, int z, int8_t data);