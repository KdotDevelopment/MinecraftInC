#include <gui/screen_delete_world.h>

#include <gui/screen_select_world.h>
#include <gui/screen_yes_no.h>
#include <gui/screen_options.h>
#include <gui/screen_generate_world.h>
#include <minecraft.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

void delete_files_recursive(const char *path) {
#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    char search_path[512];
    snprintf(search_path, sizeof(search_path), "%s/*", path);
    
    HANDLE handle = FindFirstFile(search_path, &find_data);
    if (handle == INVALID_HANDLE_VALUE) {
        return;
    }
    
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s", path, find_data.cFileName);
        
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            delete_files_recursive(file_path);
            RemoveDirectory(file_path);
        } else {
            DeleteFile(file_path);
        }
    } while (FindNextFile(handle, &find_data));
    
    FindClose(handle);
#else
    DIR *dir = opendir(path);
    if (!dir) {
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);
        
        struct stat st;
        if (lstat(file_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                delete_files_recursive(file_path);
            }
            remove(file_path);
        }
    }
    
    closedir(dir);
#endif
}

void world_delete(const char *game_dir, const char *world_name) {
    char world_path[512];
    snprintf(world_path, sizeof(world_path), "%s/saves/%s", game_dir, world_name);
    
    delete_files_recursive(world_path);
    
#ifdef _WIN32
    RemoveDirectory(world_path);
#else
    remove(world_path);
#endif
}

screen_t screen_delete_world_create(screen_t *parent) {
    screen_t screen = screen_select_world_create(parent);
    screen.type = SCREEN_TYPE_DELETE_WORLD;
    screen.parent = parent;

    screen.on_open = screen_delete_world_on_open;
    screen.on_button_clicked = screen_delete_world_on_button_clicked;
    screen.render = screen_delete_world_render;

    return screen;
}

void screen_delete_world_on_open(screen_t *screen) {
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
    button_t button_cancel = button_create(6, screen->width / 2 - 100, screen->height / 6 + 168, "Cancel");
    screen->buttons = array_list_push(screen->buttons, &button_cancel);
}

void screen_delete_world_select(screen_t *screen, int world_id) {
    char name[24];
    snprintf(name, 23, "World%d", world_id);
    if(!world_get_nbt_tag("./.minecraft", name).null) {
        char message[128];
        snprintf(message, 126, "\'%s\' will be lost forever!", name);
        screen_t *yes_no = malloc(sizeof(screen_t));
        *yes_no = screen_yes_no_create(screen, "Are you sure you want to delete this world?", message, world_id);
        minecraft_set_current_screen(screen->minecraft, yes_no);
    }
}

void screen_delete_world_on_button_clicked(screen_t *screen, button_t *button) {
    if(button->active == 0) return;
    if(button->id < 5) {
        screen_delete_world_select(screen, button->id + 1);
    }
    if(button->id == 5) {
        screen_t *delete_world = malloc(sizeof(screen_t));
        *delete_world = screen_delete_world_create(screen);
        minecraft_set_current_screen(screen->minecraft, delete_world);
    }
    if(button->id == 6) {
        minecraft_set_current_screen(screen->minecraft, (screen_t *)screen->parent);
        return;
    }
}

void screen_delete_world_render(screen_t *screen, int mouse_x, int mouse_y, float partial_tick) {
    screen_render_background(screen);
    gui_draw_centered_string(screen->font, "Delete world", screen->width / 2, 20, 0xFFFFFFFF);

    screen_render(screen, mouse_x, mouse_y, partial_tick);
}

void screen_delete_world_delete(screen_t *screen, uint8_t perform, int world_id) {
    if(perform) {
        char world_name[24];
        snprintf(world_name, sizeof(world_name), "World%d", world_id);
        world_delete(screen->minecraft->game_dir, world_name);
    }

    minecraft_set_current_screen(screen->minecraft, screen->parent);
}