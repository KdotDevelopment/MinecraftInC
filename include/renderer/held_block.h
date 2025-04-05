#pragma once

#include <level/block/block.h>

#include <stdint.h>

typedef struct {
    struct minecraft_s *minecraft;
    block_t *block;
    float position;
    float last_position;
    int offset;
    uint8_t moving;
} held_block_t;