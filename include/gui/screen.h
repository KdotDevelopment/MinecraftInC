#pragma once

#include <gui/button.h>
#include <gui/font.h>
#include <gui/gui.h>
#include <game_settings.h>

#include <util/string.h>
#include <util/array_list.h>

#include <stdint.h>

#include <SDL2/SDL.h>

struct minecraft_s;

enum {
    SCREEN_TYPE_NONE,
    SCREEN_TYPE_BLOCK_SELECT,
    SCREEN_TYPE_CHAT_INPUT,
    SCREEN_TYPE_CONTROLS,
    SCREEN_TYPE_DEATH,
    SCREEN_TYPE_ERROR,
    SCREEN_TYPE_GENERATE_LEVEL,
    SCREEN_TYPE_LEVEL_NAME,
    SCREEN_TYPE_LOAD_LEVEL,
    SCREEN_TYPE_OPTIONS,
    SCREEN_TYPE_PAUSE,
    SCREEN_TYPE_SAVE_LEVEL,
};

typedef struct screen_s {
    struct minecraft_s *minecraft;
    int width;
    int height;
    button_t *buttons; //arraylist
    uint8_t grabs_mouse;
    font_t *font;
    uint8_t type;
    uint8_t removed;

    game_settings_t *settings;
    char *title;
    char *text;
    char *name; //level name screen, String
    char *names[5]; //load level screen, String
    struct screen_s *parent;
    int id;
    int counter;

    void (*render)(struct screen_s *screen, int x, int y);
    void (*on_key_pressed)(struct screen_s *screen, char event_char, int event_key);
    void (*on_mouse_clicked)(struct screen_s *screen, int x, int y, int button);
    void (*on_button_clicked)(struct screen_s *screen, button_t *button);
    void (*on_open)(struct screen_s *screen);
    void (*tick)(struct screen_s *screen);
    void (*on_close)(struct screen_s *screen);
    void (*destroy)(struct screen_s *screen);
} screen_t;

screen_t screen_create();
void screen_render(struct screen_s *screen, int mouse_x, int mouse_y);
void screen_on_key_pressed(struct screen_s *screen, char event_char, int event_key);
void screen_on_mouse_clicked(struct screen_s *screen, int x, int y, int button);
void screen_on_button_clicked(struct screen_s *screen, button_t *button);
void screen_open(screen_t *screen, struct minecraft_s *minecraft, int width, int height);
void screen_on_open(struct screen_s *screen);
void screen_do_input(screen_t *screen, SDL_Event *events);
void screen_mouse_event(screen_t *screen, SDL_Event event);
void screen_keyboard_event(screen_t *screen, SDL_Event event);
void screen_tick(struct screen_s *screen);
void screen_on_close(struct screen_s *screen);
void screen_destroy(struct screen_s *screen);