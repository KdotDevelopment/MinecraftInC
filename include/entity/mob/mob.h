#pragma once

#include <entity/entity.h>
#include <entity/ai/ai_basic.h>
#include <renderer/textures.h>
#include <model/model.h>

#include <stdint.h>
#include <stddef.h>

#define AIR_SUPPLY 300
#define INVULNERABLE_DURATION 20

typedef struct mob_s {
    union {
        entity_t;
        entity_t entity;
    };
    //Models modelCache;
    int invulnerable_duration; // = 20;
    float rot;
    float time_offs;
    float speed;
    float rot_a;
    float y_body_rot; // = 0.0F;
    float y_body_roto; // = 0.0F;
    float o_run;
    float run;
    float anim_step;
    float anim_stepo;
    uint8_t has_hair; // = true;
    char *texture_name; // "char.png"
    uint8_t allow_alpha; // = true;
    float rot_offs; // = 0.0F;
    model_type_t model_type;
    float bob_strength; // = 1.0F;
    int death_score; // = 0;
    float render_offset; // = 0.0F;
    int health;
    int last_health;
    int invulnerable_time; // = 0;
    int air_supply; // = 300;
    int hurt_time;
    int hurt_duration;
    float hurt_dir; // = 0.0F;
    int death_time; // = 0;
    int attack_time; // = 0;
    float o_tilt;
    float tilt;
    uint8_t dead; // = false;
    ai_t ai;

    uint8_t has_armor;
    uint8_t has_helmet;
    uint8_t has_fur; //sheep
    uint8_t is_grazing; //sheep
    int grazing_time; //sheep
    float graze; //sheep
    float graze_o; //sheep

    void (*render_model)(struct mob_s *mob, float time, float r, float bob, float y_rot, float x_rot, float scale);
    void (*die)(struct mob_s *mob, struct mob_s *causer);
    void (*ai_step)(struct mob_s *mob);
} mob_t;

void mob_create(mob_t *mob, struct level_s *level);
void mob_tick(entity_t *entity);
void mob_ai_step(struct mob_s *mob);
void mob_render(entity_t *entity, textures_t *textures, float delta);
void mob_hurt(entity_t *entity, entity_t *entity_causer, int damage);
void mob_knockback(mob_t *mob, mob_t *causer, int damage, float x_diff, float z_diff);
void mob_die(struct mob_s *mob, mob_t *causer);
void mob_cause_fall_damage(entity_t *entity, float distance);
void mob_travel(mob_t *mob, float x, float z);
void mob_heal(struct entity_s *entity, int amount);
void mob_render_model(mob_t *mob, float time, float r, float bob, float y_rot, float x_rot, float scale);
uint8_t mob_can_be_hit(entity_t *entity);