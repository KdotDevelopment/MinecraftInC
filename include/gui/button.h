#pragma once

#include <stdint.h>

typedef struct {
    int width; // = 200
    int height; // = 20
    int x;
    int y;
    int id;
    char *string;
    uint8_t active;
    uint8_t visible;
} button_t;

button_t button_create(int id, int x, int y, char *string);
button_t button_create_size(int id, int x, int y, int width, int height, char *string);
void button_destroy(button_t *button);