#pragma once

#include <util/random.h>

struct ai_s;
struct player_s;
struct level_s;

typedef struct ai_s {
    //BASIC AI
    random_t *random;
    float xa;
    float za;
    float y_rota;
    int default_look_angle;
    struct level_s *level;
    struct mob_s *mob;
    uint8_t jumping; // = 0
    int attack_delay; // = 0
    float run_speed; // = 0.7
    float no_action_time; // = 0
    int damage;
    struct entity_s *attack_target;

    //PLAYER AI
    struct player_s *player;

    void (*tick)(struct ai_s *ai);
    void (*update)(struct ai_s *ai);
    void (*before_removal)(struct ai_s *ai);
    void (*hurt)(struct ai_s *ai, struct entity_s *entity, int damage);
    void (*jump)(struct ai_s *ai);
    uint8_t (*attack)(struct ai_s *ai);
} ai_t;

ai_t ai_create();
void ai_tick(struct ai_s *ai);
void ai_before_removal(struct ai_s *ai);
void ai_hurt(struct ai_s *ai, struct entity_s *entity, int damage);
void ai_jump(struct ai_s *ai);
uint8_t ai_attack(struct ai_s *ai);