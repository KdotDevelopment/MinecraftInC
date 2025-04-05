#include <gui/screen_generate_level.h>
#include <minecraft.h>

#include <util/array_list.h>

screen_t screen_generate_level_create(screen_t *parent) {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_GENERATE_LEVEL;
    screen.parent = (struct screen_s *)parent;
    //screen.grabs_mouse = 1;
    
    screen.on_open = screen_generate_level_on_open;
    screen.on_button_clicked = screen_generate_level_on_button_clicked;
    screen.render = screen_generate_level_render;

    return screen;
}

void screen_generate_level_on_open(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    button_t button_small = button_create(0, screen->width / 2 - 100, screen->height / 4, "Small");
    button_t button_normal = button_create(1, screen->width / 2 - 100, screen->height / 4 + 24, "Normal");
    button_t button_large = button_create(2, screen->width / 2 - 100, screen->height / 4 + 48, "Large");
    button_t button_cancel = button_create(200, screen->width / 2 - 100, screen->height / 4 + 120, "Cancel");
    screen->buttons = array_list_push(screen->buttons, &button_small);
    screen->buttons = array_list_push(screen->buttons, &button_normal);
    screen->buttons = array_list_push(screen->buttons, &button_large);
    screen->buttons = array_list_push(screen->buttons, &button_cancel);
}

void screen_generate_level_on_button_clicked(struct screen_s *proto_screen, button_t *button) {
    screen_t *screen = (screen_t *)proto_screen;
    if(button->id == 200) {
        minecraft_set_current_screen(screen->minecraft, (screen_t *)screen->parent);
        return;
    }
    minecraft_regenerate_level(screen->minecraft, button->id);
    minecraft_set_current_screen(screen->minecraft, NULL);
}

void screen_generate_level_render(struct screen_s *proto_screen, int mouse_x, int mouse_y) {
    screen_t *screen = (screen_t *)proto_screen;
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x05050060, 0x303060A0);
    gui_draw_centered_string(screen->font, "Generate new level", screen->width / 2, 40, 0xFFFFFFFF);

    screen_render(proto_screen, mouse_x, mouse_y);
}