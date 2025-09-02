#include <gui/screen_title.h>

#include <gui/screen_options.h>
#include <gui/screen_select_world.h>
#include <minecraft.h>
#include <renderer/tesselator.h>
#include <util/stats.h>
#include <util/time.h>

#include <GL/gl.h>
#include <GL/glu.h>

char *splashes[] = {"Pre-beta!", "As seen on TV!", "Awesome!", "100% pure!", "May contain nuts!", "Better than Prey!", "More polygons!", "Sexy!", "Limited edition!", "Flashing letters!", "Made by Notch!", "Coming soon!", "Best in class!", "When it\'s finished!", "Absolutely dragon free!", "Excitement!", "More than 5000 sold!", "One of a kind!", "700+ hits on YouTube!", "Indev!", "Spiders everywhere!", "Check it out!", "Holy cow, man!", "It\'s a game!", "Made in Sweden!", "Uses LWJGL!", "Reticulating splines!", "Minecraft!", "Yaaay!", "Alpha version!", "Singleplayer!", "Keyboard compatible!", "Undocumented!", "Ingots!", "Exploding creepers!", "That\'s not a moon!", "l33t!", "Create!", "Survive!", "Dungeon!", "Exclusive!", "The bee\'s knees!", "Down with O.P.P.!", "Closed source!", "Classy!", "Wow!", "Not on steam!", "9.95 euro!", "Half price!", "Oh man!", "Check it out!", "Awesome community!", "Pixels!", "Teetsuuuuoooo!", "Kaaneeeedaaaa!", "Now with difficulty!", "Enhanced!", "90% bug free!", "Pretty!", "12 herbs and spices!", "Fat free!", "Absolutely no memes!", "Free dental!", "Ask your doctor!", "Minors welcome!", "Cloud computing!", "Legal in Finland!", "Hard to label!", "Technically good!", "Bringing home the bacon!", "Indie!", "GOTY!", "Ceci n\'est pas une title screen!", "Euclidian!", "Now in 3D!", "Inspirational!", "Herregud!", "Complex cellular automata!", "Yes, sir!", "Played by cowboys!", "OpenGL 1.1!", "Thousands of colors!", "Try it!", "Age of Wonders is better!", "Try the mushroom stew!", "Sensational!", "Hot tamale, hot hot tamale!", "Play him off, keyboard cat!", "Guaranteed!", "Macroscopic!", "Bring it on!", "Random splash!", "Call your mother!", "Monster infighting!", "Loved by millions!", "Ultimate edition!", "Freaky!", "You\'ve got a brand new key!", "Water proof!", "Uninflammable!", "Whoa, dude!", "All inclusive!", "Tell your friends!", "NP is not in P!", "Notch <3 Ez!", "Music by C418!"};

screen_t screen_title_create() {
    screen_t screen = screen_create();

    screen.type = SCREEN_TYPE_TITLE;
    screen.id = rand() % (sizeof(splashes) / sizeof(char *));
    screen.grabs_mouse = 0;

    screen.tick = screen_title_tick;
    screen.on_open = screen_title_on_open;
    screen.on_button_clicked = screen_title_on_button_clicked;
    screen.render = screen_title_render;

    return screen;
}

void screen_title_tick(screen_t *screen) {
    screen->counter_f += 0.01;
}

void screen_title_on_open(screen_t *screen) {
    screen->buttons = array_list_clear(screen->buttons);
    button_t button_singleplayer = button_create(1, screen->width / 2 - 100, screen->height / 4 + 48, "Single player");
    button_t button_multiplayer = button_create(2, screen->width / 2 - 100, screen->height / 4 + 72, "Multi player");
    button_t button_tutorial = button_create(3, screen->width / 2 - 100, screen->height / 4 + 96, "Play tutorial level");
    button_t button_options = button_create(0, screen->width / 2 - 100, screen->height / 4 + 120 + 12, "Options...");
    button_multiplayer.active = 0;
    button_tutorial.active = 0;
    screen->buttons = array_list_push(screen->buttons, &button_singleplayer);
    screen->buttons = array_list_push(screen->buttons, &button_multiplayer);
    screen->buttons = array_list_push(screen->buttons, &button_tutorial);
    screen->buttons = array_list_push(screen->buttons, &button_options);
}

void screen_title_on_button_clicked(screen_t *screen, button_t *button) {
    if(button->id == 0) {
        screen_t *options = malloc(sizeof(screen_t));
        *options = screen_options_create(screen, &screen->minecraft->settings);
        minecraft_set_current_screen(screen->minecraft, (screen_t *)options);
    }
}

void screen_title_render(screen_t *screen, int mouse_x, int mouse_y, float partial_tick) {
    screen_render_background(screen);
    glBindTexture(GL_TEXTURE_2D, textures_load(&((minecraft_t *)screen->minecraft)->textures, "gui/logo.png"));
    glColor4f(1.0, 1.0, 1.0, 1.0);
    tesselator_color_opaque_int(0xffffff);
    gui_blit((screen->width - 256) / 2, 30, 0, 0, 256, 49, 0);
    glPushMatrix();
    glTranslatef((screen->width / 2) + 90, 70, 0);
    glRotatef(-20.0, 0.0, 0.0, 1.0);
    float splash_scale = 1.8 - fabs(tsin((time_millis() % 1000) / 1000.0 * M_PI * 2) * 0.1);
    splash_scale *= 100.0 / (font_get_width(screen->font, splashes[screen->id]) + 32);
    glScalef(splash_scale, splash_scale, splash_scale);
    gui_draw_centered_string(screen->font, splashes[screen->id], 0, -8, 0xFFFF00);
    glPopMatrix();
    char *copyright = "Copyright Mojang Specifications. Do not distribute.";
    gui_draw_string(screen->font, copyright, screen->width - font_get_width(screen->font, copyright) - 2, screen->height - 10, 0xFFFFFF);

    /*long max_mem = stats_get_max_memory();
    long total_mem = stats_get_total_memory();
    long free_mem = stats_get_free_memory();
    long avail_mem = max_mem - free_mem;
    char string[128];
    snprintf(string, sizeof(string),
        "Free memory: %ld%% of %ldMB",
        avail_mem * 100L / max_mem,
        max_mem / 1024L / 1024L
    );
    gui_draw_string(screen->font, string, screen->width - font_get_width(screen->font, string) - 2, 2, 0xFFFFFF);
    memset(string, 0, sizeof(string));
    snprintf(string, sizeof(string),
        "Allocated memory: %ld%% (%ld MB)",
        total_mem * 100L / max_mem,
        total_mem / 1024L / 1024L
    );
    gui_draw_string(screen->font, string, screen->width - font_get_width(screen->font, string) - 2, 12, 0xFFFFFF);*/

    screen_render(screen, mouse_x, mouse_y, partial_tick);
}