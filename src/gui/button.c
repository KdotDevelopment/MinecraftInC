#include <gui/button.h>

#include <util/string.h>

button_t button_create(int id, int x, int y, char *string) {
    button_t button;
    button.active = 1;
    button.visible = 1;
    button.x = x;
    button.y = y;
    button.width = 200;
    button.height = 20;
    button.string = string_create(string);
    button.id = id;

    return button;
}

button_t button_create_size(int id, int x, int y, int width, int height, char *string) {
    button_t button;
    button.active = 1;
    button.visible = 1;
    button.x = x;
    button.y = y;
    button.width = width;
    button.height = height;
    button.string = string_create(string);
    button.id = id;

    return button;
}

void button_destroy(button_t *button) {
    string_free(button->string);
}