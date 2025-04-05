#include <renderer/game_renderer.h>
#include <level/block/blocks.h>
#include <model/vec3.h>
#include <minecraft.h>

#include <time.h>

game_renderer_t game_renderer_create(minecraft_t *minecraft) {
    game_renderer_t game_renderer = { 0 };
    game_renderer.minecraft = minecraft;
    game_renderer.entity = NULL;
    game_renderer.held_block = (held_block_t){ .minecraft = minecraft };
    game_renderer.random = random_create(time(NULL));
    game_renderer.fog_r = 0.0;
    game_renderer.fog_g = 0.0;
    game_renderer.fog_b = 0.0;
    game_renderer.fog_end = 0.0;
    game_renderer.fog_color_multiplier = 1.0;

    return game_renderer;
}

vec3_t renderer_get_player_vector(game_renderer_t *renderer, float delta) {
    entity_t *player = &renderer->minecraft->player.mob.entity;
    return (vec3_t){ player->xo + (player->x - player->xo) * delta, player->yo + (player->y - player->yo) * delta, player->zo + (player->z - player->zo) * delta };
}

void renderer_apply_bobbing(game_renderer_t *renderer, float delta) {
    entity_t* entity = &renderer->minecraft->player.mob.entity;
    player_t *player = &renderer->minecraft->player;
    float walk = entity->walk_dist - entity->walk_disto;
    walk = entity->walk_dist + walk * delta;
    float bob = player->obob + (player->bob - player->obob) * delta;
    float tilt = player->o_tilt + (player->tilt - player->o_tilt) * delta;
    glTranslatef(tsin(walk * M_PI) * bob * 0.5, -fabs(tcos(walk * M_PI) * bob), 0.0);
    glRotatef(tsin(walk * M_PI) * bob * 3.0, 0.0, 0.0, 1.0);
    glRotatef(fabs(tcos(walk * M_PI + 0.2) * bob) * 5.0, 1.0, 0.0, 0.0);
    glRotatef(tilt, 1.0, 0.0, 0.0);
}

void renderer_set_lighting(game_renderer_t *game_renderer, uint8_t lighting) {
    if(!lighting) {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }else {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        vec3_t pos = vec3_normalize((vec3_t){ 0.0, -1.0, 0.5 });
        glLightfv(GL_LIGHT0, GL_POSITION, (float[]){ pos.x, pos.y, pos.z, 0.0 });
        glLightfv(GL_LIGHT0, GL_DIFFUSE, (float[]){ 0.3, 0.3, 0.3, 1.0 });
        glLightfv(GL_LIGHT0, GL_AMBIENT, (float[]){ 0.0, 0.0, 0.0, 1.0 });
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float[]){ 0.7, 0.7, 0.7, 1.0 });
    }
}

void renderer_setup_gui_camera(game_renderer_t *game_renderer) {
    int width = game_renderer->minecraft->width * 240 / game_renderer->minecraft->height;
    int height = game_renderer->minecraft->height * 240 / game_renderer->minecraft->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)width, (double)height, 0.0, 100.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -200.0);
}

void renderer_setup_fog(game_renderer_t *game_renderer) {
    level_t *level = &game_renderer->minecraft->level;
    player_t *player = &game_renderer->minecraft->player;
    glFogfv(GL_FOG_COLOR, (float []){ game_renderer->fog_r, game_renderer->fog_g, game_renderer->fog_b, 1.0 });
    glNormal3f(0.0, -1.0, 0.0);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    block_t *block = &block_list[level_get_block(level, player->x, player->y + 0.12, player->z)];
    if(block->id != blocks.air.id && block->liquid_type != LIQUID_NONE) {
        uint8_t liquid = block->liquid_type;
        glFogi(GL_FOG_MODE, GL_EXP);
        if(liquid == LIQUID_WATER) {
            glFogf(GL_FOG_DENSITY, 0.1);
            vec3_t a = { 0.4, 0.4, 0.9 };
            if (game_renderer->minecraft->settings.anaglyph) {
                a = (vec3_t){ (a.x * 30.0 + a.y * 59.0 + a.z * 11.0) / 100.0, (a.x * 30.0 + a.y * 70.0) / 100.0, (a.x * 30.0 + a.z * 70.0) / 100.0 };
            }
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float[]){ a.x, a.y, a.z, 1.0 });
        }

        if(liquid == LIQUID_LAVA) {
            glFogf(GL_FOG_DENSITY, 2.0);
            vec3_t a = { 0.4, 0.3, 0.3 };
            if(game_renderer->minecraft->settings.anaglyph) {
                a = (vec3_t){ (a.x * 30.0 + a.y * 59.0 + a.z * 11.0) / 100.0, (a.x * 30.0 + a.y * 70.0) / 100.0, (a.x * 30.0 + a.z * 70.0) / 100.0 };
            }
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float[]){ a.x, a.y, a.z, 1.0 });
        }
    }else {
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 0.0);
        glFogf(GL_FOG_END, game_renderer->fog_end);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float[]){ 1.0, 1.0, 1.0, 1.0 });
    }
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT);
}