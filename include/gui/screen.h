#pragma once

#include <gui/button.h>
#include <gui/container/inventory.h>
#include <gui/container/slot.h>
#include <gui/font.h>
#include <gui/gui.h>
#include <game_settings.h>
#include <world/block/tile_entity/tile_entity.h>

#include <util/string.h>
#include <util/array_list.h>

#include <stdint.h>

#include <SDL2/SDL.h>

struct minecraft_s;

enum {
    SCREEN_TYPE_NONE,
    SCREEN_TYPE_BLOCK_SELECT,
    SCREEN_TYPE_CHAT_INPUT,
    SCREEN_TYPE_CONTAINER,
    SCREEN_TYPE_CONTROLS,
    SCREEN_TYPE_DEATH,
    SCREEN_TYPE_DELETE_WORLD,
    SCREEN_TYPE_ERROR,
    SCREEN_TYPE_GENERATE_WORLD,
    SCREEN_TYPE_WORLD_NAME,
    SCREEN_TYPE_OPTIONS,
    SCREEN_TYPE_PAUSE,
    SCREEN_TYPE_SELECT_WORLD,
    SCREEN_TYPE_TITLE,
    SCREEN_TYPE_YES_NO,
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
    struct screen_s *parent;
    int id;
    int counter;
    float counter_f;
    uint8_t pauses_game;
    slot_t *inventory_slots; //arraylist
    int container_x;
    int container_y;
    item_stack_t held_item;

    union {
        struct {
            tile_entity_t *tile_entity; // furnace, chest
            inventory_t *player_inv; // chest
            inventory_t crafting_inv;
            inventory_t crafting_result;
            int mx;
            int my; // used for inventory player model
            int rows;
        };
        char *name; //world name screen, String
        char *names[5]; //load world screen, String
        struct {
            char message1[128];
            char message2[128];
            int world_id;
        };
    };

    void (*render)(struct screen_s *screen, int x, int y, float partial_tick);
    void (*render_container_foreground)(struct screen_s *screen);
    void (*render_container_background)(struct screen_s *screen);
    void (*on_key_pressed)(struct screen_s *screen, char event_char, int event_key);
    void (*on_mouse_clicked)(struct screen_s *screen, int x, int y, int button);
    void (*on_button_clicked)(struct screen_s *screen, button_t *button);
    void (*on_open)(struct screen_s *screen);
    void (*tick)(struct screen_s *screen);
    void (*update_crafting)(struct screen_s *screen);
    void (*on_close)(struct screen_s *screen);
    void (*destroy)(struct screen_s *screen);
} screen_t;

screen_t screen_create();
void screen_render(struct screen_s *screen, int mouse_x, int mouse_y, float partial_tick);
void screen_render_background(screen_t *screen);
void screen_on_key_pressed(struct screen_s *screen, char event_char, int event_key);
void screen_on_mouse_clicked(struct screen_s *screen, int x, int y, int button);
void screen_on_button_clicked(struct screen_s *screen, button_t *button);
void screen_open(screen_t *screen, struct minecraft_s *minecraft, int width, int height);
void screen_on_open(struct screen_s *screen);
void screen_do_input(screen_t *screen, SDL_Event *events);
void screen_mouse_event(screen_t *screen, SDL_Event event);
void screen_keyboard_event(screen_t *screen, SDL_Event event);
void screen_tick(struct screen_s *screen);
void screen_update_crafting(struct screen_s *screen);
void screen_on_close(struct screen_s *screen);
void screen_destroy(struct screen_s *screen);
void screen_render_container_foreground(screen_t *screen);
void screen_render_container_background(screen_t *screen);