#include <game_settings.h>
#include <minecraft.h>
#include <renderer/textures.h>
#include <sound/sounds.h>

#include <util/array_list.h>
#include <util/string.h>

#include <SDL2/SDL.h>

void game_settings_create(game_settings_t *settings, struct minecraft_s *minecraft) {
    *settings = (game_settings_t){ 0 };
    settings->music = 1;
    settings->sound = 1;
    settings->invert_mouse = 0;
    settings->show_framerate = 0;
    settings->view_distance = 1;
    settings->show_bobbing = 1;
    settings->anaglyph = 0;
    settings->limit_framerate = 0;
    settings->settings_count = 8;
    settings->difficulty = 2;
    settings->minecraft = minecraft;

    settings->forward_key = (key_binding_t){ .name = "Forward", .key = SDL_SCANCODE_W };
    settings->left_key = (key_binding_t){ .name = "Left", .key = SDL_SCANCODE_A };
    settings->back_key = (key_binding_t){ .name = "Back", .key = SDL_SCANCODE_S };
    settings->right_key = (key_binding_t){ .name = "Right", .key = SDL_SCANCODE_D };
    settings->jump_key = (key_binding_t){ .name = "Jump", .key = SDL_SCANCODE_SPACE };
    settings->drop_key = (key_binding_t){ .name = "Drop", .key = SDL_SCANCODE_Q };
    settings->inventory_key = (key_binding_t){ .name = "Inventory", .key = SDL_SCANCODE_I };
    settings->chat_key = (key_binding_t){ .name = "Chat", .key = SDL_SCANCODE_T };
    settings->toggle_fog_key = (key_binding_t){ .name = "Toggle Fog", .key = SDL_SCANCODE_F };
    settings->save_location_key = (key_binding_t){ .name = "Save Location", .key = SDL_SCANCODE_RETURN };
    settings->load_location_key = (key_binding_t){ .name = "Load Location", .key = SDL_SCANCODE_R };
    settings->bindings = array_list_create(sizeof(key_binding_t *));

    key_binding_t *bindings[] = { &settings->forward_key, &settings->left_key, &settings->back_key, &settings->right_key, &settings->jump_key, &settings->drop_key, &settings->inventory_key, &settings->chat_key, &settings->toggle_fog_key, &settings->save_location_key, &settings->load_location_key };
    for(int i = 0; i < sizeof(bindings) / sizeof(bindings[0]); i++) {
        settings->bindings = array_list_push(settings->bindings, &bindings[i]);
    }

    game_settings_load(settings);
}

void private_upper_case(char *str) {
    for(int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

char *game_settings_get_binding(game_settings_t *settings, int binding) {
    char *string = string_create(settings->bindings[binding]->name);
    string_concat(&string, ": ");
    const char *key_name = (char *)SDL_GetKeyName(SDL_SCANCODE_TO_KEYCODE(settings->bindings[binding]->key));
    char *key_upper = string_create(key_name);
    private_upper_case(key_upper);
    string_concat(&string, key_upper);
    free(key_upper);
    return string;
}

void game_settings_set_binding(game_settings_t *settings, int binding, int key) {
    settings->bindings[binding]->key = key;
    game_settings_save(settings);
}

void game_settings_toggle_setting(game_settings_t *settings, int setting) {
    switch(setting) {
        case 0:
            settings->music = !settings->music;
            sounds_set_music_volume(&settings->minecraft->sounds, settings->music);
            break;
        case 1:
            settings->sound = !settings->sound;
            sounds_set_sound_volume(&settings->minecraft->sounds, settings->sound);
            break;
        case 2:
            settings->invert_mouse = !settings->invert_mouse;
            break;
        case 3:
            settings->show_framerate = !settings->show_framerate;
            break;
        case 4:
            settings->view_distance = (settings->view_distance + 1) % 4;
            break;
        case 5:
            settings->show_bobbing = !settings->show_bobbing;
            break;
        case 6:
            settings->anaglyph = !settings->anaglyph;
            minecraft_t *minecraft = settings->minecraft;
            textures_reload(&minecraft->textures);
            settings->minecraft->font.texture = textures_load(&settings->minecraft->textures, settings->minecraft->font.texture_name);
            break;
        case 7:
            settings->limit_framerate = !settings->limit_framerate;
            SDL_GL_SetSwapInterval(settings->limit_framerate ? 1 : 0);
            break;
        case 8:
            settings->difficulty = (settings->difficulty + 1) % 4;
            break;
    }
    game_settings_save(settings);
}

char *render_distances[] = { "FAR", "NORMAL", "SHORT", "TINY" };
char *difficulties[] = { "Peaceful", "Easy", "Normal", "Hard" };

char *game_settings_get_setting(game_settings_t *settings, int setting) {
    char *string;
    switch(setting) {
        case 0:
            string = string_create("Music: ");
            string_concat(&string, settings->music ? "ON" : "OFF");
            break;
        case 1:
            string = string_create("Sound: ");
            string_concat(&string, settings->sound ? "ON" : "OFF");
            break;
        case 2:
            string = string_create("Invert mouse: ");
            string_concat(&string, settings->invert_mouse ? "ON" : "OFF");
            break;
        case 3:
            string = string_create("Show FPS: ");
            string_concat(&string, settings->show_framerate ? "ON" : "OFF");
            break;
        case 4:
            string = string_create("Render distance: ");
            string_concat(&string, render_distances[settings->view_distance]);
            break;
        case 5:
            string = string_create("View bobbing: ");
            string_concat(&string, settings->show_bobbing ? "ON" : "OFF");
            break;
        case 6:
            string = string_create("3d anaglyph: ");
            string_concat(&string, settings->anaglyph ? "ON" : "OFF");
            break;
        case 7:
            string = string_create("Limit framerate: ");
            string_concat(&string, settings->limit_framerate ? "ON" : "OFF");
            break;
        case 8:
            string = string_create("Difficulty: ");
            string_concat(&string, difficulties[settings->difficulty]);
            break;
        default:
            string = string_create("Error");
            break;
    }
    return string;
}

void game_settings_load(game_settings_t *settings) {
    FILE *file = fopen("./.minecraft/options.txt", "r");
    if(!file) {
        return;
    }

    char line[256];
    while(fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        char *colon = strchr(line, ':');
        if(!colon) continue;
        
        *colon = '\0';
        char *key = line;
        char *value = colon + 1;
        
        if(strcmp(key, "music") == 0) {
            settings->music = strcmp(value, "true") == 0;
            sounds_set_music_volume(&settings->minecraft->sounds, settings->music);
        }else if(strcmp(key, "sound") == 0) {
            settings->sound = strcmp(value, "true") == 0;
            sounds_set_sound_volume(&settings->minecraft->sounds, settings->sound);
        }else if(strcmp(key, "invertYMouse") == 0) {
            settings->invert_mouse = strcmp(value, "true") == 0;
        }else if(strcmp(key, "showFrameRate") == 0) {
            settings->show_framerate = strcmp(value, "true") == 0;
        }else if(strcmp(key, "viewDistance") == 0) {
            settings->view_distance = atoi(value);
        }else if(strcmp(key, "bobView") == 0) {
            settings->show_bobbing = strcmp(value, "true") == 0;
        }else if(strcmp(key, "anaglyph3d") == 0) {
            settings->anaglyph = strcmp(value, "true") == 0;
        }else if(strcmp(key, "limitFramerate") == 0) {
            settings->limit_framerate = strcmp(value, "true") == 0;
            SDL_GL_SetSwapInterval(settings->limit_framerate ? 1 : 0);
        }else if(strcmp(key, "difficulty") == 0) {
            settings->difficulty = atoi(value);
        }else {
            if(strncmp(key, "key_", 4) == 0) {
                char *key_name = key + 4;
                int key_code = atoi(value);
                
                for(int i = 0; i < array_list_length(settings->bindings); i++) {
                    if(strcmp(settings->bindings[i]->name, key_name) == 0) {
                        settings->bindings[i]->key = key_code;
                        break;
                    }
                }
            }
        }
    }
    
    fclose(file);
}

void game_settings_save(game_settings_t *settings) {
    FILE *file = fopen("./.minecraft/options.txt", "w");
    if(!file) {
        printf("Failed to save options\n");
        return;
    }
    
    fprintf(file, "music:%s\n", settings->music ? "true" : "false");
    fprintf(file, "sound:%s\n", settings->sound ? "true" : "false");
    fprintf(file, "invertYMouse:%s\n", settings->invert_mouse ? "true" : "false");
    fprintf(file, "showFrameRate:%s\n", settings->show_framerate ? "true" : "false");
    fprintf(file, "viewDistance:%d\n", settings->view_distance);
    fprintf(file, "bobView:%s\n", settings->show_bobbing ? "true" : "false");
    fprintf(file, "anaglyph3d:%s\n", settings->anaglyph ? "true" : "false");
    fprintf(file, "limitFramerate:%s\n", settings->limit_framerate ? "true" : "false");
    fprintf(file, "difficulty:%d\n", settings->difficulty);
    
    for(int i = 0; i < array_list_length(settings->bindings); i++) {
        fprintf(file, "key_%s:%d\n", settings->bindings[i]->name, settings->bindings[i]->key);
    }
    
    fclose(file);
}