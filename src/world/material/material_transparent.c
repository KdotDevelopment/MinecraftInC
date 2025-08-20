#include <world/material/material_transparent.h>

material_t material_transparent_create(uint8_t id) {
    material_t material = material_create(id);

    material.is_liquid = 0;
    material.is_solid = 0;
    material.can_block_grass = 0;
    
    return material;
}