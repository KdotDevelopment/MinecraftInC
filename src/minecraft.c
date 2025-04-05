#include <minecraft.h>
#include <session_data.h>
#include <renderer/game_renderer.h>
#include <renderer/tesselator.h>
#include <renderer/frustum.h>
#include <renderer/texture/texture_lava.h>
#include <renderer/texture/texture_water.h>
#include <level/block/blocks.h>
#include <level/level.h>
#include <gui/screen_pause.h>
#include <gui/screen_block_select.h>
#include <gui/screen_death.h>
#include <particle/particle_water_drop.h>
#include <model/models.h>
#include <player/gamemode/gamemode_creative.h>
#include <player/gamemode/gamemode_survival.h>

#include <util/time.h>
#include <util/array_list.h>
#include <util/sin_table.h>

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
    minecraft->has_mouse = 1;
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

    sin_table_initialize();
    blocks_init();
    session_data_initialize();
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

    minecraft->renderer = game_renderer_create(minecraft);
    game_settings_create(&minecraft->settings, (struct minecraft_s *)minecraft);
    SDL_GL_SetSwapInterval(minecraft->settings.limit_framerate ? 1 : 0);
    minecraft->textures = textures_create(&minecraft->settings);

    texture_animated_t *lava_texture = malloc(sizeof(texture_animated_t));
    *lava_texture = texture_lava_create();
    textures_register_animation(&minecraft->textures, lava_texture);
    texture_animated_t *water_texture = malloc(sizeof(texture_animated_t));
    *water_texture = texture_water_create();
    textures_register_animation(&minecraft->textures, water_texture);

    minecraft->font = font_create(&minecraft->settings, "default.png", &minecraft->textures);

    glViewport(0, 0, minecraft->frame_width, minecraft->frame_height);

    level_create(&minecraft->level, minecraft, &minecraft->progress_bar, 1);
    minecraft->gamemode.init_level(&minecraft->gamemode, &minecraft->level);
    player_create(&minecraft->player, (struct level_s *)&minecraft->level);
    minecraft->level.player = &minecraft->player;
    minecraft->player.inputs = inputs_create(&minecraft->settings);
    minecraft->gamemode.init_player(&minecraft->gamemode, &minecraft->player);
    minecraft->gamemode.adjust_player(&minecraft->gamemode, &minecraft->player);

    minecraft->level_renderer = level_renderer_create(minecraft, &minecraft->level, &minecraft->textures);
    minecraft->level.renderer = (struct level_renderer_s *)&minecraft->level_renderer;
    level_renderer_refresh((level_renderer_t *)&minecraft->level_renderer);
    minecraft->particles = particles_create(&minecraft->level, &minecraft->textures);
    minecraft->level.particles = &minecraft->particles;

    minecraft->hud = screen_hud_create((struct minecraft_s *)minecraft, width, height);
    minecraft_grab_mouse(minecraft);
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
    if(screen == NULL && minecraft->player.health <= 0) {
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
        int w = minecraft->width * 240 / minecraft->height;
        int h = minecraft->height * 240 / minecraft->height;

        screen_open(screen, minecraft, w, h);
    }else {
        minecraft->current_screen = NULL;
        minecraft_grab_mouse(minecraft);
    }
}

void minecraft_regenerate_level(minecraft_t *minecraft, int size) {
    level_regenerate(&minecraft->level, size);
    minecraft->gamemode.init_level(&minecraft->gamemode, &minecraft->level);
    player_create(&minecraft->player, (struct level_s *)&minecraft->level);
    entity_reset_pos(&minecraft->player.mob.entity);
    minecraft->gamemode.init_player(&minecraft->gamemode, &minecraft->player);
    minecraft->gamemode.adjust_player(&minecraft->gamemode, &minecraft->player);
    minecraft->player.inputs = inputs_create(&minecraft->settings);
    level_renderer_destroy(&minecraft->level_renderer);
    minecraft->level_renderer = level_renderer_create(minecraft, &minecraft->level, &minecraft->textures);
    minecraft->level.renderer = (struct level_renderer_s *)&minecraft->level_renderer;
    level_renderer_refresh((level_renderer_t *)&minecraft->level_renderer);
    particles_destroy(&minecraft->particles);
    minecraft->particles = particles_create(&minecraft->level, &minecraft->textures);
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
                block_t *block = &block_list[level_get_block(&minecraft->level, vx, vy, vz)];
                if(button == SDL_BUTTON_LEFT) {
                    if(block->id != blocks.bedrock.id) {
                        minecraft->gamemode.start_destroy_block(&minecraft->gamemode, vx, vy, vz);
                        return;
                    }
                }else {
                    int selected = inventory_get_selected(&minecraft->player.inventory);
                    if(selected <= 0) return;
    
                    block_t *block = &block_list[level_get_block(&minecraft->level, vx, vy, vz)];
                    block_t *selected_block = &block_list[selected];
                    AABB_t aabb = block_list[selected].id == blocks.air.id ? (AABB_t){ .null = 1 } : selected_block->get_collision_aabb(selected_block, vx, vy, vz);
                    if((block->id == blocks.air.id || block->id == blocks.water.id || block->id == blocks.still_water.id || block->id == blocks.lava.id || block->id == blocks.still_lava.id) && (aabb.null || !AABB_intersects(minecraft->player.mob.bb, aabb))) {
                        if(!minecraft->gamemode.remove_item(&minecraft->gamemode, selected)) {
                            return;
                        }
                        level_set_block(&minecraft->level, vx, vy, vz, selected);
                        selected_block->on_placed(selected_block, (struct level_s *)&minecraft->level, vx, vy, vz);
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
                        uint8_t block_id = level_get_block(&minecraft->level, minecraft->hit_result.x, minecraft->hit_result.y, minecraft->hit_result.z);
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
                            level_set_spawn_position(&minecraft->level, minecraft->player.mob.x, minecraft->player.mob.y, minecraft->player.mob.z, minecraft->player.mob.y_rot);
                            entity_reset_pos(&minecraft->player.mob.entity);
                        }
                    }
                    if(events[i].key.keysym.scancode == SDL_SCANCODE_F5) {
                        minecraft->raining = !minecraft->raining;
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

    game_renderer_t *renderer = &minecraft->renderer;
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

    if(minecraft->raining) {
        level_t *level = &minecraft->level;
        int vx = minecraft->player.mob.x;
        int vy = minecraft->player.mob.y;
        int vz = minecraft->player.mob.z;
        for(int i = 0; i < 50; i++) {
            int rx = vx + (int)random_next_int_range(&renderer->random, 0, 8) - 4;
            int rz = vz + (int)random_next_int_range(&renderer->random, 0, 8) - 4;
            int ry = level_get_highest_block(level, rx, rz);
            if(ry <= vy + 4 && ry >= vy - 4) {
                float xo = random_next_uniform(&renderer->random);
                float zo = random_next_uniform(&renderer->random);
                particle_t *particle = malloc(sizeof(particle_t));
                *particle = particle_water_drop_create(level, rx + xo, ry + 0.1, rz + zo);
                particles_spawn_particle(&minecraft->particles, particle);
            }
        }
    }

    minecraft->level_renderer.ticks++;
    level_tick_entities(&minecraft->level);
    level_tick(&minecraft->level);
    particles_tick(&minecraft->particles);
}

void minecraft_run(minecraft_t *minecraft) {
    minecraft->running = 1;
    int frame = 0;
    uint64_t start = time_millis();
    minecraft_grab_mouse(minecraft);
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

                if(minecraft->current_screen != NULL) {
                    int w = minecraft->width * 240 / minecraft->height;
                    int h = minecraft->height * 240 / minecraft->height;
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

        for(int i = 0; i < timer->elapsed_ticks; i++) {
            minecraft->ticks++;
            minecraft_tick(minecraft, events);
            events = array_list_clear(events);
        }

        glEnable(GL_TEXTURE_2D);

        float delta = timer->delta;
        minecraft->gamemode.render(&minecraft->gamemode, delta);
        game_renderer_t *renderer = &minecraft->renderer;
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

        int w = minecraft->width * 240 / minecraft->height;
        int h = minecraft->height * 240 / minecraft->height;
        int mx = 0, my = 0;
        SDL_GetMouseState(&mx, &my);
        mx = mx * w / minecraft->width;
        my = my * h / minecraft->height - 1;

        player_t *player = &minecraft->player;
        float rot_x = player->x_roto + (player->x_rot - player->x_roto) * delta;
        float rot_y = player->y_roto + (player->y_rot - player->y_roto) * delta;
        vec3_t v = renderer_get_player_vector(renderer, delta);
        float c1 = tcos(-rot_y * M_PI / 180.0 - M_PI);
        float s1 = tsin(-rot_y * M_PI / 180.0 - M_PI);
        float c2 = tcos(-rot_x * M_PI / 180.0);
        float s2 = tsin(-rot_x * M_PI / 180.0);
        float sc = s1 * c2;
        float cc = c1 * c2;
        float reach = minecraft->gamemode.reach;
        vec3_t v2 = { v.x + sc * reach, v.y + s2 * reach, v.z + cc * reach };
        minecraft->hit_result = level_clip(&minecraft->level, v, v2);
        if(!minecraft->hit_result.null) {
            reach = sqrtf(vec3_distance_to_sqr(v, minecraft->hit_result.location));
        }
        renderer->entity = NULL;
        a = 0.0;

        AABB_t box = AABB_expand(player->bb, sc * reach, s2 * reach, cc * reach);
        entity_t ***entities = entity_map_get_entities(&minecraft->level.entity_map, &player->entity, box.x0, box.y0, box.z0, box.x1, box.y1, box.z1);
        for(int i = 0; i < array_list_length(*entities); i++) {
            entity_t *entity = *(entity_t **)array_list_get(*entities, i);
            if(entity->can_be_hit(entity)) {
                float r = 0.1;
                float dist;
                hit_result_t pos = AABB_clip(AABB_grow(entity->bb, r, r, r), v, v2);
                if(!pos.null) dist = (vec3_distance_to(v, pos.location));
                if(!pos.null && (dist < a || a == 0.0)) {
                    renderer->entity = entity;
                    a = dist;
                }
            }
        }

        if(renderer->entity != NULL && minecraft->gamemode.gamemode_type == GAMEMODE_SURVIVAL) {
            minecraft->hit_result = (hit_result_t){ .type = 1, .entity = renderer->entity };
        }

        for(int i = 0; i < 2; i++) {
            if(minecraft->settings.anaglyph) {
                if(i == 0) glColorMask(0, 1, 1, 0);
                else glColorMask(1, 0, 0, 0);
            }

            level_t *level = &minecraft->level;
            glViewport(0, 0, minecraft->frame_width, minecraft->frame_height);
            float a = 1.0 / (4 - minecraft->settings.view_distance);
            a = 1.0 - powf(a, 0.25);
            float sky_r = ((level->sky_color >> 24) & 0xFF) / 255.0;
            float sky_g = ((level->sky_color >> 16) & 0xFF) / 255.0;
            float sky_b = ((level->sky_color >> 8) & 0xFF) / 255.0;
            renderer->fog_r = ((level->fog_color >> 24) & 0xFF) / 255.0;
            renderer->fog_g = ((level->fog_color >> 16) & 0xFF) / 255.0;
            renderer->fog_b = ((level->fog_color >> 8) & 0xFF) / 255.0;
            renderer->fog_r = (renderer->fog_r + (sky_r - renderer->fog_r) * a) * renderer->fog_color_multiplier;
            renderer->fog_g = (renderer->fog_g + (sky_g - renderer->fog_g) * a) * renderer->fog_color_multiplier;
            renderer->fog_b = (renderer->fog_b + (sky_b - renderer->fog_b) * a) * renderer->fog_color_multiplier;
            block_t *block = &block_list[level_get_block(level, player->x, player->y + 0.12, player->z)];
            if(block->id != blocks.air.id && block->liquid_type != LIQUID_NONE) {
                if(block->liquid_type == LIQUID_WATER) {
                    renderer->fog_r = 0.02;
                    renderer->fog_g = 0.02;
                    renderer->fog_b = 0.2;
                }
                if(block->liquid_type == LIQUID_LAVA) {
                    renderer->fog_r = 0.6;
                    renderer->fog_g = 0.0;
                    renderer->fog_b = 0.1;
                }
            }
            if(minecraft->settings.anaglyph) {
                float ar = (renderer->fog_r * 30.0 + renderer->fog_g * 59.0 + renderer->fog_b * 11.0) / 100.0;
                float ag = (renderer->fog_r * 30.0 + renderer->fog_g * 70.0) / 100.0;
                float ab = (renderer->fog_r * 30.0 + renderer->fog_b * 70.0) / 100.0;
                renderer->fog_r = ar;
                renderer->fog_g = ag;
                renderer->fog_b = ab;
            }
            
            glClearColor(renderer->fog_r, renderer->fog_g, renderer->fog_b, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer->fog_color_multiplier = 1.0;
            glEnable(GL_CULL_FACE);
            renderer->fog_end = (512 >> (minecraft->settings.view_distance << 1));
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            if(minecraft->settings.anaglyph) glTranslatef(-((i << 1) - 1) * 0.07, 0.0, 0.0);

            gluPerspective(70.0, (float)minecraft->width / (float)minecraft->height, 0.05, renderer->fog_end);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            if(minecraft->settings.anaglyph) glTranslatef(((i << 1) - 1) * 0.1, 0.0, 0.0);
            if(minecraft->settings.show_bobbing) renderer_apply_bobbing(renderer, delta);

            glTranslatef(0.0, 0.0, -0.1);
            float rot_x = player->x_roto + (player->x_rot - player->x_roto) * delta;
            float rot_y = player->y_roto + (player->y_rot - player->y_roto) * delta;
            glRotatef(rot_x, 1.0, 0.0, 0.0);
            glRotatef(rot_y, 0.0, 1.0, 0.0);
            float pos_x = player->xo + (player->x - player->xo) * delta;
            float pos_y = player->yo + (player->y - player->yo) * delta;
            float pos_z = player->zo + (player->z - player->zo) * delta;
            glTranslatef(-pos_x, -pos_y, -pos_z);

            frustum_t frustum = frustum_get();
            level_renderer_t *lrenderer = &minecraft->level_renderer;
            for(int j = 0; j < lrenderer->chunk_cache_count; j++) {
                chunk_clip(lrenderer->chunk_cache[j], frustum);
            }

            qsort(lrenderer->chunks, array_list_length(lrenderer->chunks), sizeof(chunk_t *), chunk_visible_distance_comparator);

            int limit = array_list_length(lrenderer->chunks);
            if(limit > 3) limit = 3;
            for(int j = 0; j < limit; j++) {
                chunk_t *chunk = lrenderer->chunks[0];
                lrenderer->chunks = array_list_remove(lrenderer->chunks, 0);
                chunk_update(chunk);
                chunk->loaded = 0;
            }

            renderer_setup_fog(renderer);
            glEnable(GL_FOG);
            level_renderer_sort_chunks(lrenderer, player, 0);
            if(level_is_solid_search(level, player->x, player->y, player->z, 0.1)) {
                int px = player->x;
                int py = player->y;
                int pz = player->z;
                for(int x = px - 1; x <= px + 1; x++) {
                    for(int y = py - 1; y <= py + 1; y++) {
                        for(int z = pz - 1; z <= pz + 1; z++) {
                            int vx = x;
                            int vy = y;
                            int vz = z;
                            uint8_t block_id = level_get_block(level, x, y, z);
                            block_t *block = &block_list[block_id];
                            if(block_id != blocks.air.id && block->is_solid) {
                                glColor4f(0.2, 0.2, 0.2, 1.0);
                                glDepthFunc(GL_LESS);
                                tesselator_begin();
                                for(int j = 0; j < 6; j++) {
                                    block->render_inside(block, vx, vy, vz, j);
                                }
                                tesselator_end();
                                glCullFace(GL_FRONT);
                                tesselator_begin();
                                for(int j = 0; j < 6; j++) {
                                    block->render_inside(block, vx, vy, vz, j);
                                }
                                tesselator_end();
                                glCullFace(GL_BACK);
                                glDepthFunc(GL_LEQUAL);
                            }
                        }
                    }
                }
            }

            renderer_set_lighting(renderer, 1);
            entity_map_render(&minecraft->level.entity_map, v, &frustum, &minecraft->textures, timer->delta);
            particles_t *particles = &minecraft->particles;
            renderer_set_lighting(renderer, 0);
            renderer_setup_fog(renderer);
            float dt = delta;
            float c = -tcos(player->y_rot * M_PI / 180.0);
            float s = -tsin(player->y_rot * M_PI / 180.0);
            float ss = -s * tsin(player->x_rot * M_PI / 180.0);
            float cs = c * tsin(player->x_rot * M_PI / 180.0);
            float c2 = tcos(player->x_rot * M_PI / 180.0);
            for(int j = 0; j < 2; j++) {
                if(array_list_length(particles->particles[j]) != 0) {
                    int tex = 0;
                    if(j == 0) tex = textures_load(&minecraft->textures, "particles.png");
                    if(j == 1) tex = textures_load(&minecraft->textures, "terrain.png");
                    glBindTexture(GL_TEXTURE_2D, tex);
                    tesselator_begin();
                    for(int k = 0; k < array_list_length(particles->particles[j]); k++) {
                        particle_t *particle = particles->particles[j][k];
                        particle->render(particle, dt, c, c2, s, ss, cs);
                    }
                    tesselator_end();
                }
            }
            
            glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "rock.png"));
            glEnable(GL_TEXTURE_2D);
            glCallList(lrenderer->list_id);
            renderer_setup_fog(renderer);
            glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "clouds.png"));
            glColor4f(1.0, 1.0, 1.0, 1.0);
            float cloud_r = ((level->cloud_color >> 24) & 0xFF) / 255.0;
            float cloud_g = ((level->cloud_color >> 16) & 0xFF) / 255.0;
            float cloud_b = ((level->cloud_color >> 8) & 0xFF) / 255.0;
            if(minecraft->settings.anaglyph) {
                float ar = (cloud_r * 30.0 + cloud_g * 59.0 + cloud_b * 11.0) / 100.0;
                float ag = (cloud_r * 30.0 + cloud_g * 70.0) / 100.0;
                float ab = (cloud_r * 30.0 + cloud_b * 70.0) / 100.0;
                cloud_r = ar;
                cloud_g = ag;
                cloud_b = ab;
            }
            
            float z = level->depth + 2.0;
            float t = 0.03 * (lrenderer->ticks + delta);
            tesselator_begin();
            tesselator_color(cloud_r, cloud_g, cloud_b);
            for(int x = -2048; x < (int)level->width + 2048; x += 512) {
                for(int y = -2048; y < (int)level->height + 2048; y += 512) {
                    tesselator_vertex_uv(x, z, y + 512, (x + t) / 2048.0, (y + 512) / 2048.0);
                    tesselator_vertex_uv(x + 512, z, y + 512, (x + 512 + t) / 2048.0, (y + 512) / 2048.0);
                    tesselator_vertex_uv(x + 512, z, y, (x + 512 + t) / 2048.0, y / 2048.0);
                    tesselator_vertex_uv(x, z, y, (x + t) / 2048.0, y / 2048.0);
                    tesselator_vertex_uv(x, z, y, (x + t) / 2048.0, y / 2048.0);
                    tesselator_vertex_uv(x + 512, z, y, (x + 512 + t) / 2048.0, y / 2048.0);
                    tesselator_vertex_uv(x + 512, z, y + 512, (x + 512 + t) / 2048.0, (y + 512) / 2048.0);
                    tesselator_vertex_uv(x, z, y + 512, (x + t) / 2048.0, (y + 512) / 2048.0);
                }
            }
            tesselator_end();

            glDisable(GL_TEXTURE_2D);
            tesselator_begin();
            if(minecraft->settings.anaglyph) {
                float ar = (sky_r * 30.0 + sky_g * 59.0 + sky_b * 11.0) / 100.0;
                float ag = (sky_r * 30.0 + sky_g * 70.0) / 100.0;
                float ab = (sky_r * 30.0 + sky_b * 70.0) / 100.0;
                sky_r = ar;
                sky_g = ag;
                sky_b = ab;
            }
            tesselator_color(sky_r, sky_g, sky_b);
            z = level->depth + 10.0;
            for(int x = -2048; x < (int)level->width + 2048; x += 512) {
                for(int y = -2048; y < (int)level->height + 2048; y += 512) {
                    tesselator_vertex(x, z, y);
                    tesselator_vertex(x + 512, z, y);
                    tesselator_vertex(x + 512, z, y + 512);
                    tesselator_vertex(x, z, y + 512);
                }
            }
            tesselator_end();
            glEnable(GL_TEXTURE_2D);
            renderer_setup_fog(renderer);

            if(!minecraft->hit_result.null) {
                glDisable(GL_ALPHA_TEST);
                hit_result_t pos = minecraft->hit_result;
                glEnable(GL_BLEND);
                glEnable(GL_ALPHA_TEST);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glColor4f(1.0, 1.0, 1.0, (tsin(time_millis() / 100.0) * 0.2 + 0.4) * 0.5);
                if(lrenderer->destroy_progress > 0) {
                    glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
                    glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "terrain.png"));
                    glColor4f(1, 1, 1, 0.5);
                    glPushMatrix();
                    uint8_t block_id = level_get_block(level, pos.x, pos.y, pos.z);
                    block_t *block = NULL;
                    if(block_id != blocks.air.id) {
                        block = &block_list[block_id];
                    }
                    float center_x = (block->x0 + block->x1 + 0.001) / 2;
                    float center_y = (block->y0 + block->y1 + 0.001) / 2;
                    float center_z = (block->z0 + block->z1 + 0.001) / 2;
                    glTranslatef(pos.x + center_x, pos.y + center_y, pos.z + center_z);
                    glScalef(1.0005, 1.0005, 1.0005);
                    glTranslatef(-(pos.x + center_x), -(pos.y + center_y), -(pos.z + center_z));
                    tesselator_begin();
                    glDepthMask(0);
                    if(block == NULL) block = &block_list[blocks.stone.id];
                    for(int j = 0; j < 6; j++) {
                        block_render_side_with_texture(block, pos.x, pos.y, pos.z, j, 240 + lrenderer->destroy_progress * 10);
                    }
                    tesselator_end();
                    glDepthMask(1);
                    glPopMatrix();
                }
                glDisable(GL_BLEND);
                glDisable(GL_ALPHA_TEST);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glColor4f(0.0, 0.0, 0.0, 0.4);
                glLineWidth(2.0);
                glDisable(GL_TEXTURE_2D);
                glDepthMask(GL_FALSE);
                uint8_t block_id = level_get_block(level, pos.x, pos.y, pos.z);
                block_t *block = &block_list[block_id];
                if(block_id != blocks.air.id) {
                    AABB_t bb = AABB_grow(block->get_selection_aabb(block, pos.x, pos.y, pos.z), 0.002, 0.002, 0.002);
                    glBegin(GL_LINE_STRIP);
                    glVertex3f(bb.x0, bb.y0, bb.z0);
                    glVertex3f(bb.x1, bb.y0, bb.z0);
                    glVertex3f(bb.x1, bb.y0, bb.z1);
                    glVertex3f(bb.x0, bb.y0, bb.z1);
                    glVertex3f(bb.x0, bb.y0, bb.z0);
                    glEnd();
                    glBegin(GL_LINE_STRIP);
                    glVertex3f(bb.x0, bb.y1, bb.z0);
                    glVertex3f(bb.x1, bb.y1, bb.z0);
                    glVertex3f(bb.x1, bb.y1, bb.z1);
                    glVertex3f(bb.x0, bb.y1, bb.z1);
                    glVertex3f(bb.x0, bb.y1, bb.z0);
                    glEnd();
                    glBegin(GL_LINES);
                    glVertex3f(bb.x0, bb.y0, bb.z0);
                    glVertex3f(bb.x0, bb.y1, bb.z0);
                    glVertex3f(bb.x1, bb.y0, bb.z0);
                    glVertex3f(bb.x1, bb.y1, bb.z0);
                    glVertex3f(bb.x1, bb.y0, bb.z1);
                    glVertex3f(bb.x1, bb.y1, bb.z1);
                    glVertex3f(bb.x0, bb.y0, bb.z1);
                    glVertex3f(bb.x0, bb.y1, bb.z1);
                    glEnd();
                }
                glDepthMask(GL_TRUE);
                glEnable(GL_TEXTURE_2D);
                glDisable(GL_BLEND);
                glEnable(GL_ALPHA_TEST);
            }

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            renderer_setup_fog(renderer);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "water.png"));
            glCallList(lrenderer->list_id + 1);
            glDisable(GL_BLEND);
            glEnable(GL_BLEND);
            glColorMask(0, 0, 0, 0);
            int count = level_renderer_sort_chunks(lrenderer, player, 1);
            glColorMask(1, 1, 1, 1);
            if(minecraft->settings.anaglyph) {
                if(i == 0) glColorMask(0, 1, 1, 0);
                else glColorMask(1, 0, 0, 0);
            }
            glDisable(GL_CULL_FACE);
            if(count > 0) {
                glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "terrain.png"));
                glCallLists(count, GL_INT, lrenderer->chunk_data_cache);
            }
            glEnable(GL_CULL_FACE);

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
            glDisable(GL_FOG);
            
            if(minecraft->raining) {
                float t = delta;
                int px = player->x;
                int py = player->y;
                int pz = player->z;
                glDisable(GL_CULL_FACE);
                glNormal3f(0, 1, 0);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "rain.png"));
                for(int x = px - 5; x <= px + 5; x++) {
                    for(int z = pz - 5; z <= pz + 5; z++) {
                        int y = level_get_highest_block(level, x, z);
                        int y_min = py - 5;
                        int y_max = py + 5;
                        if(y_min < y) y_min = y;
                        if(y_max < y) y_max = y;
                        if(y_min != y_max) {
                            float tt = (((lrenderer->ticks + x * 3121 + z * 418711) % 32) + t) / 32.0;
                            float d = sqrtf((x + 0.5 - player->x) * (x + 0.5 - player->x) + (z + 0.5 - player->z) * (z + 0.5 - player->z)) / 5.0;
                            glColor4f(1.0, 1.0, 1.0, (1.0 - d * d) * 0.7);
                            tesselator_begin();
                            tesselator_vertex_uv(x, y_min, z, 0.0, y_min / 4.0 + tt * 2.0);
                            tesselator_vertex_uv(x + 1, y_min, z + 1, 2.0, y_min / 4.0 + tt * 2.0);
                            tesselator_vertex_uv(x + 1, y_max, z + 1, 2.0, y_max / 4.0 + tt * 2.0);
                            tesselator_vertex_uv(x, y_max, z, 0.0, y_max / 4.0 + tt * 2.0);
                            tesselator_vertex_uv(x, y_min, z + 1, 0.0, y_min / 4.0 + tt * 2.0);
                            tesselator_vertex_uv(x + 1, y_min, z, 2.0, y_min / 4.0 + tt * 2.0);
                            tesselator_vertex_uv(x + 1, y_max, z, 2.0, y_max / 4.0 + tt * 2.0);
                            tesselator_vertex_uv(x, y_max, z + 1, 0.0, y_max / 4.0 + tt * 2.0);
                            tesselator_end();
                        }
                    }
                }
                glEnable(GL_CULL_FACE);
                glDisable(GL_BLEND);
            }
            glClear(GL_DEPTH_BUFFER_BIT);
            glLoadIdentity();
            if(minecraft->settings.anaglyph) {
                glTranslatef(((i << 1) - 1) * 0.1, 0.0, 0.0);
            }
            if(minecraft->settings.show_bobbing) {
                renderer_apply_bobbing(renderer, delta);
            }

            held_block_t held = renderer->held_block;
            float held_pos = held.last_position + (held.position - held.last_position) * delta;
            glPushMatrix();
            glRotatef(rot_x, 1.0, 0.0, 0.0);
            glRotatef(rot_y, 0.0, 1.0, 0.0);
            renderer_set_lighting(renderer, 1);
            glPopMatrix();
            glPushMatrix();
            if(held.moving) {
                float a = (held.offset + delta) / 7.0;
                glTranslatef(-tsin(sqrt(a) * M_PI) * 0.4, tsin(sqrt(a) * M_PI * 2.0) * 0.2, -tsin(a * M_PI) * 0.2);
            }
            glTranslatef(0.7 * 0.8, -0.65 * 0.8 - (1 - held_pos) * 0.6, -0.9 * 0.8);
            glRotatef(45, 0, 1, 0);
            glEnable(GL_NORMALIZE);
            if(held.moving) {
                float a = (held.offset + delta) / 7.0;
                glRotatef(tsin(sqrt(a) * M_PI) * 80.0, 0.0, 1.0, 0.0);
                glRotatef(-tsin(a * a * M_PI), 1.0, 0.0, 0.0);
            }
            float brightness = level_get_brightness(level, player->x, player->y, player->z);
            glColor4f(brightness, brightness, brightness, 1);
            if(held.block != NULL && held.block->id != blocks.air.id) {
                glScalef(0.4, 0.4, 0.4);
                glTranslatef(-0.5, -0.5, -0.5);
                glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "terrain.png"));
                held.block->render_preview(held.block);
            }else {
                glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "char.png"));
                glScalef(1, -1, 1);
                glTranslatef(0, 0.2, 0);
                glRotatef(-120, 0, 0, 1);
                glScalef(1, 1, 1);
                cube_t *arm = &minecraft->models.humanoid.arm1;
                if(!arm->compiled) {
                    cube_compile(arm, 0.0625);
                }
                glCallList(arm->list);
            }
            glDisable(GL_NORMALIZE);
            glPopMatrix();
            renderer_set_lighting(renderer, 0);

            if(!minecraft->settings.anaglyph) break;
            if(i == 1) glColorMask(1, 1, 1, 1);
        }

        screen_hud_render(&minecraft->hud, delta, mx, my);

        if(minecraft->current_screen != NULL) {
            minecraft->current_screen->render((struct screen_s *)minecraft->current_screen, mx, my);
        }
        
        SDL_GL_SwapWindow(minecraft->window);
        frame++;

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
    minecraft_create(&minecraft, 1920, 1080, 0);
    minecraft_run(&minecraft);
    
    if(minecraft.current_screen != NULL) {
        screen_destroy((struct screen_s *)minecraft.current_screen);
        free(minecraft.current_screen);
    }
    level_destroy(&minecraft.level);
    free(minecraft.textures.animations[0]); //water animated texture
    free(minecraft.textures.animations[1]); //lava animated texture
    textures_destroy(&minecraft.textures);
    level_renderer_destroy(&minecraft.level_renderer);
    particles_destroy(&minecraft.particles);
    array_list_free(minecraft.settings.bindings);
    array_list_free(session_allowed_blocks);
    sounds_destroy(&minecraft.sounds);
    
    SDL_DestroyWindow(minecraft.window);
    SDL_Quit();

    return 0;
}