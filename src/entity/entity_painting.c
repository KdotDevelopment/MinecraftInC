#include <entity/entity_painting.h>

#include <entity/entity_item.h>
#include <nbt/nbt_tag_compound.h>
#include <item/items.h>
#include <util/array_list.h>

#include <string.h>

void entity_painting_create(entity_t *entity, world_t *world, int x, int y, int z, int8_t side) {
    entity_create(entity, world);
    entity->bb_width = 0.5;
    entity->bb_height = 0.5;
    entity->type = ENTITY_PAINTING;

    entity->painting_pos_x = x;
    entity->painting_pos_y = y;
    entity->painting_pos_z = z;
    entity->painting_ticks = 0;

    painting_t **possible_paintings = array_list_create(sizeof(painting_t *));

    for(int i = 0; i < painting_count; i++) {
        painting_t *painting = &painting_list[i];
        entity->painting = painting;
        entity_painting_set_direction(entity, side);
        if(entity_painting_on_valid_surface(entity)) {
            possible_paintings = array_list_push(possible_paintings, &painting);
        }
    }

    if(array_list_length(possible_paintings) > 0) {
        int index = random_next_int_range(&world->random, 0, array_list_length(possible_paintings) - 1);
        painting_t *selected_painting = *(painting_t **)array_list_get(possible_paintings, index);
        entity->painting = selected_painting;
    }

    entity_painting_set_direction(&entity, side);

    array_list_free(possible_paintings);

    entity->tick = entity_painting_tick;
    entity->hurt = entity_painting_hurt;
    entity->write_nbt = entity_painting_write_nbt;
    entity->read_nbt = entity_painting_read_nbt;
}

float private_get_art_size(int size) {
    return size == 32 ? 0.5 : (size == 64 ? 0.5 : 0.0);
}

void entity_painting_set_direction(entity_t *entity, int8_t side) {
    entity->painting_direction = side;
    entity->y_rot = side * 90;
    entity->y_roto = entity->y_rot;
    float size_x = entity->painting->size_x;
    float size_y = entity->painting->size_y;
    float size_z = entity->painting->size_x;

    if(side != 0 && side != 2) size_x = 0.5;
    else size_z = 0.5;

    size_x /= 32.0;
    size_y /= 32.0;
    size_z /= 32.0;
    float pos_x = entity->painting_pos_x + 0.5;
    float pos_y = entity->painting_pos_y + 0.5;
    float pos_z = entity->painting_pos_z + 0.5;

    if(side == 0) pos_z -= 9.0 / 16.0;
    if(side == 1) pos_x -= 9.0 / 16.0;
    if(side == 2) pos_z += 9.0 / 16.0;
    if(side == 3) pos_x += 9.0 / 16.0;
    if(side == 0) pos_x -= private_get_art_size(entity->painting->size_x);
    if(side == 1) pos_z += private_get_art_size(entity->painting->size_x);
    if(side == 2) pos_x += private_get_art_size(entity->painting->size_x);
    if(side == 3) pos_z -= private_get_art_size(entity->painting->size_x);

    size_y += private_get_art_size(entity->painting->size_y);
    entity_set_pos(entity, pos_x, pos_y, pos_z);
    entity->bb = (AABB_t){ pos_x - size_x, pos_y - size_y, pos_z - size_z, pos_x + size_x - 0.00625, pos_y + size_y - 0.00625, pos_z + size_z - 0.00625 };
}

void entity_painting_tick(entity_t *entity) {
    if(entity->painting_ticks++ == 100 && !entity_painting_on_valid_surface(entity)) {
        entity->painting_ticks = 0;
        entity->is_dead = 1;
        entity_t *item = malloc(sizeof(entity_t));
        item_stack_t item_stack = item_stack_create(items.painting.item_id, 1, 0);
        entity_item_create(item, entity->world, entity->x, entity->y, entity->z, item_stack);
        world_spawn_entity(entity->world, item);
    }
}

uint8_t entity_painting_on_valid_surface(entity_t *entity) {
    AABB_t *bb_list = world_get_cubes(entity->world, entity->bb);
    if(array_list_length(bb_list) > 0) {
        array_list_free(bb_list);
        return 0;
    }
    array_list_free(bb_list);
    
    int size_x = entity->painting->size_x / 16;
    int size_y = entity->painting->size_y / 16;
    int pos_x = entity->painting_pos_x;
    int pos_z = entity->painting_pos_z;
    if(entity->painting_direction == 0) pos_x = floor_double(entity->x - (entity->painting->size_x / 32.0));
    if(entity->painting_direction == 1) pos_z = floor_double(entity->z + (entity->painting->size_x / 32.0));
    if(entity->painting_direction == 2) pos_x = floor_double(entity->x + (entity->painting->size_x / 32.0));
    if(entity->painting_direction == 3) pos_z = floor_double(entity->z - (entity->painting->size_x / 32.0));
    int pos_y = floor_double(entity->y - (entity->painting->size_y / 32.0));

    for(int x = 0; x < size_x; x++) {
        for(int y = 0; y < size_y; y++) {
            material_t *material;
            if(entity->painting_direction != 0 && entity->painting_direction != 2) {
                material = world_get_block_material(entity->world, entity->painting_pos_x, pos_y + y, pos_z + x);
            }else {
                material = world_get_block_material(entity->world, pos_x + x, pos_y + y, entity->painting_pos_z);
            }

            if(!material->is_solid) return 0;
        }
    }

    entity_t **entity_list = world_get_entities_excluding(entity->world, entity, entity->bb);
    for(int i = 0; i < array_list_length(entity_list); i++) {
        entity_t *other = entity_list[i];
        if(other->type == ENTITY_PAINTING) {
            array_list_free(entity_list);
            return 0;
        }
    }

    return 1;
}

void entity_painting_hurt(entity_t *entity, entity_t *other, int damage) {
    entity->is_dead = 1;
    entity_t *item = malloc(sizeof(entity_t));
    item_stack_t item_stack = item_stack_create(items.painting.item_id, 1, 0);
    entity_item_create(item, entity->world, entity->x, entity->y, entity->z, item_stack);
    world_spawn_entity(entity->world, item);

    //return 1;
}

void entity_painting_write_nbt(entity_t *entity, nbt_base_t *nbt) {
    nbt_tag_compound_set_byte(nbt, "Dir", entity->painting_direction);
    nbt_tag_compound_set_string(nbt, "Motive", entity->painting->title);
    nbt_tag_compound_set_int(nbt, "TileX", entity->painting_pos_x);
    nbt_tag_compound_set_int(nbt, "TileY", entity->painting_pos_y);
    nbt_tag_compound_set_int(nbt, "TileZ", entity->painting_pos_z);
}

void entity_painting_read_nbt(entity_t *entity, nbt_base_t *nbt) {
    entity->painting_direction = nbt_tag_compound_get_byte(nbt, "Dir");
    entity->painting_pos_x = nbt_tag_compound_get_int(nbt, "TileX");
    entity->painting_pos_y = nbt_tag_compound_get_int(nbt, "TileY");
    entity->painting_pos_z = nbt_tag_compound_get_int(nbt, "TileZ");
    char *title = nbt_tag_compound_get_string(nbt, "Motive");

    for(int i = 0; i < painting_count; i++) {
        if(strcmp(painting_list[i].title, title) == 0) {
            entity->painting = &painting_list[i];
        }
    }

    if(entity->painting == NULL) {
        entity->painting = &painting_list[0];
    }

    entity_painting_set_direction(entity, entity->painting_direction);
}