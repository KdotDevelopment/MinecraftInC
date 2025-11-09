#include <renderer/entity/renderer_entity.h>

#include <minecraft.h>
#include <renderer/entity/renderer_entity_manager.h>
#include <renderer/tesselator.h>
#include <renderer/textures.h>
#include <world/world.h>
#include <world/block/block.h>
#include <world/block/blocks.h>
#include <util/math_helper.h>

#include <SDL2/SDL_opengl.h>
#include <string.h>

extern block_t block_list[256];

renderer_entity_t renderer_entity_create() {
    renderer_entity_t renderer = { 0 };
    memset(&renderer, 0, sizeof(renderer_entity_t));

    renderer.shadow_size = 0.0;
    renderer.shadow_opaque = 1.0;

    renderer.render = renderer_entity_render;
    renderer.render_shadow = renderer_entity_render_shadow;
    
    return renderer;
}

void renderer_entity_render(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float rot_x, float partial_tick) {
    return;
}

void renderer_entity_render_shadow(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float partial_tick) {
    if(renderer->shadow_size <= 0.0f) {
        return;
    }
    double distance_sq = renderer_entity_manager_distance(renderer->manager, x, y, z);
    float shadow_alpha = (float)((1.0 - distance_sq / (256.0 * 256.0)) * (double)renderer->shadow_opaque);
    if(shadow_alpha <= 0.0f) {
        return;
    }

    double entity_x = x;
    double entity_y = y;
    double entity_z = z;
    
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->manager->world->minecraft->textures, "shadow.png"));
    world_t *world = renderer->manager->world;
    glDepthMask(GL_FALSE);
    
    float shadow_radius = renderer->shadow_size;
    int xx_min = floor_double(x - (double)shadow_radius);
    int xx_max = floor_double(x + (double)shadow_radius);
    int yy_min = floor_double(y - (double)shadow_radius);
    int yy_max = floor_double(y);
    int zz_min = floor_double(z - (double)shadow_radius);
    int zz_max = floor_double(z + (double)shadow_radius);
    
    for(int xx = xx_min; xx <= xx_max; xx++) {
        for(int yy = yy_min; yy <= yy_max; yy++) {
            for(int zz = zz_min; zz <= zz_max; zz++) {
                uint8_t block_id_below = world_get_block(world, xx, yy - 1, zz);
                if(block_id_below > 0) {
                    uint8_t light_value = world_get_block_light_value(world, xx, yy, zz);

                    if(light_value > 3) {
                        block_t *block_below = &block_list[block_id_below];
                        
                        double vertical_distance = entity_y - yy;
                        double shadow_intensity = (shadow_alpha - vertical_distance / 2.0) * 0.5 * world_get_brightness(world, xx, yy, zz);
                        
                        if(shadow_intensity >= 0.0) {
                            glColor4f(1.0f, 1.0f, 1.0f, shadow_intensity);
                            
                            double xx0 = xx + block_below->x0;
                            double xx1 = xx + block_below->x1;
                            double yy0 = yy + block_below->y1;
                            double zz0 = zz + block_below->z0;
                            double zz1 = zz + block_below->z1;
                            
                            float u0 = (entity_x - xx0) / 2.0 / shadow_radius + 0.5;
                            float u1 = (entity_x - xx1) / 2.0 / shadow_radius + 0.5;
                            float v0 = (entity_z - zz0) / 2.0 / shadow_radius + 0.5;
                            float v1 = (entity_z - zz1) / 2.0 / shadow_radius + 0.5;
                            
                            tesselator_begin_quads();
                            tesselator_vertex_uv(xx0, yy0, zz0, u0, v0);
                            tesselator_vertex_uv(xx0, yy0, zz1, u0, v1);
                            tesselator_vertex_uv(xx1, yy0, zz1, u1, v1);
                            tesselator_vertex_uv(xx1, yy0, zz0, u1, v0);
                            tesselator_end();
                        }
                    }
                }
            }
        }
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
    if(entity->fire > 0) {
        glDisable(GL_LIGHTING);
        
        int fire_texture_index = block_list[BLOCK_FIRE].texture_id;
        int fire_tex_x = (fire_texture_index & 15) << 4;
        int fire_tex_y = fire_texture_index & 240;
        
        float u0 = fire_tex_x / 256.0f;
        float u1 = (fire_tex_x + 15.99f) / 256.0f;
        float v0 = fire_tex_y / 256.0f;
        float v1 = (fire_tex_y + 15.99f) / 256.0f;
        
        glPushMatrix();
        glTranslatef(x, y, z);
        
        float fire_scale = entity->bb_width * 1.4f;
        glScalef(fire_scale, fire_scale, fire_scale);
        
        glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->manager->world->minecraft->textures, "terrain.png"));
        
        float fire_width = 1.0f;
        float fire_y_offset = 0.0f;
        float fire_layers_remaining = entity->bb_height / entity->bb_width;
        
        glRotatef(-renderer->manager->player_rot, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 0.4f + (fire_layers_remaining) * 0.02);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        
        tesselator_begin_quads();
        
        while(fire_layers_remaining > 0.0f) {
            tesselator_vertex_uv(fire_width - 0.5, 0.0 - fire_y_offset, 0.0, u1, v1);
            tesselator_vertex_uv(-0.5, 0.0 - fire_y_offset, 0.0, u0, v1);
            tesselator_vertex_uv(-0.5, 1.4 - fire_y_offset, 0.0, u0, v0);
            tesselator_vertex_uv(fire_width - 0.5, 1.4 - fire_y_offset, 0.0, u1, v0);
            
            fire_layers_remaining -= 1.0f;
            fire_y_offset += 1.0f;
            fire_width *= 0.9f;
            glTranslatef(0.0f, 0.0f, -0.04f);
        }
        
        tesselator_end();
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }
}