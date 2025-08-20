#include <world/material/material.h>

material_t material_create(uint8_t id) {
    material_t material = { 0 };

    material.id = id;
    material.is_liquid = 0;
    material.is_solid = 1;
    material.can_block_grass = 1;
    
    return material;
}