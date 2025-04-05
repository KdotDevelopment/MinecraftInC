#include <level/entity_map.h>
#include <entity/mob/mob.h>

#include <util/array_list.h>

#include <stdlib.h>

void entity_map_create(entity_map_t *map, int width, int height, int depth) {
    map->width = width / 16;
    map->height = height / 16;
    map->depth = depth / 16;
    entity_map_slot_create(&map->temp_slot_1, map);
    entity_map_slot_create(&map->temp_slot_2, map);
    map->entities = malloc(sizeof(entity_t **) * map->width * map->height * map->depth);
    for(int x = 0; x < map->width; x++) {
        for(int z = 0; z < map->depth; z++) {
            for(int y = 0; y < map->height; y++) {
                map->entities[(y * map->depth + z) * map->width + x] = array_list_create(sizeof(entity_t *));
            }
        }
    }
    map->all = array_list_create(sizeof(entity_t *));
    map->temp = array_list_create(sizeof(entity_t *));
}

void entity_map_insert(entity_map_t *map, entity_t *entity) {
    map->all = array_list_push(map->all, &entity);
    entity_map_slot_init(&map->temp_slot_1, entity->x, entity->y, entity->z);
    entity_map_slot_add(&map->temp_slot_1, entity);
    entity->xo = entity->x;
    entity->yo = entity->y;
    entity->zo = entity->z;
    entity->entity_map = map;
}

void entity_map_remove(entity_map_t *map, entity_t *entity) {
    entity_map_slot_init(&map->temp_slot_1, entity->xo, entity->yo, entity->zo);
    entity_map_slot_remove(&map->temp_slot_1, entity);
    int index = array_list_index_of(map->all, entity);
    map->all = array_list_remove(map->all, index);
}

void entity_map_moved(entity_map_t *map, entity_t *entity) {
    entity_map_slot_init(&map->temp_slot_1, entity->xo, entity->yo, entity->zo);
    entity_map_slot_init(&map->temp_slot_2, entity->x, entity->y, entity->z);
    entity_map_slot_t *slot1 = &map->temp_slot_1;
    entity_map_slot_t *slot2 = &map->temp_slot_2;
    if(!entity_map_equals(slot1, slot2)) {
        entity_map_slot_remove(slot1, entity);
        entity_map_slot_add(slot2, entity);
    }
}

entity_t ***entity_map_get_entities(entity_map_t *map, entity_t *entity, float x0, float y0, float z0, float x1, float y1, float z1) {
    map->temp = array_list_clear(map->temp);
    entity_map_slot_init(&map->temp_slot_1, x0, y0, z0);
    entity_map_slot_init(&map->temp_slot_2, x1, y1, z1);

    for(int x = map->temp_slot_1.x - 1; x <= map->temp_slot_2.x + 1; x++) {
        for(int z = map->temp_slot_1.z - 1; z <= map->temp_slot_2.z + 1; z++) {
            for(int y = map->temp_slot_1.y - 1; y <= map->temp_slot_2.y + 1; y++) {
                if(x >= 0 && y >= 0 && z >= 0 && x < map->width && z < map->depth && y < map->height) {
                    entity_t **list = map->entities[(y * map->depth + z) * map->width + x];
                    
                    for(int i = 0; i < array_list_length(list); i++) {
                        entity_t *retrieved = *(entity_t **)array_list_get(list, i);
                        if(retrieved != entity && AABB_intersects(retrieved->bb, (AABB_t){ x0, y0, z0, x1, y1, z1 })) {
                            map->temp = array_list_push(map->temp, &retrieved);
                        }
                    }
                }
            }
        }
    }

    return &map->temp;
}

void entity_map_clear_non_creative_mode_entities(entity_map_t *map) {
    for(int x = 0; x < map->width; x++) {
        for(int z = 0; z < map->depth; z++) {
            for(int y = 0; y < map->height; y++) {
                entity_t **list = map->entities[(y * map->depth + z) * map->width + x];

                for(int i = 0; i < array_list_length(list); i++) {
                    entity_t *entity = *(entity_t **)array_list_get(list, i);
                    if(!entity->allowed_in_creative_mode) {
                        list = array_list_remove(list, i--);
                    }
                }
                map->entities[(y * map->depth + z) * map->width + x] = list;
            }
        }
    }
}

void entity_map_clear(entity_map_t *map) {
    for(int x = 0; x < map->width; x++) {
        for(int z = 0; z < map->depth; z++) {
            for(int y = 0; y < map->height; y++) {
                entity_t **list = map->entities[(y * map->depth + z) * map->width + x];
                list = array_list_clear(list);
                map->entities[(y * map->depth + z) * map->width + x] = list;
            }
        }
    }
}

void entity_map_tick_all(entity_map_t *map) {
    for(int i = 0; i < array_list_length(map->all); i++) {
        entity_t *entity = *(entity_t **)array_list_get(map->all, i);
        entity->tick(entity);
        if(entity->removed) {
            map->all = array_list_remove(map->all, i--);
            entity_map_slot_init(&map->temp_slot_1, entity->xo, entity->yo, entity->zo);
            entity_map_slot_remove(&map->temp_slot_1, entity);
            if(entity->type != ENTITY_MOB_PLAYER) free(entity);
        }else {
            int xo = entity->xo / 16;
            int yo = entity->yo / 16;
            int zo = entity->zo / 16;
            int x = entity->x / 16;
            int y = entity->y / 16;
            int z = entity->z / 16;
            if(xo != x || yo != y || zo != z) {
                entity_map_moved(map, entity);
            }
        }
    }
}

void entity_map_render(entity_map_t *map, vec3_t vector, frustum_t *frustum, textures_t *textures, float delta) {
    for(int x = 0; x < map->width; x++) {
        float x0 = ((x << 4) - 2);
        float x1 = (((x + 1) << 4) + 2);

        for(int z = 0; z < map->depth; z++) {
            float z0 = ((z << 4) - 2);
            float z1 = (((z + 1) << 4) + 2);

            for(int y = 0; y < map->height; y++) {
                float y0 = ((y << 4) - 2);
                float y1 = (((y + 1) << 4) + 2);

                entity_t **list = map->entities[(y * map->depth + z) * map->width + x];
                if(array_list_length(list) != 0) {
                    if(frustum_contains_box(*frustum, x0, y0, z0, x1, y1, z1)) {
                        int index = 0;

                        uint8_t should_render = 1;
                        for(;;) {
                            if(index >= 6) {
                                should_render = 0;
                                break;
                            }

                            if(frustum->planes[index][0] * x0 + frustum->planes[index][1] * y0 + frustum->planes[index][2] * z0 + frustum->planes[index][3] <= 0.0) {
                                should_render = 1;
                                break;
                            }
                            if(frustum->planes[index][0] * x1 + frustum->planes[index][1] * y0 + frustum->planes[index][2] * z0 + frustum->planes[index][3] <= 0.0) {
                                should_render = 1;
                                break;
                            }
                            if(frustum->planes[index][0] * x0 + frustum->planes[index][1] * y1 + frustum->planes[index][2] * z0 + frustum->planes[index][3] <= 0.0) {
                                should_render = 1;
                                break;
                            }
                            if(frustum->planes[index][0] * x1 + frustum->planes[index][1] * y1 + frustum->planes[index][2] * z0 + frustum->planes[index][3] <= 0.0) {
                                should_render = 1;
                                break;
                            }
                            if(frustum->planes[index][0] * x0 + frustum->planes[index][1] * y0 + frustum->planes[index][2] * z1 + frustum->planes[index][3] <= 0.0) {
                                should_render = 1;
                                break;
                            }
                            if(frustum->planes[index][0] * x1 + frustum->planes[index][1] * y0 + frustum->planes[index][2] * z1 + frustum->planes[index][3] <= 0.0) {
                                should_render = 1;
                                break;
                            }
                            if(frustum->planes[index][0] * x0 + frustum->planes[index][1] * y1 + frustum->planes[index][2] * z1 + frustum->planes[index][3] <= 0.0) {
                                should_render = 1;
                                break;
                            }
                            if(frustum->planes[index][0] * x1 + frustum->planes[index][1] * y1 + frustum->planes[index][2] * z1 + frustum->planes[index][3] <= 0.0) {
                                should_render = 1;
                                break;
                            }

                            index++;
                        }

                        for(int i = 0; i < array_list_length(list); i++) {
                            entity_t *entity = list[i];
                            if(entity_should_render(entity, vector)) {
                                if(should_render) {
                                    if(!frustum_contains_box(*frustum, entity->bb.x0, entity->bb.y0, entity->bb.z0, entity->bb.x1, entity->bb.y1, entity->bb.z1)) {
                                        continue;
                                    }
                                }
                                entity->render(entity, textures, delta);
                            }
                        }
                    }
                }
            }
        }
    }
}

void entity_map_destroy(entity_map_t *map) {
    for (int x = 0; x < map->width; x++) {
        for (int z = 0; z < map->depth; z++) {
            for (int y = 0; y < map->height; y++) {
                array_list_free(map->entities[(y * map->depth + z) * map->width + x]);
            }
        }
    }
    free(map->entities);
    array_list_free(map->all);
    array_list_free(map->temp);
}