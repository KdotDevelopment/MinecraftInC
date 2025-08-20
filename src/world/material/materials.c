#include <world/material/materials.h>

#include <world/material/material_liquid.h>
#include <world/material/material_logic.h>
#include <world/material/material_transparent.h>
#include <world/material/material.h>

struct materials_s materials = { 0 };

void materials_init() {
    materials.air = material_transparent_create(0);
    materials.ground = material_create(1);
    materials.wood = material_create(2);
    materials.rock = material_create(3);
    materials.metal = material_create(4);
    materials.water = material_liquid_create(5);
    materials.lava = material_liquid_create(6);
    materials.leaves = material_create(7);
    materials.plants = material_logic_create(8);
    materials.sponge = material_create(9);
    materials.wool = material_create(10);
    materials.fire = material_transparent_create(11);
    materials.sand = material_create(12);
    materials.logic = material_logic_create(13);
    materials.glass = material_create(14);
    materials.tnt = material_create(15);
}