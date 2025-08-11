#include <world/material/materials.h>

#include <world/material/material_liquid.h>
#include <world/material/material_logic.h>
#include <world/material/material_transparent.h>
#include <world/material/material.h>

struct materials_s materials = { 0 };

void materials_init() {
    materials.air = material_transparent_create();
    materials.ground = material_create();
    materials.wood = material_create();
    materials.rock = material_create();
    materials.metal = material_create();
    materials.water = material_liquid_create();
    materials.lava = material_liquid_create();
    materials.leaves = material_create();
    materials.plants = material_logic_create();
    materials.sponge = material_create();
    materials.wool = material_create();
    materials.fire = material_transparent_create();
    materials.sand = material_create();
    materials.logic = material_logic_create();
    materials.glass = material_create();
    materials.tnt = material_create();
}