#pragma once

#include <stdint.h>

uint8_t *nibble_array_create(int size);
uint8_t *nibble_array_create_from(uint8_t *data, int size);
uint8_t nibble_array_get(uint8_t *nibble_array, int x, int y, int z);
uint8_t nibble_array_set(uint8_t *nibble_array, int x, int y, int z, uint8_t data);