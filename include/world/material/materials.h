#pragma once

#include <world/material/material.h>

extern struct materials_s {
    material_t air;
    material_t ground;
    material_t wood;
    material_t rock;
    material_t metal;
    material_t water;
    material_t lava;
    material_t leaves;
    material_t plants;
    material_t sponge;
    material_t wool;
    material_t fire;
    material_t sand;
    material_t logic;
    material_t glass;
    material_t tnt;
} materials;

void materials_init();