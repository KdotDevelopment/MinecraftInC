#include <minecraft.h>

#include <session_data.h>
#include <renderer/renderer_camera.h>
#include <renderer/tesselator.h>
#include <renderer/frustum.h>
#include <renderer/texture/texture_fire.h>
#include <renderer/texture/texture_gears.h>
#include <renderer/texture/texture_lava.h>
#include <renderer/texture/texture_water.h>
#include <renderer/texture/texture_water_flow.h>
#include <world/block/blocks.h>
#include <world/world.h>
#include <gui/screen_pause.h>
#include <gui/screen_block_select.h>
#include <gui/screen_death.h>
#include <gui/screen_title.h>
#include <gui/container/screen_inventory.h>
#include <particle/particle_water_drop.h>
#include <model/models.h>
#include <player/gamemode/gamemode_creative.h>
#include <player/gamemode/gamemode_survival.h>
#include <entity/entity_item.h>
#include <entity/entity_arrow.h>
#include <item/items.h>

#include <util/time.h>
#include <util/array_list.h>
#include <util/math_helper.h>

#include <GL/gl.h>
#include <GL/glu.h>

//gets rid of annoying warning for windows static linking
extern void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

#include <stdio.h>

void minecraft_create(minecraft_t *minecraft, uint16_t width, uint16_t height, uint8_t fullscreen) {
    minecraft->fullscreen = fullscreen;
    minecraft->width = width;
    minecraft->height = height;
    minecraft->running = 0;
    minecraft->ticks = 0;
    minecraft->has_mouse = 0;
    minecraft->hit_result = (hit_result_t){.null = 1};
    minecraft->last_click = 0;
    minecraft->raining = 0;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Couldn't init SDL2: %s\n", SDL_GetError());
        exit(1);
    }
    int window_width = width;
    int window_height = height;
    SDL_Rect usable_bounds;
    if(SDL_GetDisplayUsableBounds(0, &usable_bounds) == 0) {
        int max_w = usable_bounds.w;
        int max_h = usable_bounds.h;
        if(window_width > max_w || window_height > max_h) {
            float scale_w = (float)max_w / (float)window_width;
            float scale_h = (float)max_h / (float)window_height;
            float scale = scale_w < scale_h ? scale_w : scale_h;
            window_width = (int)((float)window_width * scale);
            window_height = (int)((float)window_height * scale);
        }
        if(window_width >= max_w) window_width = max_w - SDL_max(64, max_w / 10);
        if(window_height >= max_h) window_height = max_h - SDL_max(64, max_h / 10);
    }else {
        SDL_DisplayMode desktop_mode;
        if(SDL_GetDesktopDisplayMode(0, &desktop_mode) == 0) {
            int max_w = desktop_mode.w;
            int max_h = desktop_mode.h;
            if(window_width > max_w || window_height > max_h) {
                float scale_w = (float)max_w / (float)window_width;
                float scale_h = (float)max_h / (float)window_height;
                float scale = scale_w < scale_h ? scale_w : scale_h;
                window_width = (int)((float)window_width * scale);
                window_height = (int)((float)window_height * scale);
            }
            if(window_width >= max_w) window_width = max_w - SDL_max(64, max_w / 10);
            if(window_height >= max_h) window_height = max_h - SDL_max(64, max_h / 10);
        }
    }
    if(window_width < 640) window_width = 640;
    if(window_height < 480) window_height = 480;

    SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
    if(fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    minecraft->window = SDL_CreateWindow("Minecraft Infdev", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, flags);
    if(!minecraft->window) {
        printf("Couldn't create window: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_GetWindowSize(minecraft->window, &minecraft->width, &minecraft->height);
    SDL_GL_GetDrawableSize(minecraft->window, &minecraft->frame_width, &minecraft->frame_height);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    minecraft->context = SDL_GL_CreateContext(minecraft->window);
    if(!minecraft->context) {
        printf("Couldn't create window context: %s\n", SDL_GetError());
        exit(1);
    }

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
    glCullFace(GL_BACK);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    math_helper_initialize();
    materials_init();
    items_init();
    blocks_init();
    session_data_initialize();
    item_models_init();
    tesselator_create(&g_tesselator);
    minecraft->debug = string_create("");
    minecraft->sounds = sounds_create();
    minecraft->timer = timer_create(20);
    minecraft->progress_bar = progress_bar_create(minecraft);
    minecraft->models = models_create();
    minecraft->game_dir = string_create("./.minecraft");
    #ifdef SURVIVAL_MODE
    minecraft->gamemode = gamemode_survival_create(minecraft);
    #else
    minecraft->gamemode = gamemode_creative_create(minecraft);
    #endif
    renderer_entity_manager_create(&minecraft->entity_manager, minecraft);

    game_settings_create(&minecraft->settings, (struct minecraft_s *)minecraft);
    SDL_GL_SetSwapInterval(minecraft->settings.limit_framerate ? 1 : 0);
    minecraft->textures = textures_create(&minecraft->settings);

    texture_animated_t *lava_texture = malloc(sizeof(texture_animated_t));
    *lava_texture = texture_lava_create();
    textures_register_animation(&minecraft->textures, lava_texture);
    texture_animated_t *water_texture = malloc(sizeof(texture_animated_t));
    *water_texture = texture_water_create();
    textures_register_animation(&minecraft->textures, water_texture);
    texture_animated_t *flowing_texture = malloc(sizeof(texture_animated_t));
    *flowing_texture = texture_water_flow_create();
    textures_register_animation(&minecraft->textures, flowing_texture);
    texture_animated_t *fire_texture = malloc(sizeof(texture_animated_t));
    *fire_texture = texture_fire_create(0);
    textures_register_animation(&minecraft->textures, fire_texture);
    texture_animated_t *fire1_texture = malloc(sizeof(texture_animated_t));
    *fire1_texture = texture_fire_create(1);
    textures_register_animation(&minecraft->textures, fire1_texture);
    texture_animated_t *gears_texture = malloc(sizeof(texture_animated_t));
    *gears_texture = texture_gears_create(0);
    textures_register_animation(&minecraft->textures, gears_texture);
    texture_animated_t *gears1_texture = malloc(sizeof(texture_animated_t));
    *gears1_texture = texture_gears_create(1);
    textures_register_animation(&minecraft->textures, gears1_texture);

    minecraft->font = font_create(&minecraft->settings, "default.png", &minecraft->textures);

    glViewport(0, 0, minecraft->frame_width, minecraft->frame_height);

    minecraft->world = malloc(sizeof(world_t));
    world_create(minecraft->world, minecraft, "./.minecraft/saves", "World1", time(NULL));
    /*for(int i = -196; i <= 196; i += 16) {
        for(int j = -196; j <= 196; j += 16) {
            world_get_block(minecraft->world, i + minecraft->world->spawn_x, 64, j + minecraft->world->spawn_z);
        }
    }*/
    world_save(minecraft->world, 1);
    minecraft->renderer = renderer_camera_create(minecraft);
    player_create(&minecraft->player, (struct world_s *)minecraft->world);
    printf("Player created at %f, %f, %f\n", minecraft->player.x, minecraft->player.y, minecraft->player.z);
    minecraft->player.x = 0;
    minecraft->player.y = 64;
    minecraft->player.z = 0;
    minecraft->world->player = &minecraft->player;
    printf("Preparing lights\n");
    while(array_list_length(minecraft->world->lighting_update_list) > 0) {
        world_update_lighting(minecraft->world);
    }
    while(world_update_lighting(minecraft->world));
    minecraft->player.mob->player->inputs = inputs_create(&minecraft->settings);
    //minecraft->gamemode.init_player(&minecraft->gamemode, &minecraft->player);
    //minecraft->gamemode.adjust_player(&minecraft->gamemode, &minecraft->player);
    minecraft->player.mob->player->inventory.inv[8] = item_stack_create(BLOCK_TORCH, 64, 0);
    minecraft->player.mob->player->inventory.inv[0] = item_stack_create(items.iron_pickaxe.item_id, 1, 0);
    minecraft->player.mob->player->inventory.inv[1] = item_stack_create(items.bow.item_id, 1, 0);
    minecraft->player.mob->player->inventory.inv[2] = item_stack_create(items.iron_axe.item_id, 1, 0);
    minecraft->player.mob->player->inventory.inv[3] = item_stack_create(items.iron_shovel.item_id, 1, 0);
    minecraft->player.mob->player->inventory.inv[10] = item_stack_create(items.arrow.item_id, 64, 0);
    minecraft->player.mob->player->inventory.inv[11] = item_stack_create(items.flint_and_steel.item_id, 1, 0);
    minecraft->player.mob->player->inventory.inv[12] = item_stack_create(items.cooked_pork.item_id, 20, 0);
    minecraft->player.mob->player->inventory.armor[3] = item_stack_create(items.gold_helmet.item_id, 1, 0);
    minecraft->player.mob->player->inventory.armor[2] = item_stack_create(items.gold_chestplate.item_id, 1, 0);
    minecraft->player.mob->player->inventory.armor[1] = item_stack_create(items.gold_leggings.item_id, 1, 0);
    minecraft->player.mob->player->inventory.armor[0] = item_stack_create(items.gold_boots.item_id, 1, 0);

    renderer_world_create(&minecraft->renderer_world, minecraft, minecraft->world, &minecraft->textures);
    renderer_world_change_world(&minecraft->renderer_world, minecraft->world);
    //minecraft->world->renderer = (struct renderer_world_s *)&minecraft->renderer_world;
    //renderer_world_refresh((renderer_world_t *)&minecraft->renderer_world);
    minecraft->particles = particles_create(minecraft->world, &minecraft->textures);
    minecraft->world->particles = &minecraft->particles;

    int x = minecraft->width;
    int y = minecraft->height;
    int w = x;
    int h = y;
    for(x = 1; w / (x + 1) >= 320 && h / (x + 1) >= 240; x++);
    w /= x;
    h /= x;
    minecraft->hud = screen_hud_create((struct minecraft_s *)minecraft, w, h);
    //minecraft_grab_mouse(minecraft);
    if(minecraft->world != NULL) {
        screen_t *main_menu = malloc(sizeof(screen_t));
        *main_menu = screen_title_create();
        minecraft_set_current_screen(minecraft, main_menu);
    }
}

void minecraft_grab_mouse(minecraft_t *minecraft) {
    if(!minecraft->has_mouse) {
        minecraft->has_mouse = 1;
        SDL_ShowCursor(0);
        SDL_WarpMouseInWindow(minecraft->window, minecraft->width / 2, minecraft->height / 2);
        minecraft->last_click = minecraft->ticks + 10000;
    }
}

void minecraft_set_current_screen(minecraft_t *minecraft, screen_t *screen) {
    if(minecraft->current_screen != NULL && minecraft->current_screen->type == SCREEN_TYPE_ERROR) {
        return;
    }
    if(minecraft->current_screen != NULL) {
        //minecraft->current_screen->on_close((struct screen_s *)minecraft->current_screen);
    }
    if(screen == NULL && minecraft->world == NULL) {
        screen = malloc(sizeof(screen_t));
        *screen = screen_title_create();
    }else if(screen == NULL && minecraft->player.mob->health <= 0) {
        screen = malloc(sizeof(screen_t));
        *screen = screen_death_create();
    }
    if(screen != NULL) {
        minecraft->current_screen = screen;
        if(minecraft->has_mouse) {
            inputs_reset_keys(&minecraft->player.mob->player->inputs);
            minecraft->has_mouse = 0;
            SDL_ShowCursor(1);
        }
        int x = minecraft->width;
        int y = minecraft->height;
        int w = x;
        int h = y;
        for(x = 1; w / (x + 1) >= 320 && h / (x + 1) >= 240; x++);
        w /= x;
        h /= x;

        screen_open(screen, minecraft, w, h);
    }else {
        minecraft->current_screen = NULL;
        minecraft_grab_mouse(minecraft);
    }
}

void minecraft_regenerate_world(minecraft_t *minecraft, int size) {
    //world_regenerate(minecraft->world, size);
    minecraft->gamemode.init_world(&minecraft->gamemode, minecraft->world);
    player_create(&minecraft->player, (struct world_s *)minecraft->world);
    entity_reset_pos(&minecraft->player);
    minecraft->gamemode.init_player(&minecraft->gamemode, &minecraft->player);
    minecraft->gamemode.adjust_player(&minecraft->gamemode, &minecraft->player);
    minecraft->player.mob->player->inputs = inputs_create(&minecraft->settings);
    renderer_world_destroy(&minecraft->renderer_world);
    //minecraft->renderer_world = renderer_world_create(minecraft, minecraft->world, &minecraft->textures);
    //minecraft->world->renderer = (struct renderer_world_s *)&minecraft->renderer_world;
    //renderer_world_refresh((renderer_world_t *)&minecraft->renderer_world);
    particles_destroy(&minecraft->particles);
    minecraft->particles = particles_create(minecraft->world, &minecraft->textures);
}

void minecraft_pause(minecraft_t *minecraft) {
    if(minecraft->current_screen == NULL) {
        screen_t *screen = malloc(sizeof(screen_t));
        *screen = screen_pause_create((struct minecraft_s *)minecraft);
        minecraft_set_current_screen(minecraft, screen);
    }
}

void on_mouse_clicked(minecraft_t *minecraft, int button) {
    if(button != SDL_BUTTON_LEFT || minecraft->miss_time <= 0) {
        if(button == SDL_BUTTON_LEFT) {
            minecraft->renderer.renderer_overlay.swing_time = -1;
            minecraft->renderer.renderer_overlay.is_swinging = 1;
        }

        if(button == SDL_BUTTON_RIGHT) {
            item_stack_t current_item = inventory_player_get_selected(&minecraft->player.mob->player->inventory);
            if(current_item.item_id != 0) {
                int size = current_item.stack_size;
                player_t *player = minecraft->player.mob->player;
                item_stack_t result_item = item_list[current_item.item_id].on_right_click(&current_item, minecraft->world, player);
                if(result_item.stack_size != size || result_item.item_damage != current_item.item_damage || result_item.item_id != current_item.item_id) {
                    inventory_player_set_slot(&player->inventory, player->inventory.selected, result_item);
                    minecraft->renderer.renderer_overlay.equipped_progress = 0;
                    if(result_item.stack_size == 0) {
                        item_stack_t null_item = item_stack_create(0, 0, 0);
                        inventory_player_set_slot(&player->inventory, player->inventory.selected, null_item);
                    }
                }
            }
        }

        if(minecraft->hit_result.null) {
            if(button == SDL_BUTTON_LEFT && minecraft->gamemode.gamemode_type != GAMEMODE_CREATIVE) {
                minecraft->miss_time = 10;
            }
        }else {
            if(minecraft->hit_result.type == 1) {
                if(button == SDL_BUTTON_LEFT) {
                    entity_t *hit_entity = minecraft->hit_result.entity;

                    // TODO entity damage

                    return;
                }
            }else if(minecraft->hit_result.type == 0) {
                int hit_x = minecraft->hit_result.x;
                int hit_y = minecraft->hit_result.y;
                int hit_z = minecraft->hit_result.z;
                int face = minecraft->hit_result.face;
                if(button == SDL_BUTTON_LEFT) {
                    world_extinguish_fire(minecraft->world, hit_x, hit_y, hit_z, face);
                    if(world_get_block(minecraft->world, hit_x, hit_y, hit_z) != blocks.bedrock.id) {
                        minecraft->gamemode.start_destroy_block(&minecraft->gamemode, hit_x, hit_y, hit_z);
                        return;
                    }
                }else {
                    player_t *player = minecraft->player.mob->player;
                    item_stack_t current_item = inventory_player_get_selected(&player->inventory);
                    uint8_t block_id = world_get_block(minecraft->world, hit_x, hit_y, hit_z);
                    if(block_id > 0 && block_list[block_id].on_interacted(&block_list[block_id], minecraft->world, hit_x, hit_y, hit_z, &minecraft->player)) {
                        return;
                    }
                    if(current_item.item_id == 0) {
                        return;
                    }

                    int size = current_item.stack_size;
                    if(item_list[current_item.item_id].on_use(&item_list[current_item.item_id], &current_item, minecraft->world, hit_x, hit_y, hit_z, face)) {
                        minecraft->renderer.renderer_overlay.swing_time = -1;
                        minecraft->renderer.renderer_overlay.is_swinging = 1;
                        inventory_player_set_slot(&player->inventory, player->inventory.selected, current_item);
                    }

                    if(current_item.stack_size == 0) {
                        item_stack_t null_item = item_stack_create(0, 0, 0);
                        inventory_player_set_slot(&player->inventory, player->inventory.selected, null_item);
                        return;
                    }

                    if(current_item.stack_size != size) {
                        minecraft->renderer.renderer_overlay.equipped_progress = 0;
                    }
                }
            }
        }
    }
}

void minecraft_tick(minecraft_t *minecraft, SDL_Event *events) {
    glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "terrain.png"));
    if(!minecraft->is_paused) {
        for(int i = 0; i < array_list_length(minecraft->textures.animations); i++) {
            texture_animated_t *texture = minecraft->textures.animations[i];
            texture->anaglyph = minecraft->settings.anaglyph;
            texture->tick(texture);
            memcpy(minecraft->textures.texture_buffer, texture->data, 1024);
            glTexSubImage2D(GL_TEXTURE_2D, 0, texture->texture_id % 16 << 4, texture->texture_id / 16 << 4, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, minecraft->textures.texture_buffer);
        }
    }

    if(minecraft->miss_time > 0) {
        minecraft->miss_time--;
    }

    if(minecraft->current_screen == NULL && minecraft->player.mob->health <= 0) {
        minecraft_set_current_screen(minecraft, NULL);
    }

    if(minecraft->current_screen == NULL || minecraft->current_screen->grabs_mouse) {
        for(int i = 0; i < array_list_length(events); i++) {
            if(events[i].type == SDL_MOUSEWHEEL) {
                int slot = events[i].wheel.y;
                slot = slot > 0 ? 1 : (slot < 0 ? -1 : slot);
                for(minecraft->player.mob->player->inventory.selected -= slot; minecraft->player.mob->player->inventory.selected < 0; minecraft->player.mob->player->inventory.selected += 9);
                while(minecraft->player.mob->player->inventory.selected >= 9) minecraft->player.mob->player->inventory.selected -= 9;
            }
            if(minecraft->current_screen == NULL) {
                if(!minecraft->has_mouse && events[i].type == SDL_MOUSEBUTTONDOWN) {
                    minecraft_grab_mouse(minecraft);
                }else if(events[i].type == SDL_MOUSEBUTTONDOWN) {
                    if(events[i].button.button == SDL_BUTTON_LEFT) {
                        on_mouse_clicked(minecraft, SDL_BUTTON_LEFT);
                        minecraft->last_click = minecraft->ticks;
                    }
                    if(events[i].button.button == SDL_BUTTON_RIGHT) {
                        on_mouse_clicked(minecraft, SDL_BUTTON_RIGHT);
                        minecraft->last_click = minecraft->ticks;
                    }
                    if(events[i].button.button == SDL_BUTTON_MIDDLE && !minecraft->hit_result.null) {
                        uint8_t block_id = world_get_block(minecraft->world, minecraft->hit_result.x, minecraft->hit_result.y, minecraft->hit_result.z);
                        if(block_id == blocks.grass.id) block_id = blocks.dirt.id;
                        if(block_id == blocks.double_slab.id) block_id = blocks.slab.id;
                        if(block_id == blocks.bedrock.id) block_id = blocks.stone.id;
                        inventory_player_get_hotbar_slot(&minecraft->player.mob->player->inventory, block_id);
                    }
                }
            }else {
                screen_mouse_event(minecraft->current_screen, events[i]);
            }
        }
        for(int i = 0; i < array_list_length(events); i++) {
            if(events[i].type == SDL_KEYDOWN || events[i].type == SDL_KEYUP) {
                player_set_key(minecraft->player.mob->player, events[i].key.keysym.scancode, events[i].type == SDL_KEYDOWN);
            }
            if(events[i].type == SDL_KEYDOWN) {
                if(minecraft->current_screen != NULL) {
                    screen_keyboard_event(minecraft->current_screen, events[i]);
                }else {
                    if(events[i].key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        minecraft_pause(minecraft);
                        events[i] = (SDL_Event){ 0 };
                    }
                    if(minecraft->gamemode.gamemode_type == GAMEMODE_CREATIVE) {
                        if(events[i].key.keysym.scancode == minecraft->settings.load_location_key.key) {
                            entity_reset_pos(&minecraft->player);
                        }
                        if(events[i].key.keysym.scancode == minecraft->settings.save_location_key.key) {
                            world_set_spawn_position(minecraft->world, minecraft->player.x, minecraft->player.y, minecraft->player.z);
                            entity_reset_pos(&minecraft->player);
                        }
                    }
                    if(events[i].key.keysym.scancode == SDL_SCANCODE_F5) {
                        minecraft->raining = !minecraft->raining;
                    }
                    if(events[i].key.keysym.scancode == SDL_SCANCODE_TAB && minecraft->gamemode.gamemode_type == GAMEMODE_SURVIVAL && minecraft->player.mob->player->arrows > 0) {
                        /*entity_t *arrow = malloc(sizeof(entity_t));
                        entity_arrow_create(arrow, minecraft->world, &minecraft->player, minecraft->player.x, minecraft->player.y, minecraft->player.z, minecraft->player.y_rot, minecraft->player.x_rot, 1.2);
                        world_spawn_entity(minecraft->world, arrow);
                        minecraft->player.mob->player->arrows--;*/
                    }
                    if(events[i].key.keysym.scancode == minecraft->settings.inventory_key.key) {
                        //minecraft->gamemode.open_inventory((struct gamemode_s *)&minecraft->gamemode);
                        screen_t *inv = malloc(sizeof(screen_t));
                        screen_inventory_create(inv, &minecraft->player.mob->player->inventory);
                        minecraft_set_current_screen(minecraft, inv);
                        events[i] = (SDL_Event){ 0 };
                    }
                    if(events[i].key.keysym.scancode == minecraft->settings.chat_key.key) {
                        events[i] = (SDL_Event){ 0 };
                    }
                }
                for(int j = 0; j < 9; j++) {
                    if(events[i].key.keysym.scancode == SDL_SCANCODE_1 + j) {
                        minecraft->player.mob->player->inventory.selected = j;
                    }
                }
                if(events[i].key.keysym.scancode == minecraft->settings.toggle_fog_key.key) {
                    game_settings_toggle_setting(&minecraft->settings, 4);
                }
            }
        }
    }

    if(minecraft->current_screen == NULL) {
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK && (minecraft->ticks - minecraft->last_click) >= minecraft->timer.ticks_per_second / 4.0) {
            on_mouse_clicked(minecraft, SDL_BUTTON_LEFT);
            minecraft->last_click = minecraft->ticks;
        }
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK && (minecraft->ticks - minecraft->last_click) >= minecraft->timer.ticks_per_second / 4.0) {
            on_mouse_clicked(minecraft, SDL_BUTTON_RIGHT);
            minecraft->last_click = minecraft->ticks;
        }
    }

    if(!minecraft->gamemode.instant_break && minecraft->miss_time <= 0) {
        if(minecraft->current_screen == NULL && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK && !minecraft->hit_result.null && minecraft->hit_result.type == 0) { // and mouse was down
            minecraft->gamemode.continue_destroy_block(&minecraft->gamemode, minecraft->hit_result.x, minecraft->hit_result.y, minecraft->hit_result.z, minecraft->hit_result.face);
        }else {
            minecraft->gamemode.stop_destroy_block(&minecraft->gamemode);
        }
    }

    if(minecraft->current_screen != NULL) {
        if(minecraft->current_screen->removed) {
            minecraft->current_screen->destroy((struct screen_s *)minecraft->current_screen);
            free(minecraft->current_screen);
            minecraft->current_screen = NULL;
        }else {
            minecraft->last_click = minecraft->ticks + 10000;
            screen_do_input(minecraft->current_screen, events);
            if(minecraft->current_screen != NULL) {
                minecraft->current_screen->tick((struct screen_s *)minecraft->current_screen);
            }
        }
    }
    
    /*item_stack_t selected = inventory_player_get_selected(&minecraft->player.mob->player->inventory);
    block_t *block = &block_list[blocks.air.id];
    if(selected.item_id > 0) block = &block_list[selected];
    float s = (block == renderer->held_block.block ? 1.0 : 0.0) - renderer->held_block.position;
    if(s < -0.4) s = -0.4;
    if(s > 0.4) s = 0.4;
    renderer->held_block.position += s;
    if(renderer->held_block.position < 0.1) {
        renderer->held_block.block = block;
    }*/

    /*if(minecraft->raining) {
        world_t *world = minecraft->world;
        int vx = minecraft->player.mob.x;
        int vy = minecraft->player.mob.y;
        int vz = minecraft->player.mob.z;
        for(int i = 0; i < 50; i++) {
            int rx = vx + (int)random_next_int_range(&renderer->random, 0, 8) - 4;
            int rz = vz + (int)random_next_int_range(&renderer->random, 0, 8) - 4;
            int ry = world_get_highest_block(world, rx, rz);
            if(ry <= vy + 4 && ry >= vy - 4) {
                float xo = random_next_uniform(&renderer->random);
                float zo = random_next_uniform(&renderer->random);
                particle_t *particle = malloc(sizeof(particle_t));
                *particle = particle_water_drop_create(world, rx + xo, ry + 0.1, rz + zo);
                particles_spawn_particle(&minecraft->particles, particle);
            }
        }
    }*/

    minecraft->renderer_world.ticks++;
    //world_tick_entities(minecraft->world);
    //world_tick(minecraft->world);
    if(minecraft->world != NULL && !minecraft->is_paused) {
        renderer_camera_update(&minecraft->renderer);
        renderer_world_update_clouds(&minecraft->renderer_world);
        world_update_entities(minecraft->world);
        world_restart_time_of_day(minecraft->world);
        world_visual_update(minecraft->world, floor_double(minecraft->player.x), floor_double(minecraft->player.y), floor_double(minecraft->player.z));
        particles_tick(&minecraft->particles);
    }
}

void minecraft_run(minecraft_t *minecraft) {
    minecraft->running = 1;
    int frame = 0;
    uint64_t start = time_millis();
    //minecraft_grab_mouse(minecraft);
    SDL_Event event;
    SDL_Event *events = array_list_create(sizeof(SDL_Event));

    while(minecraft->running) {
        if(minecraft->timer.elapsed_ticks > 0) events = array_list_clear(events);
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                minecraft->running = 0;
            }
            int x = minecraft->width;
            int y = minecraft->height;
            int w = x;
            int h = y;
            for(x = 1; w / (x + 1) >= 320 && h / (x + 1) >= 240; x++);
            w /= x;
            h /= x;
            if(event.type == SDL_WINDOWEVENT) {
                int ev = event.window.event;
                if(ev == SDL_WINDOWEVENT_RESIZED || ev == SDL_WINDOWEVENT_SIZE_CHANGED || ev == SDL_WINDOWEVENT_MAXIMIZED || ev == SDL_WINDOWEVENT_RESTORED || ev == SDL_WINDOWEVENT_SHOWN || ev == SDL_WINDOWEVENT_ENTER) {
                    SDL_GetWindowSize(minecraft->window, &minecraft->width, &minecraft->height);
                    SDL_GL_GetDrawableSize(minecraft->window, &minecraft->frame_width, &minecraft->frame_height);
                    glViewport(0, 0, minecraft->frame_width, minecraft->frame_height);
                    screen_hud_destroy(&minecraft->hud);
                    minecraft->hud = screen_hud_create((struct minecraft_s *)minecraft, w, h);
                    glClear(GL_COLOR_BUFFER_BIT);
                }

                if(minecraft->current_screen != NULL) {
                    screen_open(minecraft->current_screen, minecraft, w, h);
                }
            }
            SDL_Event copy;
            memcpy(&copy, &event, sizeof(SDL_Event));
            events = array_list_push(events, &copy);
        }
        
        game_timer_t *timer = &minecraft->timer;
        int64_t time = time_millis();
        int64_t d = time - timer->last_system_clock;
        int64_t t2 = time_nano() / 1000000;
        double b;
        if(d > 1000) {
            int64_t d2 = t2 - timer->last_hr_clock;
            b = (double)d / d2;
            timer->adjustment += (b - timer->adjustment) * 0.2;
            timer->last_system_clock = time;
            timer->last_hr_clock = t2;
        }
        if(d < 0) {
            timer->last_system_clock = time;
            timer->last_hr_clock = t2;
        }
        double a = t2 / 1000.0;
        b = (a - timer->last_hr) * timer->adjustment;
        timer->last_hr = a;
        if(b < 0.0) b = 0.0;
        if(b > 1.0) b = 1.0;
        timer->elapsed_delta += b * timer->speed * timer->ticks_per_second;
        timer->elapsed_ticks = timer->elapsed_delta;
        timer->elapsed_delta -= timer->elapsed_ticks;
        if(timer->elapsed_ticks > 10) timer->elapsed_ticks = 10;
        timer->delta = timer->elapsed_delta;
        float delta = timer->delta;

        int ticks = 0;
        for(;;) {
            if(ticks >= minecraft->timer.elapsed_ticks) {
                if(minecraft->is_paused) {
                    minecraft->timer.delta = 1;
                }
                glEnable(GL_TEXTURE);
                if(minecraft->world != NULL) {
                    while(world_update_lighting(minecraft->world));
                }
                gamemode_set_partial_time(&minecraft->gamemode, delta);
                renderer_camera_update_mouse(&minecraft->renderer, delta);
                minecraft->is_paused = minecraft->current_screen != NULL && minecraft->current_screen->pauses_game;
                frame++;
                break;
            }
            minecraft->ticks++;
            minecraft_tick(minecraft, events);
            ticks++;
        }

        renderer_camera_t *renderer = &minecraft->renderer;

        minecraft->gamemode.render(&minecraft->gamemode, delta);
        if(renderer->display_active && (SDL_GetWindowFlags(minecraft->window) & SDL_WINDOW_INPUT_FOCUS) == 0) {
            minecraft_pause(minecraft);
        }
        renderer->display_active = (SDL_GetWindowFlags(minecraft->window) & SDL_WINDOW_INPUT_FOCUS) > 0;
        if(minecraft->has_mouse) {
            int dx = 0;
            int dy = 0;
            int x = 0;
            int y = 0;
            SDL_GetWindowPosition(minecraft->window, &x, &y);
            x += minecraft->width / 2;
            y += minecraft->height / 2;
            SDL_GetGlobalMouseState(&dx, &dy);
            dx -= x;
            dy = -(dy - y);
            SDL_WarpMouseGlobal(x, y);

            entity_turn(&minecraft->player, dy * (minecraft->settings.invert_mouse ? -1 : 1), dx);
        }

        int x = minecraft->width;
        int y = minecraft->height;
        int w = x;
        int h = y;
        for(x = 1; w / (x + 1) >= 320 && h / (x + 1) >= 240; x++);
        w /= x;
        h /= x;
        int mx = 0, my = 0;
        SDL_GetMouseState(&mx, &my);
        mx = mx * w / minecraft->width;
        my = my * h / minecraft->height - 1;

        //screen_hud_render(&minecraft->hud, mx, my, delta);

        /*glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4ub(255, 0, 255, 255);
        glOrtho(0.0, w, h, 0.0, 0.0, 1);
        if(minecraft->current_screen != NULL) {
           // minecraft->current_screen->render((struct screen_s *)minecraft->current_screen, mx, my, delta);
        }*/
        
        //frame++;
        //SDL_GL_SwapWindow(minecraft->window);

        while(time_millis() >= start + 1000) {
            char *chunks = string_create_from_int(chunk_updates);
            string_concat(&chunks, " chunk updates");
            string_set_from_int(&minecraft->debug, frame);
            string_concat(&minecraft->debug, " fps, ");
            string_concat(&minecraft->debug, chunks);
            string_free(chunks);
            start += 1000;
            frame = 0;
            chunk_updates = 0;
        }
    }
    array_list_free(events);
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
#include <windows.h>
    extern uint8_t SetProcessDPIAware(void); //gets rid of implicit warning
    SetProcessDPIAware();
#endif
    minecraft_t minecraft = { 0 };
    memset(&minecraft, 0, sizeof(minecraft_t));
    minecraft_create(&minecraft, 1920, 1080, 0);
    minecraft_run(&minecraft);
    
    if(minecraft.current_screen != NULL) {
        screen_destroy((struct screen_s *)minecraft.current_screen);
        free(minecraft.current_screen);
    }
    world_destroy(minecraft.world);
    free(minecraft.world);
    textures_destroy(&minecraft.textures);
    renderer_world_destroy(&minecraft.renderer_world);
    particles_destroy(&minecraft.particles);
    array_list_free(minecraft.settings.bindings);
    array_list_free(session_allowed_blocks);
    sounds_destroy(&minecraft.sounds);
    
    SDL_DestroyWindow(minecraft.window);
    SDL_Quit();

    return 0;
}