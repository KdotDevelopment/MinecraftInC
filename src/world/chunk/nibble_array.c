#include <world/chunk/nibble_array.h>

#include <stdlib.h>
#include <string.h>

uint8_t *nibble_array_create(int size) {
    uint8_t *nibble_array = malloc(size / 2);

    memset(nibble_array, 0, size / 2);

    return nibble_array;
}

uint8_t *nibble_array_create_from(uint8_t *data, int size) {
    uint8_t *nibble_array = malloc(size / 2);

    memcpy(nibble_array, data, size / 2);

    return nibble_array;
}

uint8_t nibble_array_get(uint8_t *nibble_array, int x, int y, int z) {
    if(x < 0 || y < 0 || z < 0) return 0;
    x = x << 11 | z << 7 | y;
    y = x >> 1;
    x &= 1;
    return x == 0 ? nibble_array[y] & 0x0F : nibble_array[y] >> 4 & 0x0F;
}

void nibble_array_set(uint8_t *nibble_array, int x, int y, int z, uint8_t data) {
    if(x < 0 || y < 0 || z < 0) return;
    x = x << 11 | z << 7 | y;
    y = x / 2;
    x &= 1;
    if(x == 0) {
        nibble_array[y] = (uint8_t)(nibble_array[y] & 0xF0) | (data & 0x0F);
    } else {
        nibble_array[y] = (uint8_t)(nibble_array[y] & 0x0F) | ((data & 0x0F) << 4);
    }
}