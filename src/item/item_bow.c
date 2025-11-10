#include <item/item_bow.h>

#include <entity/entity_arrow.h>
#include <item/items.h>
#include <sound/sounds.h>

#include <stdlib.h>

item_t item_bow_create(uint8_t id, uint8_t texture_id) {
    item_t item = item_create(id, texture_id);

    item.max_stack_size = 1;

    item.on_right_click = item_bow_on_right_click;

    item_list[id + 256] = item;

    return item;
}

item_stack_t item_bow_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player) {
    if(inventory_player_consume_item(&player->inventory, items.arrow.id)) {
        world_play_sound_at_entity(world, player->entity, SOUND_RANDOM_BOW, 1.0, 1.0 / (random_uniform() * 0.4 + 0.8));
        entity_t *arrow = malloc(sizeof(entity_t));
        entity_arrow_create(arrow, world, world->player, world->player->x, world->player->y, world->player->z, world->player->y_rot, world->player->x_rot, 1.2);
        world_spawn_entity(world, arrow);
    }
    return *item_stack;
}