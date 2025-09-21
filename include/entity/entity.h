#pragma once

#include <physics/AABB.h>
#include <model/vec3.h>
#include <renderer/textures.h>
#include <model/model.h>
#include <entity/paintings.h>
#include <nbt/nbt_base.h>
#include <item/item_stack.h>

struct player_s;

typedef enum {
    ENTITY_ARROW,
    ENTITY_ITEM,
    ENTITY_PAINTING,
    ENTITY_PRIMED_TNT,
    ENTITY_ITEM_TAKE, //when a mob picks up an item this is the animation it makes
    ENTITY_ITEM_TAKE_MOCK, //the hidden item entity inside of an item take animation
    ENTITY_MOB_PLAYER,
    ENTITY_MOB_CREEPER,
    ENTITY_MOB_HUMANOID,
    ENTITY_MOB_PIG,
    ENTITY_MOB_SHEEP,
    ENTITY_MOB_SKELETON,
    ENTITY_MOB_SPIDER,
    ENTITY_MOB_ZOMBIE,
} entity_type_t;

typedef struct entity_s {
    struct world_s *world;
    float xo, yo, zo;
    float last_tick_x, last_tick_y, last_tick_z;
    float x, y, z;
    float xd, yd, zd;
    float y_rot, x_rot;
    float y_roto, x_roto;
    AABB_t bb;
    uint8_t on_ground; // = false
    uint8_t horizontal_collision; // = false
    uint8_t collision; // = false
    uint8_t slide; // = true
    uint8_t removed; // = false
    uint8_t is_shootable; // = false
    float height_offset; // = 0.0F
    float bb_width; // = 0.6F
    float bb_height; // = 1.8F
    float walk_disto; // = 0.0F
    float walk_dist; // = 0.0F
    uint8_t make_step_sound; // = true
    float fall_distance; // = 0.0F
    int32_t next_step; // = 1
    uint8_t allowed_in_creative_mode;
    int32_t texture_id; // = 0
    float y_slide_offset; // = 0.0F
    float foot_size; // = 0.0F
    uint8_t no_physics; // = false
    float push_through; // = 0.0F
    uint8_t hovered; // = false
    int age;
    int tick_count; // = 0;
    entity_type_t type;
    int time;
    uint8_t is_dead;
    uint8_t prevents_spawning;

    union {
        struct { // arrow
            struct entity_s *owner;
            int damage;
            int arrow_type;
            float gravity;
            uint8_t has_hit;
        };
        struct { // item
            struct entity_s *item;
            item_stack_t item_stack;
            uint8_t delay_before_pickup;
        };
        struct { // painting
            painting_t *painting;
            int painting_pos_x;
            int painting_pos_y;
            int painting_pos_z;
            uint8_t painting_direction;
            int painting_ticks;
        };
    };
    
    model_t *model;

    void (*tick)(struct entity_s *entity);
    void (*render)(struct entity_s *entity, textures_t *textures, float delta);
    void (*hurt)(struct entity_s *entity, struct entity_s *causer, int damage);
    void (*heal)(struct entity_s *entity, int damage);
    void (*cause_fall_damage)(struct entity_s *entity, float distance);
    void (*award_kill_score)(struct entity_s *entity, struct entity_s *causer, int score);
    float (*get_brightness)(struct entity_s *entity, float tick);
    void (*player_touch)(struct entity_s *entity, struct entity_s *player);
    uint8_t (*can_be_hit)(struct entity_s *entity);
    void (*write_nbt)(struct entity_s *entity, nbt_base_t *nbt);
    void (*read_nbt)(struct entity_s *entity, nbt_base_t *nbt);
} entity_t;

void entity_create(entity_t *entity, struct world_s *world);
void entity_set_pos(entity_t *entity, float x, float y, float z);
void entity_reset_pos(entity_t *entity);
void entity_turn(entity_t *entity, float y_rot, float x_rot);
void entity_interpolate_turn(entity_t *entity, float y_rot, float x_rot);
void entity_tick(entity_t *entity);
void entity_play_sound(entity_t *entity, uint8_t sound, float volume, float pitch);
void entity_move(entity_t *entity, float x, float y, float z);
void entity_move_to(entity_t *entity, float x, float y, float z, float x_rot, float y_rot);
void entity_move_relative(entity_t *entity, float x, float y, float z);
float entity_distance_to(entity_t *a, entity_t *b);
float entity_distance_to_sqr(entity_t *a, entity_t *b);
void entity_push_dist(entity_t *entity, float x, float y, float z);
void entity_push(entity_t *a, entity_t *b);
uint8_t should_render_at_sqr_distance(entity_t *entity, float distance);
uint8_t entity_should_render(entity_t *entity, vec3_t vec);
void entity_remove(entity_t *entity);
uint8_t entity_is_free(entity_t *entity, float x, float y, float z);
uint8_t entity_is_underwater(entity_t *entity);
uint8_t entity_is_in_water(entity_t *entity);
uint8_t entity_is_in_lava(entity_t *entity);
uint8_t entity_on_ground(entity_t *entity);
float entity_get_brightness(entity_t *entity, float tick);
void entity_render(entity_t *entity, textures_t *textures, float delta);
void entity_hurt(entity_t *entity, entity_t *causer, int damage);
void entity_heal(struct entity_s *entity, int damage);
void entity_cause_fall_damage(entity_t *entity, float distance);
void entity_award_kill_score(entity_t *entity, entity_t *causer, int score);
void entity_player_touch(entity_t *entity, entity_t *player);
uint8_t entity_can_be_hit(entity_t *entity);
void entity_read_nbt(entity_t *entity, nbt_base_t *nbt);
void entity_write_nbt(entity_t *entity, nbt_base_t *nbt);