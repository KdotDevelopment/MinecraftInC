#include <gui/screen_yes_no.h>

#include <gui/screen_delete_world.h>

screen_t screen_yes_no_create(screen_t *parent, char *message1, char *message2, int world_id) {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_YES_NO;
    screen.parent = parent;
    strncpy(screen.message1, message1, 127);
    strncpy(screen.message2, message2, 127);
    screen.world_id = world_id;

    screen.on_open = screen_yes_no_on_open;
    screen.on_button_clicked = screen_yes_no_on_button_clicked;
    screen.render = screen_yes_no_render;

    return screen;
}

void screen_yes_no_on_open(screen_t *screen) {
    screen->buttons = array_list_clear(screen->buttons);
    button_t button_yes = button_create_size(0, screen->width / 2 - 155, screen->height / 6 + 96, 150, 20, "Yes");
    button_t button_no = button_create_size(1, screen->width / 2 - 155 + 160, screen->height / 6 + 96, 150, 20, "No");

    screen->buttons = array_list_push(screen->buttons, &button_yes);
    screen->buttons = array_list_push(screen->buttons, &button_no);
}

void screen_yes_no_on_button_clicked(screen_t *screen, button_t *button) {
    screen_delete_world_delete(screen->parent, button->id == 0, screen->world_id);
}

void screen_yes_no_render(screen_t *screen, int mouse_x, int mouse_y, float partial_tick) {
    screen_render_background(screen);
    gui_draw_centered_string(screen->font, screen->message1, screen->width / 2, 70, 0xFFFFFFFF);
    gui_draw_centered_string(screen->font, screen->message2, screen->width / 2, 90, 0xFFFFFFFF);

    screen_render(screen, mouse_x, mouse_y, partial_tick);
}