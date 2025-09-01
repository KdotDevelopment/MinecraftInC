#include <renderer/renderer_block.h>

#include <renderer/tesselator.h>
#include <world/block/block_fire.h>
#include <world/block/block.h>
#include <world/world.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL_opengl.h>

renderer_block_t renderer_block_create(world_t *world) {
    renderer_block_t renderer_block = { 0 };

    renderer_block.world = world;
    renderer_block.fixed_texture_id = -1;
    renderer_block.flip_texture = 0;

    return renderer_block;
}

void renderer_block_render_using_texture(renderer_block_t *renderer, block_t *block, int x, int y, int z, int override_texture) {
    renderer->fixed_texture_id = override_texture;
    renderer_block_render(renderer, block, x, y, z);
    renderer->fixed_texture_id = -1;
}

void renderer_block_render_inside(renderer_block_t *renderer, block_t *block, int x, int y, int z) {
    renderer->flip_texture = 1;
    renderer_block_render(renderer, block, x, y, z);
    renderer->flip_texture = 0;
}

uint8_t renderer_block_render_normal(renderer_block_t *renderer, block_t *block, int x, int y, int z) {
    float center_brightness = block->get_brightness(block, renderer->world, x, y, z);
    uint8_t is_rendered = 0;
    if(renderer->flip_texture || block->can_render_side(block, renderer->world, x, y - 1, z, 0)) {
        float brightness = block->get_brightness(block, renderer->world, x, y - 1, z);
        if(block->light_value > 0) {
            brightness = 1.0;
        }

        tesselator_color_float(brightness * 0.5, brightness * 0.5, brightness * 0.5, 1.0);
        renderer_block_render_bottom(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 0));
        is_rendered = 1;
    }

    if(renderer->flip_texture || block->can_render_side(block, renderer->world, x, y + 1, z, 1)) {
        float brightness = block->get_brightness(block, renderer->world, x, y + 1, z);
        if(block->x0 != 1.0 && !block->material->is_liquid) {
            brightness = center_brightness;
        }

        if(block->light_value > 0) {
            brightness = 1.0;
        }

        tesselator_color_float(brightness, brightness, brightness, 1.0);
        renderer_block_render_top(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 1));
        is_rendered = 1;
    }

    if(renderer->flip_texture || block->can_render_side(block, renderer->world, x, y, z - 1, 2)) {
        float brightness = block->get_brightness(block, renderer->world, x, y, z - 1);
        if(block->light_value > 0) {
            brightness = 1.0;
        }

        tesselator_color_float(brightness * 0.8, brightness * 0.8, brightness * 0.8, 1.0);
        renderer_block_render_north(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 2));
        is_rendered = 1;
    }

    if(renderer->flip_texture || block->can_render_side(block, renderer->world, x, y, z + 1, 3)) {
        float brightness = block->get_brightness(block, renderer->world, x, y, z + 1);
        if(block->light_value > 0) {
            brightness = 1.0;
        }

        tesselator_color_float(brightness * 0.8, brightness * 0.8, brightness * 0.8, 1.0);
        renderer_block_render_south(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 3));
        is_rendered = 1;
    }

    if(renderer->flip_texture || block->can_render_side(block, renderer->world, x - 1, y, z, 4)) {
        float brightness = block->get_brightness(block, renderer->world, x - 1, y, z);
        if(block->light_value > 0) {
            brightness = 1.0;
        }

        tesselator_color_float(brightness * 0.6, brightness * 0.6, brightness * 0.6, 1.0);
        renderer_block_render_west(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 4));
        is_rendered = 1;
    }

    if(renderer->flip_texture || block->can_render_side(block, renderer->world, x + 1, y, z, 5)) {
        float brightness = block->get_brightness(block, renderer->world, x + 1, y, z);
        if(block->light_value > 0) {
            brightness = 1.0;
        }

        tesselator_color_float(brightness * 0.6, brightness * 0.6, brightness * 0.6, 1.0);
        renderer_block_render_east(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 5));
        is_rendered = 1;
    }

    return is_rendered;
}

float private_not_water(renderer_block_t *renderer, int x, int y, int z) {
    return world_get_block_material(renderer->world, x, y, z) != &materials.water ? 1.0 : world_get_block_metadata(renderer->world, x, y, z) / 9.0;
}

// This render type (4) does not seem to have any blocks associated with it. Code included anyway.
uint8_t renderer_block_render_unknown(renderer_block_t *renderer, block_t *block, int x, int y, int z) {
    uint8_t is_rendered = 0;
    block_t new_block = *block;
    new_block.y1 = block->y1 - private_not_water(renderer, x, y, z);
    if(renderer->flip_texture || block->can_render_side(block, renderer->world, x, y - 1, z, 0)) {
        float brightness = block->get_brightness(block, renderer->world, x, y - 1, z);
        tesselator_color_float(brightness * 0.5, brightness * 0.5, brightness * 0.5, 1.0);
        renderer_block_render_bottom(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 0));
        is_rendered = 1;
    }

    if(renderer->flip_texture || block->can_render_side(block, renderer->world, x, y + 1, z, 1)) {
        float brightness = block->get_brightness(block, renderer->world, x, y + 1, z);
        tesselator_color_float(brightness, brightness, brightness, 1.0);
        renderer_block_render_top(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 1));
        is_rendered = 1;
    }

    new_block.y0 = block->y0 - private_not_water(renderer, x, y, z - 1);
    if(renderer->flip_texture || block->y1 > block->y0 || block->can_render_side(block, renderer->world, x, y, z - 1, 2)) {
        float brightness = block->get_brightness(block, renderer->world, x, y, z - 1);
        tesselator_color_float(brightness * 0.8, brightness * 0.8, brightness * 0.8, 1.0);
        renderer_block_render_north(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 2));
        is_rendered = 1;
    }

    new_block.y0 = block->y0 - private_not_water(renderer, x, y, z + 1);
    if(renderer->flip_texture || block->y1 > block->y0 || block->can_render_side(block, renderer->world, x, y, z + 1, 3)) {
        float brightness = block->get_brightness(block, renderer->world, x, y, z + 1);
        tesselator_color_float(brightness * 0.8, brightness * 0.8, brightness * 0.8, 1.0);
        renderer_block_render_south(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 3));
        is_rendered = 1;
    }

    new_block.y0 = block->y0 - private_not_water(renderer, x - 1, y, z);
    if(renderer->flip_texture || block->y1 > block->y0 || block->can_render_side(block, renderer->world, x - 1, y, z, 4)) {
        float brightness = block->get_brightness(block, renderer->world, x - 1, y, z);
        tesselator_color_float(brightness * 0.6, brightness * 0.6, brightness * 0.6, 1.0);
        renderer_block_render_west(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 4));
        is_rendered = 1;
    }

    new_block.y0 = block->y0 - private_not_water(renderer, x + 1, y, z);
    if(renderer->flip_texture || new_block.y1 > block->y0 || block->can_render_side(block, renderer->world, x + 1, y, z, 5)) {
        float brightness = block->get_brightness(block, renderer->world, x + 1, y, z);
        tesselator_color_float(brightness * 0.6, brightness * 0.6, brightness * 0.6, 1.0);
        renderer_block_render_east(renderer, block, x, y, z, block->get_texture(block, renderer->world, x, y, z, 5));
        is_rendered = 1;
    }

    return is_rendered;
}

void renderer_block_render_plant(renderer_block_t *renderer, block_t *block, int8_t metadata, double x, double y, double z) {
    int texture_id = block->get_texture_side_metadata(block, 0, metadata);
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;

    double x0 = x + 0.5 - 0.45;
    double x1 = x + 0.5 + 0.45;
    double z0 = z + 0.5 - 0.45;
    double z1 = z + 0.5 + 0.45;

    tesselator_vertex_uv(x0, y + 1.0, z0, u0, v0);
    tesselator_vertex_uv(x0, y, z0, u0, v1);
    tesselator_vertex_uv(x1, y, z1, u1, v1);
    tesselator_vertex_uv(x1, y + 1.0, z1, u1, v0);

    tesselator_vertex_uv(x1, y + 1.0, z1, u0, v0);
    tesselator_vertex_uv(x1, y, z1, u0, v1);
    tesselator_vertex_uv(x0, y, z0, u1, v1);
    tesselator_vertex_uv(x0, y + 1.0, z0, u1, v0);

    tesselator_vertex_uv(x0, y + 1.0, z1, u0, v0);
    tesselator_vertex_uv(x0, y, z1, u0, v1);
    tesselator_vertex_uv(x1, y, z0, u1, v1);
    tesselator_vertex_uv(x1, y + 1.0, z0, u1, v0);

    tesselator_vertex_uv(x1, y + 1.0, z0, u0, v0);
    tesselator_vertex_uv(x1, y, z0, u0, v1);
    tesselator_vertex_uv(x0, y, z1, u1, v1);
    tesselator_vertex_uv(x0, y + 1.0, z1, u1, v0);
}

void renderer_block_render_crops(renderer_block_t *renderer, block_t *block, int8_t metadata, double x, double y, double z) {
    int texture_id = block->get_texture_side_metadata(block, 0, metadata);
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;

    double x0 = x + 0.5 - 0.25;
    double x1 = x + 0.5 + 0.25;
    double z0 = z + 0.5 - 0.5;
    double z1 = z + 0.5 + 0.5;

    tesselator_vertex_uv(x0, y + 1.0, z0, u0, v0);
    tesselator_vertex_uv(x0, y, z0, u0, v1);
    tesselator_vertex_uv(x0, y, z1, u1, v1);
    tesselator_vertex_uv(x0, y + 1.0, z1, u1, v0);

    tesselator_vertex_uv(x0, y + 1.0, z1, u0, v0);
    tesselator_vertex_uv(x0, y, z1, u0, v1);
    tesselator_vertex_uv(x0, y, z0, u1, v1);
    tesselator_vertex_uv(x0, y + 1.0, z0, u1, v0);

    tesselator_vertex_uv(x1, y + 1.0, z1, u0, v0);
    tesselator_vertex_uv(x1, y, z1, u0, v1);
    tesselator_vertex_uv(x1, y, z0, u1, v1);
    tesselator_vertex_uv(x1, y + 1.0, z0, u1, v0);

    tesselator_vertex_uv(x1, y + 1.0, z0, u0, v0);
    tesselator_vertex_uv(x1, y, z0, u0, v1);
    tesselator_vertex_uv(x1, y, z1, u1, v1);
    tesselator_vertex_uv(x1, y + 1.0, z1, u1, v0);

    x0 = x + 0.5 - 0.5;
    x1 = x + 0.5 + 0.5;
    z0 = z + 0.5 - 0.25;
    z1 = z + 0.5 + 0.25;

    tesselator_vertex_uv(x0, y + 1.0, z0, u0, v0);
    tesselator_vertex_uv(x0, y, z0, u0, v1);
    tesselator_vertex_uv(x1, y, z0, u1, v1);
    tesselator_vertex_uv(x1, y + 1.0, z0, u1, v0);

    tesselator_vertex_uv(x1, y + 1.0, z0, u0, v0);
    tesselator_vertex_uv(x1, y, z0, u0, v1);
    tesselator_vertex_uv(x0, y, z0, u1, v1);
    tesselator_vertex_uv(x0, y + 1.0, z0, u1, v0);

    tesselator_vertex_uv(x1, y + 1.0, z1, u0, v0);
    tesselator_vertex_uv(x1, y, z1, u0, v1);
    tesselator_vertex_uv(x0, y, z1, u1, v1);
    tesselator_vertex_uv(x0, y + 1.0, z1, u1, v0);

    tesselator_vertex_uv(x0, y + 1.0, z1, u0, v0);
    tesselator_vertex_uv(x0, y, z1, u0, v1);
    tesselator_vertex_uv(x1, y, z1, u1, v1);
    tesselator_vertex_uv(x1, y + 1.0, z1, u1, v0);
}

void renderer_block_render_torch(renderer_block_t *renderer, block_t *block, double x, double y, double z, float slope_x, float slope_z) {
    int texture_id = block->get_texture_side(block, 0);
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    float texture_u0 = texture_x / 256.0;
    float texture_u1 = (texture_x + 15.99) / 256.0;
    float texture_v0 = texture_y / 256.0;
    float texture_v1 = (texture_y + 15.99) / 256.0;

    double u0 = texture_u0 + 1.75 / 64.0;
    double v0 = texture_v0 + 6.0 / 256.0;
    double u1 = texture_u1 + 9.0 / 256.0;
    double v1 = texture_v1 + 1.0 / 32.0;

    x += 0.5;
    z += 0.5;
    double x0 = x - 0.5;
    double x1 = x + 0.5;
    double z0 = z - 0.5;
    double z1 = z + 0.5;

    // Torch top
    tesselator_vertex_uv(x + slope_x * 0.375 - 1.0 / 16.0, y + 0.625, z + slope_z * 0.375 - 1.0 / 16.0, u0, v0);
    tesselator_vertex_uv(x + slope_x * 0.375 - 1.0 / 16.0, y + 0.625, z + slope_z * 0.375 + 1.0 / 16.0, u0, v1);
    tesselator_vertex_uv(x + slope_x * 0.375 + 1.0 / 16.0, y + 0.625, z + slope_z * 0.375 + 1.0 / 16.0, u1, v1);
    tesselator_vertex_uv(x + slope_x * 0.375 + 1.0 / 16.0, y + 0.625, z + slope_z * 0.375 - 1.0 / 16.0, u1, v0);

    // Torch sides
    tesselator_vertex_uv(x - 1.0 / 16.0, y + 1.0, z0 - 1.0 / 16.0, u0, v0);
    tesselator_vertex_uv(x - 1.0 / 16.0 + slope_x, y, z0 - 1.0 / 16.0 + slope_z, u0, v1);
    tesselator_vertex_uv(x - 1.0 / 16.0 + slope_x, y, z1 + 1.0 / 16.0 + slope_z, u1, v1);
    tesselator_vertex_uv(x - 1.0 / 16.0, y + 1.0, z1 + 1.0 / 16.0, u1, v0);

    tesselator_vertex_uv(x + 1.0 / 16.0, y + 1.0, z1 + 1.0 / 16.0, u1, v0);
    tesselator_vertex_uv(x + slope_x + 1.0 / 16.0, y, z1 + 1.0 / 16.0 + slope_z, u1, v1);
    tesselator_vertex_uv(x + slope_x + 1.0 / 16.0, y, z0 - 1.0 / 16.0 + slope_z, u0, v1);
    tesselator_vertex_uv(x + 1.0 / 16.0, y + 1.0, z0 - 1.0 / 16.0, u0, v0);

    tesselator_vertex_uv(x0, y + 1.0, z + 1.0 / 16.0, u0, v0);
    tesselator_vertex_uv(x0 + slope_x, y, z + 1.0 / 16.0 + slope_z, u0, v1);
    tesselator_vertex_uv(x1 + slope_x, y, z + 1.0 / 16.0 + slope_z, u1, v1);
    tesselator_vertex_uv(x1, y + 1.0, z + 1.0 / 16.0, u1, v0);

    tesselator_vertex_uv(x1, y + 1.0, z - 1.0 / 16.0, u0, v0);
    tesselator_vertex_uv(x1 + slope_x, y, z - 1.0 / 16.0 + slope_z, u0, v1);
    tesselator_vertex_uv(x0 + slope_x, y, z - 1.0 / 16.0 + slope_z, u1, v1);
    tesselator_vertex_uv(x0, y + 1.0, z - 1.0 / 16.0, u1, v0);
}

void renderer_block_render_fire(renderer_block_t *renderer, block_t *block, double x, double y, double z) {
    int texture_id = block->get_texture_side(block, 0);
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;

    if(!world_is_solid(renderer->world, x, y - 1, z) && !block_fire_can_catch_fire(renderer->world, x, y - 1, z)) {
        if((int)(x + y + z) & 1) {
            u0 = (double)texture_x / 256.0;
            u1 = (double)(texture_x + 15.99) / 256.0;
            v0 = (double)(texture_y + 16) / 256.0;
            v1 = (double)(texture_y + 15.99 + 16.0) / 256.0;
        }

        if((int)(x / 2 + y / 2 + z / 2) & 1) {
            double temp = u1;
            u1 = u0;
            u0 = temp;
        }

        if(block_fire_can_catch_fire(renderer->world, x - 1, y, z)) {
            tesselator_vertex_uv(x + 0.2, y + 1.4 + 1.0 / 16.0, z + 1, u1, v0);
            tesselator_vertex_uv(x, y + 1.0 / 16.0, z + 1, u1, v1);
            tesselator_vertex_uv(x, y + 1.0 / 16.0, z, u0, v1);
            tesselator_vertex_uv(x + 0.2, y + 1.4 + 1.0 / 16.0, z, u0, v0);

            tesselator_vertex_uv(x + 0.2, y + 1.4 + 1.0 / 16.0, z, u0, v0);
            tesselator_vertex_uv(x, y + 1.0 / 16.0, z, u0, v1);
            tesselator_vertex_uv(x, y + 1.0 / 16.0, z + 1, u1, v1);
            tesselator_vertex_uv(x + 0.2, y + 1.4 + 1.0 / 16.0, z + 1, u1, v0);
        }

        if(block_fire_can_catch_fire(renderer->world, x + 1, y, z)) {
            tesselator_vertex_uv(x + 1 - 0.2, y + 1.4 + 1.0 / 16.0, z, u0, v0);
            tesselator_vertex_uv(x + 1, y + 1.0 / 16.0, z, u0, v1);
            tesselator_vertex_uv(x + 1, y + 1.0 / 16.0, z + 1, u1, v1);
            tesselator_vertex_uv(x + 1 - 0.2, y + 1.4 + 1.0 / 16.0, z + 1, u1, v0);

            tesselator_vertex_uv(x + 1 - 0.2, y + 1.4 + 1.0 / 16.0, z + 1, u1, v0);
            tesselator_vertex_uv(x + 1, y + 1.0 / 16.0, z + 1, u1, v1);
            tesselator_vertex_uv(x + 1, y + 1.0 / 16.0, z, u0, v1);
            tesselator_vertex_uv(x + 1 - 0.2, y + 1.4 + 1.0 / 16.0, z, u0, v0);
        }

        if(block_fire_can_catch_fire(renderer->world, x, y, z - 1)) {
            tesselator_vertex_uv(x, y + 1.4 + 1.0 / 16.0, z + 0.2, u1, v0);
            tesselator_vertex_uv(x, y + 1.0 / 16.0, z, u1, v1);
            tesselator_vertex_uv(x + 1, y + 1.0 / 16.0, z, u0, v1);
            tesselator_vertex_uv(x + 1, y + 1.4 + 1.0 / 16.0, z + 0.2, u0, v0);

            tesselator_vertex_uv(x + 1, y + 1.4 + 1.0 / 16.0, z + 0.2, u0, v0);
            tesselator_vertex_uv(x + 1, y + 1.0 / 16.0, z, u0, v1);
            tesselator_vertex_uv(x, y + 1.0 / 16.0, z, u1, v1);
            tesselator_vertex_uv(x, y + 1.4 + 1.0 / 16.0, z + 0.2, u1, v0);
        }

        if(block_fire_can_catch_fire(renderer->world, x, y, z + 1)) {
            tesselator_vertex_uv(x + 1, y + 1.4 + 1.0 / 16.0, z + 1 - 0.2, u0, v0);
            tesselator_vertex_uv(x + 1, y + 1.0 / 16.0, z + 1, u0, v1);
            tesselator_vertex_uv(x, y + 1.0 / 16.0, z + 1, u1, v1);
            tesselator_vertex_uv(x, y + 1.4 + 1.0 / 16.0, z + 1 - 0.2, u1, v0);

            tesselator_vertex_uv(x, y + 1.4 + 1.0 / 16.0, z + 1 - 0.2, u1, v0);
            tesselator_vertex_uv(x, y + 1.0 / 16.0, z + 1, u1, v1);
            tesselator_vertex_uv(x + 1, y + 1.0 / 16.0, z + 1, u0, v1);
            tesselator_vertex_uv(x + 1, y + 1.4 + 1.0 / 16.0, z + 1 - 0.2, u0, v0);
        }

        if(block_fire_can_catch_fire(renderer->world, x, y + 1, z)) {
            double x0 = x + 0.5 - 0.5;
            double x1 = x + 0.5 + 0.5;
            double z0 = z + 0.5 - 0.5;
            double z1 = z + 0.5 + 0.5;
            u0 = (double)texture_x / 256.0;
            u1 = (double)(texture_x + 15.99) / 256.0;
            v0 = (double)texture_y / 256.0;
            v1 = (double)(texture_y + 15.99) / 256.0;
            y++;
            if((int)(x + y + z) & 1) {
                tesselator_vertex_uv(x0, y - 0.2, z, u1, v0);
                tesselator_vertex_uv(x1, y, z, u1, v1);
                tesselator_vertex_uv(x1, y, z + 1, u0, v1);
                tesselator_vertex_uv(x0, y - 0.2, z + 1, u0, v0);
                u0 = (double)texture_x / 256.0;
                u1 = (double)(texture_x + 15.99) / 256.0;
                v0 = (double)(texture_y + 16) / 256.0;
                v1 = (double)(texture_y + 15.99 + 16.0) / 256.0;
                tesselator_vertex_uv(x1, y - 0.2, z + 1, u1, v0);
                tesselator_vertex_uv(x0, y, z + 1, u1, v1);
                tesselator_vertex_uv(x0, y, z, u0, v1);
                tesselator_vertex_uv(x1, y - 0.2, z, u0, v0);
            }else {
                tesselator_vertex_uv(x, y - 0.2, z, z1, v0);
                tesselator_vertex_uv(x, y, z0, u0, v1);
                tesselator_vertex_uv(x + 1, y, z0, u1, v1);
                tesselator_vertex_uv(x + 1, y - 0.2, z1, u1, v0);
                u0 = (double)texture_x / 256.0;
                u1 = (double)(texture_x + 15.99) / 256.0;
                v0 = (double)(texture_y + 16) / 256.0;
                v1 = (double)(texture_y + 15.99 + 16.0) / 256.0;
                tesselator_vertex_uv(x + 1, y - 0.2, z1, u0, v0);
                tesselator_vertex_uv(x + 1, y, z0, u0, v1);
                tesselator_vertex_uv(x, y, z0, u1, v1);
                tesselator_vertex_uv(x, y - 0.2, z1, u1, v0);
            }
        }
    }else {
        double x0 = x + 0.5 + 0.2;
        double x1 = x + 0.5 - 0.2;
        double z0 = z + 0.5 + 0.2;
        double z1 = z + 0.5 - 0.2;
        double x2 = x + 0.5 - 0.3;
        double x3 = x + 0.5 + 0.3;
        double z2 = z + 0.5 - 0.3;
        double z3 = z + 0.5 + 0.3;

        tesselator_vertex_uv(x2, y + 1.4, z + 1, u1, v0);
        tesselator_vertex_uv(x0, y, z + 1, u1, v1);
        tesselator_vertex_uv(x0, y, z, u0, v1);
        tesselator_vertex_uv(x2, y + 1.4, z, u0, v0);

        tesselator_vertex_uv(x3, y + 1.4, z, u1, v0);
        tesselator_vertex_uv(x1, y, z, u1, v1);
        tesselator_vertex_uv(x1, y, z + 1, u0, v1);
        tesselator_vertex_uv(x3, y + 1.4, z + 1, u0, v0);

        u0 = (double)texture_x / 256.0;
        u1 = (double)(texture_x + 15.99) / 256.0;
        v0 = (double)(texture_y + 16) / 256.0;
        v1 = (double)(texture_y + 15.99 + 16.0) / 256.0;

        tesselator_vertex_uv(x + 1, y + 1.4, z3, u1, v0);
        tesselator_vertex_uv(x + 1, y, z1, u1, v1);
        tesselator_vertex_uv(x, y, z1, u0, v1);
        tesselator_vertex_uv(x, y + 1.4, z3, u0, v0);

        tesselator_vertex_uv(x, y + 1.4, z2, u1, v0);
        tesselator_vertex_uv(x, y, z0, u1, v1);
        tesselator_vertex_uv(x + 1, y, z0, u0, v1);
        tesselator_vertex_uv(x + 1, y + 1.4, z2, u0, v0);

        x0 = x + 0.5 - 0.5;
        x1 = x + 0.5 + 0.5;
        z0 = z + 0.5 - 0.5;
        z1 = z + 0.5 + 0.5;
        x2 = x + 0.5 - 0.4;
        x3 = x + 0.5 + 0.4;
        z2 = z + 0.5 - 0.4;
        z3 = z + 0.5 + 0.4;

        tesselator_vertex_uv(x2, y + 1.4, z, u0, v0);
        tesselator_vertex_uv(x0, y, z, u0, v1);
        tesselator_vertex_uv(x0, y, z + 1, u1, v1);
        tesselator_vertex_uv(x2, y + 1.4, z + 1, u1, v0);

        tesselator_vertex_uv(x3, y + 1.4, z + 1, u0, v0);
        tesselator_vertex_uv(x1, y, z + 1, u0, v1);
        tesselator_vertex_uv(x1, y, z, u1, v1);
        tesselator_vertex_uv(x3, y + 1.4, z, u1, v0);

        tesselator_vertex_uv(x, y + 1.4, z3, u1, v0);
        tesselator_vertex_uv(x, y, z1, u1, v1);
        tesselator_vertex_uv(x + 1, y, z1, u0, v1);
        tesselator_vertex_uv(x + 1, y + 1.4, z3, u0, v0);

        tesselator_vertex_uv(x + 1, y + 1.4, z2, u1, v0);
        tesselator_vertex_uv(x + 1, y, z0, u1, v1);
        tesselator_vertex_uv(x, y, z0, u0, v1);
        tesselator_vertex_uv(x, y + 1.4, z2, u0, v0);
    }
}

void renderer_block_render_gears(renderer_block_t *renderer, block_t *block, double x, double y, double z) {
    int texture_id = block->get_texture_side(block, 0);
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    float brightness = block->get_brightness(block, renderer->world, x, y, z);
    tesselator_color_float(brightness, brightness, brightness, 1.0);

    int texture_x = ((texture_id & 15) << 4) + 16;
    int texture_x_alt = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;

    if((int)(x + y + z) & 1) {
        texture_x = (texture_id & 15) << 4;
        texture_x_alt = ((texture_id & 15) << 4) + 16;
    }

    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;

    double u0_alt = (double)texture_x_alt / 256.0;
    double u1_alt = (double)(texture_x_alt + 15.99) / 256.0;

    if(world_is_solid(renderer->world, x - 1, y, z)) {
        tesselator_vertex_uv(x + 0.05, y + 1.0 + 2.0 / 16.0, z + 1.0 + 2.0 / 16.0, u0, v0);
        tesselator_vertex_uv(x + 0.05, y - 2.0 / 16.0, z + 1.0 + 2.0 / 16.0, u0, v1);
        tesselator_vertex_uv(x + 0.05, y - 2.0 / 16.0, z - 2.0 / 16.0, u1, v1);
        tesselator_vertex_uv(x + 0.05, y + 1.0 + 2.0 / 16.0, z - 2.0 / 16.0, u1, v0);
    }

    if(world_is_solid(renderer->world, x + 1, y, z)) {
        tesselator_vertex_uv(x + 1.0 - 0.05, y - 2.0 / 16.0, z + 1.0 + 2.0 / 16.0, u1, v1);
        tesselator_vertex_uv(x + 1.0 - 0.05, y + 1.0 + 2.0 / 16.0, z + 1.0 + 2.0 / 16.0, u1, v0);
        tesselator_vertex_uv(x + 1.0 - 0.05, y + 1.0 + 2.0 / 16.0, z - 2.0 / 16.0, u0, v0);
        tesselator_vertex_uv(x + 1.0 - 0.05, y - 2.0 / 16.0, z - 2.0 / 16.0, u0, v1);
    }

    if(world_is_solid(renderer->world, x, y, z - 1)) {
        tesselator_vertex_uv(x + 1.0 + 2.0 / 16.0, y - 2.0 / 16.0, z + 0.05, u1_alt, v1);
        tesselator_vertex_uv(x + 1.0 + 2.0 / 16.0, y + 1.0 + 2.0 / 16.0, z + 0.05, u1_alt, v0);
        tesselator_vertex_uv(x - 2.0 / 16.0, y + 1.0 + 2.0 / 16.0, z + 0.05, u0_alt, v0);
        tesselator_vertex_uv(x - 2.0 / 16.0, y - 2.0 / 16.0, z + 0.05, u0_alt, v1);
    }

    if(world_is_solid(renderer->world, x, y, z + 1)) {
        tesselator_vertex_uv(x + 1.0 + 2.0 / 16.0, y + 1.0 + 2.0 / 16.0, z + 1.0 - 0.05, u0_alt, v0);
        tesselator_vertex_uv(x + 1.0 + 2.0 / 16.0, y - 2.0 / 16.0, z + 1.0 - 0.05, u0_alt, v1);
        tesselator_vertex_uv(x - 2.0 / 16.0, y - 2.0 / 16.0, z + 1.0 - 0.05, u1_alt, v1);
        tesselator_vertex_uv(x - 2.0 / 16.0, y + 1.0 + 2.0 / 16.0, z + 1.0 - 0.05, u1_alt, v0);
    }
}

uint8_t renderer_block_render(renderer_block_t *renderer, block_t *block, int x, int y, int z) {
    int render_type = block->render_type;
    if(render_type == BLOCK_RENDER_TYPE_NORMAL) {
        return renderer_block_render_normal(renderer, block, x, y, z);
    }
    if(render_type == 4) {
        return renderer_block_render_unknown(renderer, block, x, y, z);
    }
    if(render_type == BLOCK_RENDER_TYPE_FLOWER) {
        float brightness = block->get_brightness(block, renderer->world, x, y, z);
        tesselator_color_float(brightness, brightness, brightness, 1.0);
        renderer_block_render_plant(renderer, block, world_get_block_metadata(renderer->world, x, y, z), x, y, z);
        return 1;
    }
    if(render_type == BLOCK_RENDER_TYPE_CROPS) {
        float brightness = block->get_brightness(block, renderer->world, x, y, z);
        tesselator_color_float(brightness, brightness, brightness, 1.0);
        renderer_block_render_crops(renderer, block, world_get_block_metadata(renderer->world, x, y, z), x, (double)y - 15.0 / 16.0, z);
        return 1;
    }
    if(render_type == BLOCK_RENDER_TYPE_TORCH) {
        uint8_t metadata = world_get_block_metadata(renderer->world, x, y, z);
        float brightness = block->get_brightness(block, renderer->world, x, y, z);
        if(block->light_value > 0) {
            brightness = 1.0;
        }
        tesselator_color_float(brightness, brightness, brightness, 1.0);
        if(metadata == 1) {
            renderer_block_render_torch(renderer, block, x - 0.099999994, y + 0.2, z, -0.4, 0);
        }else if(metadata == 2) {
            renderer_block_render_torch(renderer, block, x + 0.099999994, y + 0.2, z, 0.4, 0);
        }else if(metadata == 3) {
            renderer_block_render_torch(renderer, block, x, y + 0.2, z - 0.099999994, 0, -0.4);
        }else if(metadata == 4) {
            renderer_block_render_torch(renderer, block, x, y + 0.2, z + 0.099999994, 0, 0.4);
        }else {
            renderer_block_render_torch(renderer, block, x, y, z, 0, 0);
        }

        return 1;
    }
    if(render_type == BLOCK_RENDER_TYPE_FIRE) {
        renderer_block_render_fire(renderer, block, x, y, z);
        return 1;
    }
    if(render_type == BLOCK_RENDER_TYPE_GEARS) {
        renderer_block_render_gears(renderer, block, x, y, z);
        return 1;
    }

    return 0;
}

void renderer_block_render_bottom(renderer_block_t *renderer, block_t *block, double x, double y, double z, int texture_id) {
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;

    double x0 = x + block->x0;
    double x1 = x + block->x1;
    double y0 = y + block->y0;
    double z0 = z + block->z0;
    double z1 = z + block->z1;

    tesselator_vertex_uv(x0, y0, z1, u0, v1);
    tesselator_vertex_uv(x0, y0, z0, u0, v0);
    tesselator_vertex_uv(x1, y0, z0, u1, v0);
    tesselator_vertex_uv(x1, y0, z1, u1, v1);
}

void renderer_block_render_top(renderer_block_t *renderer, block_t *block, double x, double y, double z, int texture_id) {
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;

    double x0 = x + block->x0;
    double x1 = x + block->x1;
    double y1 = y + block->y1;
    double z0 = z + block->z0;
    double z1 = z + block->z1;

    tesselator_vertex_uv(x1, y1, z1, u1, v1);
    tesselator_vertex_uv(x1, y1, z0, u1, v0);
    tesselator_vertex_uv(x0, y1, z0, u0, v0);
    tesselator_vertex_uv(x0, y1, z1, u0, v1);
}

void renderer_block_render_north(renderer_block_t *renderer, block_t *block, double x, double y, double z, int texture_id) {
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;
    if(block->y0 >= 0.0 && block->y1 <= 1.0) {
        v0 = (double)(texture_y + block->y0 * 15.99) / 256.0;
        v1 = (double)(texture_y + block->y1 * 15.99) / 256.0;
    }

    double x0 = x + block->x0;
    double x1 = x + block->x1;
    double y0 = y + block->y0;
    double y1 = y + block->y1;
    double z0 = z + block->z0;

    tesselator_vertex_uv(x0, y1, z0, u1, v0);
    tesselator_vertex_uv(x1, y1, z0, u0, v0);
    tesselator_vertex_uv(x1, y0, z0, u0, v1);
    tesselator_vertex_uv(x0, y0, z0, u1, v1);
}

void renderer_block_render_south(renderer_block_t *renderer, block_t *block, double x, double y, double z, int texture_id) {
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;
    if(block->y0 >= 0.0 && block->y1 <= 1.0) {
        v0 = (double)(texture_y + block->y0 * 15.99) / 256.0;
        v1 = (double)(texture_y + block->y1 * 15.99) / 256.0;
    }

    double x0 = x + block->x0;
    double x1 = x + block->x1;
    double y0 = y + block->y0;
    double y1 = y + block->y1;
    double z1 = z + block->z1;

    tesselator_vertex_uv(x0, y1, z1, u0, v0);
    tesselator_vertex_uv(x0, y0, z1, u0, v1);
    tesselator_vertex_uv(x1, y0, z1, u1, v1);
    tesselator_vertex_uv(x1, y1, z1, u1, v0);
}

void renderer_block_render_west(renderer_block_t *renderer, block_t *block, double x, double y, double z, int texture_id) {
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;
    if(block->y0 >= 0.0 && block->y1 <= 1.0) {
        v0 = (double)(texture_y + block->y0 * 15.99) / 256.0;
        v1 = (double)(texture_y + block->y1 * 15.99) / 256.0;
    }

    double x0 = x + block->x0;
    double y0 = y + block->y0;
    double y1 = y + block->y1;
    double z0 = z + block->z0;
    double z1 = z + block->z1;

    tesselator_vertex_uv(x0, y1, z1, u1, v0);
    tesselator_vertex_uv(x0, y1, z1, u0, v0);
    tesselator_vertex_uv(x0, y0, z0, u0, v1);
    tesselator_vertex_uv(x0, y0, z0, u1, v1);
}

void renderer_block_render_east(renderer_block_t *renderer, block_t *block, double x, double y, double z, int texture_id) {
    if(renderer->fixed_texture_id >= 0) {
        texture_id = renderer->fixed_texture_id;
    }

    int texture_x = (texture_id & 15) << 4;
    int texture_y = texture_id & 240;
    double u0 = (double)texture_x / 256.0;
    double u1 = (double)(texture_x + 15.99) / 256.0;
    double v0 = (double)texture_y / 256.0;
    double v1 = (double)(texture_y + 15.99) / 256.0;
    if(block->y0 >= 0.0 && block->y1 <= 1.0) {
        v0 = (double)(texture_y + block->y0 * 15.99) / 256.0;
        v1 = (double)(texture_y + block->y1 * 15.99) / 256.0;
    }

    double x1 = x + block->x1;
    double y0 = y + block->y0;
    double y1 = y + block->y1;
    double z0 = z + block->z0;
    double z1 = z + block->z1;

    tesselator_vertex_uv(x1, y0, z1, u0, v1);
    tesselator_vertex_uv(x1, y0, z0, u1, v1);
    tesselator_vertex_uv(x1, y1, z0, u1, v0);
    tesselator_vertex_uv(x1, y1, z1, u0, v0);
}

void renderer_block_render_in_inventory(renderer_block_t *renderer, block_t *block) {
    int render_type = block->render_type;
    if(render_type == BLOCK_RENDER_TYPE_NORMAL) {
        glTranslatef(-0.5, -0.5, -0.5);
        tesselator_begin_quads();
        tesselator_normal(0, -1.0, 0);
        renderer_block_render_bottom(renderer, block, 0, 0, 0, block->get_texture_side(block, 0));
        tesselator_end();

        tesselator_begin_quads();
        tesselator_normal(0, 1.0, 0);
        renderer_block_render_top(renderer, block, 0, 0, 0, block->get_texture_side(block, 1));
        tesselator_end();

        tesselator_begin_quads();
        tesselator_normal(0, 0, -1.0);
        renderer_block_render_north(renderer, block, 0, 0, 0, block->get_texture_side(block, 2));
        tesselator_end();

        tesselator_begin_quads();
        tesselator_normal(0, 0, 1.0);
        renderer_block_render_south(renderer, block, 0, 0, 0, block->get_texture_side(block, 3));
        tesselator_end();

        tesselator_begin_quads();
        tesselator_normal(-1.0, 0, 0);
        renderer_block_render_west(renderer, block, 0, 0, 0, block->get_texture_side(block, 4));
        tesselator_end();

        tesselator_begin_quads();
        tesselator_normal(1.0, 0, 0);
        renderer_block_render_east(renderer, block, 0, 0, 0, block->get_texture_side(block, 5));
        tesselator_end();
        glTranslatef(0.5, 0.5, 0.5);
    }
    if(render_type == BLOCK_RENDER_TYPE_FLOWER) {
        tesselator_begin_quads();
        tesselator_normal(0, -1.0, 0);
        renderer_block_render_plant(renderer, block, -1, -0.5, -0.5, -0.5);
        tesselator_end();
    }
    if(render_type == BLOCK_RENDER_TYPE_CROPS) {
        tesselator_begin_quads();
        tesselator_normal(0, -1.0, 0);
        renderer_block_render_crops(renderer, block, -1, -0.5, -0.5, -0.5);
        tesselator_end();
    }
    if(render_type == BLOCK_RENDER_TYPE_TORCH) {
        tesselator_begin_quads();
        tesselator_normal(0, -1.0, 0);
        renderer_block_render_torch(renderer, block, -0.5, -0.5, -0.5, 0, 0);
        tesselator_end();
    }
}