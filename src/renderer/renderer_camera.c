#include <renderer/renderer_camera.h>

#include <minecraft.h>
#include <model/vec3.h>
#include <renderer/tesselator.h>
#include <world/block/blocks.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL_opengl.h>

#include <time.h>

//gets rid of annoying warning for windows static linking
extern void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

renderer_camera_t renderer_camera_create(minecraft_t *minecraft) {
    renderer_camera_t renderer_camera = { 0 };

    renderer_camera.minecraft = minecraft;
    renderer_camera.entity = NULL;
    renderer_camera.held_block = (held_block_t){ .minecraft = minecraft };
    renderer_camera.random = random_create(time(NULL));
    renderer_camera.fog_r = 0.0;
    renderer_camera.fog_g = 0.0;
    renderer_camera.fog_b = 0.0;

    return renderer_camera;
}

void renderer_camera_update(renderer_camera_t *renderer) {
    renderer->fog_fade_prev = renderer->fog_fade;
    float brightness = world_get_brightness(renderer->minecraft->world, floor_double(renderer->minecraft->player.x), floor_double(renderer->minecraft->player.y), floor_double(renderer->minecraft->player.z));
    float distance_factor = (3.0 - renderer->minecraft->settings.view_distance) / 3.0;
    brightness = brightness * (1.0 - distance_factor) + distance_factor;
    renderer->fog_fade += (brightness - renderer->fog_fade) * 0.1;
    renderer->ticks++;

    // TODO
}

vec3_t renderer_camera_get_player_vector(renderer_camera_t *renderer, float delta) {
    entity_t *player = &renderer->minecraft->player.mob.entity;
    return (vec3_t){ player->xo + (player->x - player->xo) * delta, player->yo + (player->y - player->yo) * delta, player->zo + (player->z - player->zo) * delta };
}

void renderer_camera_hurt_effect(renderer_camera_t *renderer, float delta) {
    mob_t *player = &renderer->minecraft->player.mob;
    float dtime = player->hurt_time - delta;
    if(player->hurt_time <= 0) {
        delta += player->death_time;
        glRotatef(40.0 - 8000.0 / (delta + 200.0), 0.0, 0.0, 1.0);
    }

    if(dtime >= 0.0) {
        dtime /= player->hurt_duration;
        dtime = tsin(dtime * dtime * dtime * dtime * M_PI);
        delta = player->hurt_dir;
        glRotatef(-delta, 0.0, 1.0, 0.0);
        glRotatef(-dtime * 14.0, 0.0, 0.0, 1.0);
        glRotatef(delta, 0.0, 1.0, 0.0);
    }
}

void renderer_camera_apply_bobbing(renderer_camera_t *renderer, float delta) {
    if(renderer->minecraft->settings.third_person) return;
    entity_t* entity = &renderer->minecraft->player.mob.entity;
    player_t *player = &renderer->minecraft->player;
    float walk = entity->walk_dist - entity->walk_disto;
    walk = entity->walk_dist + walk * delta;
    float bob = player->obob + (player->bob - player->obob) * delta;
    float tilt = player->o_tilt + (player->tilt - player->o_tilt) * delta;
    glTranslatef(tsin(walk * M_PI) * bob * 0.5, -fabs(tcos(walk * M_PI) * bob), 0.0);
    glRotatef(tsin(walk * M_PI) * bob * 3.0, 0.0, 0.0, 1.0);
    glRotatef(fabs(tcos(walk * M_PI + 0.2) * bob) * 5.0, 1.0, 0.0, 0.0);
    glRotatef(tilt, 1.0, 0.0, 0.0);
}

void renderer_camera_update_mouse(renderer_camera_t *renderer, float delta) {
    if(renderer->display_active && !(SDL_GetWindowFlags(renderer->minecraft->window) & SDL_WINDOW_INPUT_FOCUS)) {
        minecraft_pause(renderer->minecraft);
    }

    // TODO

    int mx = 0, my = 0;
    SDL_GetMouseState(&mx, &my);
    int x = renderer->minecraft->width;
    int y = renderer->minecraft->height;
    int w = x;
    int h = y;
    for(x = 1; w / (x + 1) >= 320 && h / (x + 1) >= 240; x++);
    w /= x;
    h /= x;
    mx = mx * w / renderer->minecraft->width;
    my = my * h / renderer->minecraft->height - 1;

    if(renderer->minecraft->world != NULL) {
        renderer_camera_update_camera(renderer, delta);
        glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4ub(255, 0, 255, 255);
        glOrtho(0.0, w, h, 0.0, 0.0, 1);
        renderer_camera_setup_gui(renderer);
        screen_hud_render(&renderer->minecraft->hud, mx, my, delta);
    }else {
        glViewport(0, 0, renderer->minecraft->frame_width, renderer->minecraft->frame_height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        renderer_camera_setup_gui(renderer);
    }

    if(renderer->minecraft->current_screen != NULL) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4ub(255, 0, 255, 255);
        glOrtho(0.0, w, h, 0.0, 0.0, 1);
        renderer->minecraft->current_screen->render((struct screen_s *)renderer->minecraft->current_screen, mx, my, delta);
    }

    SDL_Delay(0);
    SDL_GL_SwapWindow(renderer->minecraft->window);
}

void renderer_camera_update_camera(renderer_camera_t *renderer, float delta) {
    player_t *player = &renderer->minecraft->player;
    float rot_x = player->x_roto + (player->x_rot - player->x_roto) * delta;
    float rot_y = player->y_roto + (player->y_rot - player->y_roto) * delta;
    vec3_t v = renderer_camera_get_player_vector(renderer, delta);
    float c1 = tcos(-rot_y * M_PI / 180.0 - M_PI);
    float s1 = tsin(-rot_y * M_PI / 180.0 - M_PI);
    float c2 = -tcos(-rot_x * M_PI / 180.0);
    float s2 = tsin(-rot_x * M_PI / 180.0);
    float sc = s1 * c2;
    float cc = c1 * c2;
    float reach = renderer->minecraft->gamemode.reach;
    vec3_t v2 = { v.x + sc * reach, v.y + s2 * reach, v.z + cc * reach };
    renderer->minecraft->hit_result = world_clip(renderer->minecraft->world, v, v2);
    v = renderer_camera_get_player_vector(renderer, delta);
    if(!renderer->minecraft->hit_result.null) {
        reach = sqrtf(vec3_distance_to_sqr(v, renderer->minecraft->hit_result.location));
    }

    if(renderer->minecraft->gamemode.gamemode_type == GAMEMODE_CREATIVE) {
        reach = 32.0;
    }else {
        if(reach > 3.0) reach = 3.0;
    }

    renderer->entity = NULL;
    double a = 0.0;

    AABB_t box = AABB_expand(player->bb, sc * reach, s2 * reach, cc * reach);
    entity_t **entities = world_get_entities_excluding(renderer->minecraft->world, &player->entity, box);
    for(int i = 0; i < array_list_length(entities); i++) {
        /*entity_t *entity = *(entity_t **)array_list_get(entities, i);
        if(entity->can_be_hit(entity)) {
            float r = 0.1;
            float dist;
            hit_result_t pos = AABB_clip(AABB_grow(entity->bb, r, r, r), v, v2);
            if(!pos.null) dist = (vec3_distance_to(v, pos.location));
            if(!pos.null && (dist < a || a == 0.0)) {
                renderer->entity = entity;
                a = dist;
            }
        }*/
    }

    array_list_free(entities);

    if(renderer->entity != NULL && renderer->minecraft->gamemode.gamemode_type == GAMEMODE_SURVIVAL) {
        renderer->minecraft->hit_result = (hit_result_t){ .type = 1, .entity = renderer->entity };
    }

    world_t *world = renderer->minecraft->world;
    double dx = player->xo + (player->x - player->xo) * delta;
    double dy = player->yo + (player->y - player->yo) * delta;
    double dz = player->zo + (player->z - player->zo) * delta;

    for(int i = 0; i < 2; i++) {
        if(renderer->minecraft->settings.anaglyph) {
            if(i == 0) glColorMask(0, 1, 1, 0);
            else glColorMask(1, 0, 0, 0);
        }

        glViewport(0, 0, renderer->minecraft->frame_width, renderer->minecraft->frame_height);
        float a = 1.0 / (4 - renderer->minecraft->settings.view_distance);
        a = 1.0 - powf(a, 0.25);

        vec3_t sky_color = world_get_sky_color(world, delta);
        float sky_r = sky_color.x;
        float sky_g = sky_color.y;
        float sky_b = sky_color.z;

        vec3_t fog_color = world_get_fog_color(world, delta);
        renderer->fog_r = fog_color.x;
        renderer->fog_g = fog_color.y;
        renderer->fog_b = fog_color.z;
        renderer->fog_r = (renderer->fog_r + (sky_r - renderer->fog_r) * a);
        renderer->fog_g = (renderer->fog_g + (sky_g - renderer->fog_g) * a);
        renderer->fog_b = (renderer->fog_b + (sky_b - renderer->fog_b) * a);

        block_t *block = &block_list[world_get_block(world, floor_double(player->x), floor_double(player->y + 0.12), floor_double(player->z))];
        if(block->id != blocks.air.id && block->material != &materials.air) {
            if(block->material == &materials.water) {
                renderer->fog_r = 0.02;
                renderer->fog_g = 0.02;
                renderer->fog_b = 0.2;
            }
            if(block->material == &materials.lava) {
                renderer->fog_r = 0.6;
                renderer->fog_g = 0.1;
                renderer->fog_b = 0.0;
            }
        }

        float dfog = renderer->fog_fade_prev + (renderer->fog_fade - renderer->fog_fade_prev) * delta;
        renderer->fog_r *= dfog;
        renderer->fog_g *= dfog;
        renderer->fog_b *= dfog;

        if(renderer->minecraft->settings.anaglyph) {
            float ar = (renderer->fog_r * 30.0 + renderer->fog_g * 59.0 + renderer->fog_b * 11.0) / 100.0;
            float ag = (renderer->fog_r * 30.0 + renderer->fog_g * 70.0) / 100.0;
            float ab = (renderer->fog_r * 30.0 + renderer->fog_b * 70.0) / 100.0;
            renderer->fog_r = ar;
            renderer->fog_g = ag;
            renderer->fog_b = ab;
        }

        glClearColor(renderer->fog_r, renderer->fog_g, renderer->fog_b, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        renderer->far_plane_distance = (256 >> renderer->minecraft->settings.view_distance);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if(renderer->minecraft->settings.anaglyph) glTranslatef(-((i << 1) - 1) * 0.07, 0.0, 0.0);

        float fov = 70.0;
        /*
        if(entity_is_inside_material(player->mob.entity)) {
            fov = 60.0;
        }
        */
        if(player->health <= 0) {
            float ddeath_time = player->death_time + delta;
            fov /= (1.0 - 500.0 / (ddeath_time + 500.0)) * 2.0 + 1.0;
        }
        
        gluPerspective(fov, (float)renderer->minecraft->width / (float)renderer->minecraft->height, 0.05, renderer->far_plane_distance);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if(renderer->minecraft->settings.anaglyph) glTranslatef(((i << 1) - 1) * 0.1, 0.0, 0.0);

        //renderer_camera_hurt_effect(renderer, delta);
        if(renderer->minecraft->settings.show_bobbing) renderer_camera_apply_bobbing(renderer, delta);

        if(!renderer->minecraft->settings.third_person) {
            glTranslatef(0.0, 0.0, -0.1);
        }else {
            double reach = 4.0;
            double offset_x = -tsin(player->x_rot / 180.0 * M_PI) * tcos(player->y_rot / 180.0 * M_PI) * reach;
            double offset_z = tcos(player->x_rot / 180.0 * M_PI) * tcos(player->y_rot / 180.0 * M_PI) * reach;
            double offset_y = -tsin(player->y_rot / 180.0 * M_PI) * reach;

            for(int j = 0; j < 8; j++) {
                float step_x = ((j & 1) << 1) - 1;
                float step_y = ((j >> 1 & 1) << 1) - 1;
                float step_z = ((j >> 2 & 1) << 1) - 1;
                step_x *= 0.1;
                step_y *= 0.1;
                step_z *= 0.1;
                hit_result_t result = world_clip(world, (vec3_t){ dx + step_x, dy + step_y, dz + step_z }, 
                    (vec3_t){dx - offset_x + step_x + step_z, dy - offset_y + step_y, dz - offset_z + step_z});
                if(!result.null) {
                    double hit_distance = vec3_distance_to(result.location, (vec3_t){dx, dy, dz});
                    if(hit_distance < reach) {
                        reach = hit_distance;
                    }
                }
            }

            glTranslatef(0.0, 0.0, -reach);
        }

        float rot_x = player->x_roto + (player->x_rot - player->x_roto) * delta;
        float rot_y = player->y_roto + (player->y_rot - player->y_roto) * delta;
        glRotatef(rot_x, 1.0, 0.0, 0.0);
        glRotatef(rot_y, 0.0, 1.0, 0.0);
        frustum_t frustum = frustum_get();

        renderer_camera_setup_fog(renderer);
        glEnable(GL_FOG);
        //renderer_world_update_renderers(&renderer->minecraft->renderer_world, &player->mob.entity);
        renderer_world_draw_sky(&renderer->minecraft->renderer_world, delta);

        renderer_camera_setup_fog(renderer);
        frustum_set_position(&frustum, dx, dy, dz);
        renderer_world_update_frustum(&renderer->minecraft->renderer_world, &frustum);
        glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->minecraft->textures, "terrain.png"));
        renderer_world_update_renderers(&renderer->minecraft->renderer_world, &player->mob.entity);

        renderer_camera_setup_fog(renderer);
        glEnable(GL_FOG);
        glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->minecraft->textures, "terrain.png"));
        // render_helper_disable_standard_item_lighting
        renderer_world_sort_and_render(&renderer->minecraft->renderer_world, &player->mob.entity, 0, delta);

        int player_x = floor_double(player->x);
        int player_y = floor_double(player->y);
        int player_z = floor_double(player->z);

        if(world_is_solid(world, player_x, player_y, player_z)) {
            renderer_block_t block_renderer = renderer_block_create(world);

            for(int x = player_x - 1; x <= player_x + 1; x++) {
                for(int y = player_y - 1; y <= player_y + 1; y++) {
                    for(int z = player_z - 1; z <= player_z + 1; z++) {
                        uint8_t block_id = world_get_block(world, x, y, z);
                        if(block_id > 0) {
                            renderer_block_render_inside(&block_renderer, &block_list[block_id], x, y, z);
                        }
                    }
                }
            }
        }
        // render_helper_enable_standard_item_lighting
        glPushMatrix();
        renderer_world_update_entities(&renderer->minecraft->renderer_world, renderer_camera_get_player_vector(renderer, delta), &frustum, delta);
        // render lit particles
        glPopMatrix();
        // render_helper_disable_standard_item_lighting

        renderer_camera_setup_fog(renderer);
        // render particles

        if(!renderer->minecraft->hit_result.null /*&& entity_is_inside_block(player)*/) {
            glDisable(GL_ALPHA_TEST);
            renderer_world_draw_block_breaking(&renderer->minecraft->renderer_world, &player->mob.entity, &renderer->minecraft->hit_result, 0, NULL, delta);
            renderer_world_draw_selection_box(&renderer->minecraft->renderer_world, &player->mob.entity, &renderer->minecraft->hit_result, 0, delta);
            glEnable(GL_ALPHA_TEST);
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        renderer_camera_setup_fog(renderer);
        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glColorMask(0, 0, 0, 0);
        glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->minecraft->textures, "terrain.png"));
        int rendered_chunks = renderer_world_sort_and_render(&renderer->minecraft->renderer_world, &player->mob.entity, 1, delta);
        glColorMask(1, 1, 1, 1);

        if(renderer->minecraft->settings.anaglyph) {
            if(i == 0) {
                glColorMask(0, 1, 1, 0);
            }else {
                glColorMask(1, 0, 0, 0);
            }
        }

        if(rendered_chunks > 0) {
            glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->minecraft->textures, "terrain.png"));
            renderer_world_render_all_lists(&renderer->minecraft->renderer_world, 1, delta);
        }

        glDepthMask(1);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        if(!renderer->minecraft->hit_result.null /*&& !entity_is_inside_block(player)*/) {
            glDisable(GL_ALPHA_TEST);
            renderer_world_draw_block_breaking(&renderer->minecraft->renderer_world, &player->mob.entity, &renderer->minecraft->hit_result, 0, NULL, delta);
            renderer_world_draw_selection_box(&renderer->minecraft->renderer_world, &player->mob.entity, &renderer->minecraft->hit_result, 0, delta);
            glEnable(GL_ALPHA_TEST);
        }

        glDisable(GL_FOG);
        if(renderer->minecraft->raining) {
            float t = delta;
            int px = floor_double(player->x);
            int py = floor_double(player->y);
            int pz = floor_double(player->z);
            glDisable(GL_CULL_FACE);
            glNormal3f(0, 1, 0);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->minecraft->textures, "rain.png"));
            for(int x = px - 5; x <= px + 5; x++) {
                for(int z = pz - 5; z <= pz + 5; z++) {
                    int y_min = py - 5;
                    int y_max = py + 5;
                    // This is why rain particles only render at sea level
                    if(y_min < 64) y_min = 64;
                    if(y_max < 64) y_max = 64;

                    if(y_min != y_max) {
                        float tt = (((renderer->ticks + x * 3121 + z * 418711) % 32) + t) / 32.0;
                        float d = sqrtf((x + 0.5 - player->x) * (x + 0.5 - player->x) + (z + 0.5 - player->z) * (z + 0.5 - player->z)) / 5.0;
                        glColor4f(1.0, 1.0, 1.0, (1.0 - d * d) * 0.7);
                        tesselator_begin_quads();
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

        if(renderer->minecraft->settings.anaglyph) {
            glTranslatef(((i << 1) - 1) * 0.1, 0.0, 0.0);
        }

        glPushMatrix();
        renderer_camera_hurt_effect(renderer, delta);
        if(renderer->minecraft->settings.show_bobbing) {
            renderer_camera_apply_bobbing(renderer, delta);
        }

        if(!renderer->minecraft->settings.third_person) {
            // render item in first person
        }

        glPopMatrix();
        if(!renderer->minecraft->settings.third_person) {
            // item renderer render overlays
            renderer_camera_hurt_effect(renderer, delta);
        }

        if(renderer->minecraft->settings.show_bobbing) {
            renderer_camera_apply_bobbing(renderer, delta);
        }

        if(!renderer->minecraft->settings.anaglyph) {
            return;
        }
    }

    glColorMask(1, 1, 1, 0);
}

void renderer_camera_setup_gui(renderer_camera_t *renderer_camera) {
    int width = renderer_camera->minecraft->width * 240 / renderer_camera->minecraft->height;
    int height = renderer_camera->minecraft->height * 240 / renderer_camera->minecraft->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)width, (double)height, 0.0, 1000.0, 3000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -2000.0);
}

void renderer_camera_setup_fog(renderer_camera_t *renderer_camera) {
    world_t *world = renderer_camera->minecraft->world;
    player_t *player = &renderer_camera->minecraft->player;
    glFogfv(GL_FOG_COLOR, (float []){ renderer_camera->fog_r, renderer_camera->fog_g, renderer_camera->fog_b, 1.0 });
    glNormal3f(0.0, -1.0, 0.0);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    block_t *block = &block_list[world_get_block(world, player->x, player->y + 0.12, player->z)];
    if(block->id != blocks.air.id && block->material != &materials.air) {
        glFogi(GL_FOG_MODE, GL_EXP);
        if(block->material == &materials.water) {
            glFogf(GL_FOG_DENSITY, 0.1);
        }
        if(block->material == &materials.lava) {
            glFogf(GL_FOG_DENSITY, 2.0);
        }
    }else {
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, renderer_camera->far_plane_distance * 0.25);
        glFogf(GL_FOG_END, renderer_camera->far_plane_distance);
        if(glGetString(GL_EXTENSIONS) && strstr((const char *)glGetString(GL_EXTENSIONS), "GL_NV_fog_distance")) {
            glFogi(GL_FOG_DISTANCE_MODE_NV, GL_EYE_RADIAL_NV);
        }
    }
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT);
}