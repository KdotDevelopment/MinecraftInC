#pragma once

typedef struct painting_s {
    char *title;
    int size_x;
    int size_y;
    int offset_x;
    int offset_y;
} painting_t;

// Defiend in entity_painting.c
extern int painting_count;
extern painting_t painting_list[];