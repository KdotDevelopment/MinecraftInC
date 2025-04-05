#include <gui/screen_load_level.h>
#include <gui/screen_save_level.h>
#include <minecraft.h>

screen_t screen_load_level_create(screen_t *parent) {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_LOAD_LEVEL;
    screen.parent = (struct screen_s *)parent;
    screen.title = "Load level";
    //screen.grabs_mouse = 1;
    for(int i = 0; i < 5; i++) {
        screen.names[i] = string_create("NOT IMPLEMENTED");
        // more load stuff, not gonna worry about yet
    }

    screen.on_open = screen_load_level_on_open;
    screen.on_button_clicked = screen_load_level_on_button_clicked;
    screen.render = screen_load_level_render;

    return screen;
}

void screen_load_level_on_open(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    for(int i = 0; i < 5; i++) {
        char *name = screen->names[i];
        if(name[0] == '\0') {
            screen->names[i] = string_create("---");
        }
        button_t button_name = button_create(i, screen->width / 2 - 100, screen->height / 6 + i * 24, name);
        screen->buttons = array_list_push(screen->buttons, &button_name);
        screen->buttons[i].active = screen->type != SCREEN_TYPE_LOAD_LEVEL || screen->names[i][0] != '\0';
    }

    button_t button_load = button_create(5, screen->width / 2 - 100, screen->height / 6 + 132, "Load file...");
    button_t button_cancel = button_create(6, screen->width / 2 - 100, screen->height / 6 + 168, "Cancel");
    screen->buttons = array_list_push(screen->buttons, &button_load);
    screen->buttons = array_list_push(screen->buttons, &button_cancel);
    screen->buttons[5].active = 0;
    if(screen->type == SCREEN_TYPE_LOAD_LEVEL) screen_save_level_on_open(proto_screen);
}

void screen_load_level_on_button_clicked(struct screen_s *proto_screen, button_t *button) {
    if(!button->active) return;
    if(button->id < 5) screen_load_level_open_level(proto_screen, button->id);
    else if(button->id == 5) {} //not impl.
    else if(button->id == 6) minecraft_set_current_screen(((screen_t *)proto_screen)->minecraft, (screen_t *)((screen_t *)proto_screen)->parent);
}

void screen_load_level_open_level(struct screen_s *proto_screen, int level) {
    minecraft_set_current_screen(((screen_t *)proto_screen)->minecraft, (screen_t *)((screen_t *)proto_screen)->parent);
    //minecraft_grab_mouse(((screen_t *)proto_screen)->minecraft);
}

void screen_load_level_open_level_from_file(struct screen_s *proto_screen, char *file) {
    minecraft_set_current_screen(((screen_t *)proto_screen)->minecraft, (screen_t *)((screen_t *)proto_screen)->parent);
    //minecraft_grab_mouse(((screen_t *)proto_screen)->minecraft);
}

void screen_load_level_render(struct screen_s *proto_screen, int mouse_x, int mouse_y) {
    screen_t *screen = (screen_t *)proto_screen;
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x05050060, 0x303060A0);
    gui_draw_centered_string(screen->font, screen->title, screen->width / 2, 40, 0xFFFFFFFF);
    screen->type = SCREEN_TYPE_NONE;
    screen_render(proto_screen, mouse_x, mouse_y);
    screen->type = SCREEN_TYPE_LOAD_LEVEL;
}