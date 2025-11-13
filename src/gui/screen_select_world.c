#include <gui/screen_select_world.h>

#include <gui/screen_options.h>
#include <gui/screen_generate_world.h>
#include <minecraft.h>

screen_t screen_select_world_create(screen_t *parent) {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_SELECT_WORLD;
    screen.parent = parent;
    //screen.grabs_mouse = 1;

    screen.on_open = screen_select_world_on_open;
    screen.on_button_clicked = screen_select_world_on_button_clicked;
    screen.render = screen_select_world_render;

    return screen;
}

void screen_select_world_on_open(screen_t *screen) {
    screen->buttons = array_list_clear(screen->buttons);
    for(int i = 0; i < 5; i++) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "World%d", i + 1);
        nbt_base_t world_nbt = world_get_nbt_tag(screen->minecraft->game_dir, buffer);
        if(world_nbt.null == 1) {
            button_t button = button_create(i, screen->width / 2 - 100, screen->height / 6 + i * 24, "- empty -");
            screen->buttons = array_list_push(screen->buttons, &button);
        }else {
            int64_t size = nbt_tag_compound_get_long(&world_nbt, "SizeOnDisk");
            snprintf(buffer, sizeof(buffer), "World %d (%0.2f MB)", i + 1, (size / 1024.0 * 100.0 / 1024.0) / 100.0);
            button_t button = button_create(i, screen->width / 2 - 100, screen->height / 6 + i * 24, buffer);
            screen->buttons = array_list_push(screen->buttons, &button);
        }
    }
    button_t button_delete = button_create(5, screen->width / 2 - 100, screen->height / 6 + 120 + 12, "Delete world...");
    button_t button_cancel = button_create(6, screen->width / 2 - 100, screen->height / 6 + 168, "Cancel");

    screen->buttons = array_list_push(screen->buttons, &button_delete);
    screen->buttons = array_list_push(screen->buttons, &button_cancel);
}

void screen_select_world_on_button_clicked(screen_t *screen, button_t *button) {
    if(button->active == 0) return;
    if(button->id < 5) {
        screen_select_world_select(screen, button->id + 1);
    }
    if(button->id == 6) {
        minecraft_set_current_screen(screen->minecraft, (screen_t *)screen->parent);
        return;
    }
}

void screen_select_world_render(screen_t *screen, int mouse_x, int mouse_y, float partial_tick) {
    screen_render_background(screen);
    gui_draw_centered_string(screen->font, "Select world", screen->width / 2, 20, 0xFFFFFFFF);

    screen_render(screen, mouse_x, mouse_y, partial_tick);
}

void screen_select_world_select(screen_t *screen, int world_id) {
    char name[24];
    snprintf(name, 23, "World%d", world_id);
    minecraft_start_world(screen->minecraft, name);
    minecraft_set_current_screen(screen->minecraft, NULL);
}