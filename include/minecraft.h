#pragma once

#include <renderer/game_renderer.h>
#include <renderer/level_renderer.h>
#include <renderer/held_block.h>
#include <gui/screen_hud.h>
#include <gui/screen.h>
#include <gui/progress_bar.h>
#include <level/level.h>
#include <gui/font.h>
#include <player/hit_result.h>
#include <particle/particles.h>
#include <sound/sounds.h>
#include <model/models.h>
#include <player/gamemode/gamemode.h>

#include <util/timer.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <stdint.h>

typedef struct minecraft_s {
    game_renderer_t renderer;
    level_t level;
    textures_t textures;
    player_t player;
    game_settings_t settings;
    font_t font;
    level_renderer_t level_renderer;
    game_timer_t timer;
    hit_result_t hit_result;
    screen_t *current_screen;
    progress_bar_t progress_bar;
    particles_t particles;
    gamemode_t gamemode;
    sounds_t sounds;
    models_t models;
    uint8_t fullscreen;
    uint8_t has_mouse;
    char *debug;
    int last_click;
    int32_t width;
    int32_t frame_width;
    int32_t height;
    int32_t frame_height;
    uint8_t running;
    uint64_t ticks;
    uint8_t raining;
    int miss_time;
    screen_hud_t hud;
    SDL_Window *window;
    SDL_GLContext context;
} minecraft_t;

void minecraft_grab_mouse(minecraft_t *minecraft);
void minecraft_set_current_screen(minecraft_t *minecraft, screen_t *screen);
void minecraft_regenerate_level(minecraft_t *minecraft, int size);