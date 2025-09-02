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
    SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
    minecraft->window = SDL_CreateWindow("Minecraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
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
    #ifdef SURVIVAL_MODE
    minecraft->gamemode = gamemode_survival_create(minecraft);
    #else
    minecraft->gamemode = gamemode_creative_create(minecraft);
    #endif

    minecraft->renderer = renderer_camera_create(minecraft);
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
    world_create(minecraft->world, minecraft, "./.minecraft/saves", "World1", 123456789);
    world_get_chunk(minecraft->world, 0, 0);
    world_get_chunk(minecraft->world, 0, 1);
    world_get_chunk(minecraft->world, 1, 0);
    world_get_chunk(minecraft->world, 1, 1);
    player_create(&minecraft->player, (struct world_s *)minecraft->world);
    world_save(minecraft->world, 1);
    minecraft->world->player = &minecraft->player;
    minecraft->player.inputs = inputs_create(&minecraft->settings);
    minecraft->gamemode.init_player(&minecraft->gamemode, &minecraft->player);
    minecraft->gamemode.adjust_player(&minecraft->gamemode, &minecraft->player);

    renderer_world_create(&minecraft->renderer_world, minecraft, minecraft->world, &minecraft->textures);
    //minecraft->world->renderer = (struct renderer_world_s *)&minecraft->renderer_world;
    //renderer_world_refresh((renderer_world_t *)&minecraft->renderer_world);
    minecraft->particles = particles_create(minecraft->world, &minecraft->textures);
    minecraft->world->particles = &minecraft->particles;

    minecraft->hud = screen_hud_create((struct minecraft_s *)minecraft, width, height);
    //minecraft_grab_mouse(minecraft);
    if(minecraft->world != NULL) {
        screen_t *main_menu = malloc(sizeof(screen_t));
        *main_menu = screen_title_create();
        minecraft_set_current_screen(minecraft, main_menu);
    }

    //glViewport(0, 0, minecraft->width, minecraft->height);
    /*glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4ub(255, 0, 255, 255);
    int x = minecraft->width;
    int y = minecraft->height;
    int w = x;
    int h = y;
    for(x = 1; w / (x + 1) >= 320 && h / (x + 1) >= 240; x++);
    w /= x;
    h /= x;
    glOrtho(0.0, w, h, 0.0, 0.0, 1);
    char *splash = "TESTING!!!";
    while(1) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glBindTexture(GL_TEXTURE_2D, textures_load(&((minecraft_t *)minecraft)->textures, "dirt.png"));
        glColor4f(1.0, 1.0, 1.0, 1.0);
        tesselator_begin_quads();
        tesselator_color_opaque_int(0x40404000);
        tesselator_vertex_uv(0, h, 0, 0, h / 32);
        tesselator_vertex_uv(w, h, 0, w / 32, h / 32);
        tesselator_vertex_uv(w, 0, 0, w / 32, 0);
        tesselator_vertex_uv(0, 0, 0, 0, 0);
        tesselator_end();

        glBindTexture(GL_TEXTURE_2D, textures_load(&((minecraft_t *)minecraft)->textures, "gui/logo.png"));
        glEnable(GL_TEXTURE_2D);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        tesselator_color(255, 255, 255, 255);
        gui_blit((w - 256) / 2, 30, 0, 0, 256, 49, 0);
        glPushMatrix();
        glTranslatef((w / 2) + 90, 70, 0);
        glRotatef(-20.0, 0.0, 0.0, 1.0);
        float splash_scale = 1.8 - fabs(tsin((time_millis() % 1000) / 1000.0 * M_PI * 2) * 0.1);
        splash_scale *= 100.0 / (font_get_width(&minecraft->font, splash) + 32);
        glScalef(splash_scale, splash_scale, splash_scale);
        glEnable(GL_TEXTURE_2D);
        gui_draw_centered_string(&minecraft->font, splash, 0, -8, 0xFFFF00);
        glPopMatrix();
        char *copyright = "Copyright Mojang Specifications. Do not distribute.";
        gui_draw_string(&minecraft->font, copyright, w - font_get_width(&minecraft->font, copyright) - 2, h - 10, 0xFFFFFFFF);


        SDL_GL_SwapWindow(minecraft->window);
    }*/
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
    }else if(screen == NULL && minecraft->player.health <= 0) {
        screen = malloc(sizeof(screen_t));
        *screen = screen_death_create();
    }
    if(screen != NULL) {
        minecraft->current_screen = screen;
        if(minecraft->has_mouse) {
            inputs_reset_keys(&minecraft->player.inputs);
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
    entity_reset_pos(&minecraft->player.mob.entity);
    minecraft->gamemode.init_player(&minecraft->gamemode, &minecraft->player);
    minecraft->gamemode.adjust_player(&minecraft->gamemode, &minecraft->player);
    minecraft->player.inputs = inputs_create(&minecraft->settings);
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
            minecraft->renderer.held_block.offset = -1;
            minecraft->renderer.held_block.moving = 1;
        }
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK && minecraft->player.inventory.selected > 0 && minecraft->gamemode.use_item(&minecraft->gamemode, &minecraft->player, minecraft->player.inventory.selected)) {
            minecraft->renderer.held_block.position = 0;
        }else if(minecraft->hit_result.null) {
            if(button == SDL_BUTTON_LEFT && minecraft->gamemode.gamemode_type == GAMEMODE_SURVIVAL) {
                minecraft->miss_time = 10;
            }
        }else {
            if(minecraft->hit_result.type == 1) {
                if(button == SDL_BUTTON_LEFT) {
                    entity_t *entity = minecraft->hit_result.entity;
                    if(entity != NULL) {
                        entity->hurt(entity, &minecraft->player.entity, 4);
                        return;
                    }
                }
            }else if(minecraft->hit_result.type == 0) {
                int vx = minecraft->hit_result.x;
                int vy = minecraft->hit_result.y;
                int vz = minecraft->hit_result.z;
                if(button != SDL_BUTTON_LEFT) {
                    if(minecraft->hit_result.face == 0) vy--;
                    if(minecraft->hit_result.face == 1) vy++;
                    if(minecraft->hit_result.face == 2) vz--;
                    if(minecraft->hit_result.face == 3) vz++;
                    if(minecraft->hit_result.face == 4) vx--;
                    if(minecraft->hit_result.face == 5) vx++;
                }
                block_t *block = &block_list[world_get_block(minecraft->world, vx, vy, vz)];
                if(button == SDL_BUTTON_LEFT) {
                    if(block->id != blocks.bedrock.id) {
                        minecraft->gamemode.start_destroy_block(&minecraft->gamemode, vx, vy, vz);
                        return;
                    }
                }else {
                    int selected = inventory_get_selected(&minecraft->player.inventory);
                    if(selected <= 0) return;
    
                    block_t *block = &block_list[world_get_block(minecraft->world, vx, vy, vz)];
                    block_t *selected_block = &block_list[selected];
                    AABB_t aabb = block_list[selected].id == blocks.air.id ? (AABB_t){ .null = 1 } : selected_block->get_collision_aabb(selected_block, vx, vy, vz);
                    if((block->id == blocks.air.id || block->id == blocks.water.id || block->id == blocks.still_water.id || block->id == blocks.lava.id || block->id == blocks.still_lava.id) && (aabb.null || !AABB_intersects(minecraft->player.mob.bb, aabb))) {
                        if(!minecraft->gamemode.remove_item(&minecraft->gamemode, selected)) {
                            return;
                        }
                        world_set_block_with_update(minecraft->world, vx, vy, vz, selected);
                        selected_block->on_placed(selected_block, (struct world_s *)minecraft->world, vx, vy, vz, minecraft->hit_result.face);
                        minecraft->renderer.held_block.position = 0;
                    }
                }
            }
        }
    }
}

void minecraft_tick(minecraft_t *minecraft, SDL_Event *events) {
    if(time_millis() > minecraft->sounds.last_music) {
        sounds_play_music(&minecraft->sounds, "Calm");
        minecraft->sounds.last_music = time_millis() + random_next_int_range(&minecraft->sounds.random, 0, 900000) + 300000;
    }

    glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "terrain.png"));
    for(int i = 0; i < array_list_length(minecraft->textures.animations); i++) {
        texture_animated_t *texture = minecraft->textures.animations[i];
        texture->anaglyph = minecraft->settings.anaglyph;
        texture->tick(texture);
        memcpy(minecraft->textures.texture_buffer, texture->data, 1024);
        glTexSubImage2D(GL_TEXTURE_2D, 0, texture->texture_id % 16 << 4, texture->texture_id / 16 << 4, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, minecraft->textures.texture_buffer);
    }

    if(minecraft->miss_time > 0) {
        minecraft->miss_time--;
    }

    if(minecraft->current_screen == NULL && minecraft->player.health <= 0) {
        minecraft_set_current_screen(minecraft, NULL);
    }

    if(minecraft->current_screen == NULL || minecraft->current_screen->grabs_mouse) {
        for(int i = 0; i < array_list_length(events); i++) {
            if(events[i].type == SDL_MOUSEWHEEL) {
                inventory_swap_paint(&minecraft->player.inventory, events[i].wheel.y);
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
                        inventory_grab_texture(&minecraft->player.inventory, block_id);
                    }
                }
            }else {
                screen_mouse_event(minecraft->current_screen, events[i]);
            }
        }
        for(int i = 0; i < array_list_length(events); i++) {
            if(events[i].type == SDL_KEYDOWN || events[i].type == SDL_KEYUP) {
                player_set_key(&minecraft->player, events[i].key.keysym.scancode, events[i].type == SDL_KEYDOWN);
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
                            entity_reset_pos(&minecraft->player.mob.entity);
                        }
                        if(events[i].key.keysym.scancode == minecraft->settings.save_location_key.key) {
                            world_set_spawn_position(minecraft->world, minecraft->player.mob.x, minecraft->player.mob.y, minecraft->player.mob.z);
                            entity_reset_pos(&minecraft->player.mob.entity);
                        }
                    }
                    if(events[i].key.keysym.scancode == SDL_SCANCODE_F5) {
                        minecraft->raining = !minecraft->raining;
                    }
                    if(events[i].key.keysym.scancode == SDL_SCANCODE_TAB && minecraft->gamemode.gamemode_type == GAMEMODE_SURVIVAL && minecraft->player.arrows > 0) {
                        entity_t *arrow = malloc(sizeof(entity_t));
                        entity_arrow_create(arrow, minecraft->world, &minecraft->player.mob.entity, minecraft->player.x, minecraft->player.y, minecraft->player.z, minecraft->player.y_rot, minecraft->player.x_rot, 1.2);
                        world_spawn_entity(minecraft->world, arrow);
                        minecraft->player.arrows--;
                    }
                    if(events[i].key.keysym.scancode == minecraft->settings.build_key.key) {
                        minecraft->gamemode.open_inventory((struct gamemode_s *)&minecraft->gamemode);
                    }
                    if(events[i].key.keysym.scancode == minecraft->settings.chat_key.key) {
                        events[i] = (SDL_Event){ 0 };
                    }
                }
                for(int j = 0; j < 9; j++) {
                    if(events[i].key.keysym.scancode == SDL_SCANCODE_1 + j) {
                        minecraft->player.inventory.selected = j;
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

    renderer_camera_t *renderer = &minecraft->renderer;
    renderer->held_block.last_position = renderer->held_block.position;
    if(renderer->held_block.moving) {
        renderer->held_block.offset++;
        if(renderer->held_block.offset == 7) {
            renderer->held_block.offset = 0;
            renderer->held_block.moving = 0;
        }
    }
    int selected = inventory_get_selected(&minecraft->player.inventory);
    block_t *block = &block_list[blocks.air.id];
    if(selected > 0) block = &block_list[selected];
    float s = (block == renderer->held_block.block ? 1.0 : 0.0) - renderer->held_block.position;
    if(s < -0.4) s = -0.4;
    if(s > 0.4) s = 0.4;
    renderer->held_block.position += s;
    if(renderer->held_block.position < 0.1) {
        renderer->held_block.block = block;
    }

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
    particles_tick(&minecraft->particles);
}

void minecraft_run(minecraft_t *minecraft) {
    minecraft->running = 1;
    int frame = 0;
    //uint64_t start = time_millis();
    //minecraft_grab_mouse(minecraft);
    SDL_Event event;
    SDL_Event *events = array_list_create(sizeof(SDL_Event));

    while(minecraft->running) {
        if(minecraft->timer.elapsed_ticks > 0) events = array_list_clear(events);
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                minecraft->running = 0;
            }
            if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_GetWindowSize(minecraft->window, &minecraft->width, &minecraft->height);
                SDL_GL_GetDrawableSize(minecraft->window, &minecraft->frame_width, &minecraft->frame_height);
                glViewport(0, 0, minecraft->frame_width, minecraft->frame_height);
                screen_hud_destroy(&minecraft->hud);
                minecraft->hud = screen_hud_create((struct minecraft_s *)minecraft, minecraft->width, minecraft->height);
                glClear(GL_COLOR_BUFFER_BIT);

                if(minecraft->current_screen != NULL) {
                    int x = minecraft->width;
                    int y = minecraft->height;
                    int w = x;
                    int h = y;
                    for(x = 1; w / (x + 1) >= 320 && h / (x + 1) >= 240; x++);
                    w /= x;
                    h /= x;
                    screen_open(minecraft->current_screen, minecraft, w, h);
                }
            }
            SDL_Event copy;
            memcpy(&copy, &event, sizeof(SDL_Event));
            events = array_list_push(events, &copy);
        }
        
        game_timer_t *timer = &minecraft->timer;
        uint64_t time = time_millis();
        uint64_t d = time - timer->last_system_clock;
        uint64_t t2 = time_nano() / 1000000;
        double b;
        if(d > 1000) {
            uint64_t d2 = t2 - timer->last_hr_clock;
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
        if(timer->elapsed_ticks > 100) timer->elapsed_ticks = 100;
        timer->elapsed_delta -= timer->elapsed_ticks;
        timer->delta = timer->elapsed_delta;
        float delta = timer->delta;

        renderer_camera_update_camera(&minecraft->renderer, delta);

        for(int i = 0; i < timer->elapsed_ticks; i++) {
            minecraft->ticks++;
            minecraft_tick(minecraft, events);
            events = array_list_clear(events);
        }

        glEnable(GL_TEXTURE_2D);

        minecraft->gamemode.render(&minecraft->gamemode, delta);
        renderer_camera_t *renderer = &minecraft->renderer;
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

            entity_turn(&minecraft->player.mob.entity, dy * (minecraft->settings.invert_mouse ? -1 : 1), dx);
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

        if(minecraft->world != NULL) {
            //world_update_lighting(minecraft->world);
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4ub(255, 0, 255, 255);
        glOrtho(0.0, w, h, 0.0, 0.0, 1);
        if(minecraft->current_screen != NULL) {
            minecraft->current_screen->render((struct screen_s *)minecraft->current_screen, mx, my, delta);
        }
        
        frame++;
        SDL_GL_SwapWindow(minecraft->window);

        /*while(time_millis() >= start + 1000) {
            char *chunks = string_create_from_int(chunk_updates);
            string_concat(&chunks, " chunk updates");
            string_set_from_int(&minecraft->debug, frame);
            string_concat(&minecraft->debug, " fps, ");
            string_concat(&minecraft->debug, chunks);
            string_free(chunks);
            start += 1000;
            frame = 0;
            chunk_updates = 0;
        }*/
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
    free(minecraft.textures.animations[0]); //water animated texture
    free(minecraft.textures.animations[1]); //lava animated texture
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