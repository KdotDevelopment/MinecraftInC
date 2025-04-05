#include <renderer/textures.h>

#include <util/array_list.h>

#include <assets/clouds.h>
#include <assets/default.h>
#include <assets/dirt.h>
#include <assets/particles.h>
#include <assets/rain.h>
#include <assets/rock.h>
#include <assets/terrain.h>
#include <assets/water.h>
#include <assets/char.h>
#include <assets/gui/gui.h>
#include <assets/gui/icons.h>
#include <assets/item/arrows.h>
#include <assets/mob/creeper.h>
#include <assets/mob/pig.h>
#include <assets/mob/sheep_fur.h>
#include <assets/mob/sheep.h>
#include <assets/mob/skeleton.h>
#include <assets/mob/spider.h>
#include <assets/mob/zombie.h>
#include <assets/armor/chain.h>
#include <assets/armor/plate.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

textures_t textures_create(game_settings_t *settings) {
    textures_t textures = { 0 };
    textures.settings = settings;
    textures.textures = array_list_create(sizeof(uint32_t));
    textures.texture_buffer = malloc(512 * 512);
    textures.texture_names = array_list_create(sizeof(char *));
    textures.animations = array_list_create(sizeof(texture_animated_t *));

    return textures;
}

int textures_load(textures_t *textures, const char *resource) {
    uint8_t loaded = 0;
    int index = -1;

    for(int i = 0; i < array_list_length(textures->texture_names); i++) {
        if(strcmp(textures->texture_names[i], resource) == 0) {
            loaded = 1;
            index = i;
        }   
    }
    if(loaded) return textures->textures[index];

    int width = 0;
    int height = 0;
    uint8_t *p = NULL;

    if(strcmp(resource, "clouds.png") == 0) { p = (uint8_t *)asset_clouds_rgba; width = asset_clouds_width; height = asset_clouds_height; }
	if(strcmp(resource, "default.png") == 0) { p = (uint8_t *)asset_default_rgba; width = asset_default_width; height = asset_default_height; }
	if(strcmp(resource, "dirt.png") == 0) { p = (uint8_t *)asset_dirt_rgba; width = asset_dirt_width; height = asset_dirt_height; }
	if(strcmp(resource, "particles.png") == 0) { p = (uint8_t *)asset_particles_rgba; width = asset_particles_width; height = asset_particles_height; }
	if(strcmp(resource, "rain.png") == 0) { p = (uint8_t *)asset_rain_rgba; width = asset_rain_width; height = asset_rain_height; }
	if(strcmp(resource, "rock.png") == 0) { p = (uint8_t *)asset_rock_rgba; width = asset_rock_width; height = asset_rock_height; }
	if(strcmp(resource, "terrain.png") == 0) { p = (uint8_t *)asset_terrain_rgba; width = asset_terrain_width; height = asset_terrain_height; }
    if(strcmp(resource, "char.png") == 0) { p = (uint8_t *)asset_char_rgba; width = asset_char_width; height = asset_char_height; }
	if(strcmp(resource, "water.png") == 0) { p = (uint8_t *)asset_water_rgba; width = asset_water_width; height = asset_water_height; }
	if(strcmp(resource, "gui/gui.png") == 0) { p = (uint8_t *)asset_gui_rgba; width = asset_gui_width; height = asset_gui_height; }
	if(strcmp(resource, "gui/icons.png") == 0) { p = (uint8_t *)asset_icons_rgba; width = asset_icons_width; height = asset_icons_height; }
    if(strcmp(resource, "mob/creeper.png") == 0) { p = (uint8_t *)asset_creeper_rgba; width = asset_creeper_width; height = asset_creeper_height; }
    if(strcmp(resource, "mob/pig.png") == 0) { p = (uint8_t *)asset_pig_rgba; width = asset_pig_width; height = asset_pig_height; }
    if(strcmp(resource, "mob/sheep_fur.png") == 0) { p = (uint8_t *)asset_sheep_fur_rgba; width = asset_sheep_fur_width; height = asset_sheep_fur_height; }
    if(strcmp(resource, "mob/sheep.png") == 0) { p = (uint8_t *)asset_sheep_rgba; width = asset_sheep_width; height = asset_sheep_height; }
    if(strcmp(resource, "mob/skeleton.png") == 0) { p = (uint8_t *)asset_skeleton_rgba; width = asset_skeleton_width; height = asset_skeleton_height; }
    if(strcmp(resource, "mob/spider.png") == 0) { p = (uint8_t *)asset_spider_rgba; width = asset_spider_width; height = asset_spider_height; }
    if(strcmp(resource, "mob/zombie.png") == 0) { p = (uint8_t *)asset_zombie_rgba; width = asset_zombie_width; height = asset_zombie_height; }
    if(strcmp(resource, "item/arrows.png") == 0) { p = (uint8_t *)asset_arrows_rgba; width = asset_arrows_width; height = asset_arrows_height; }
    if(strcmp(resource, "armor/chain.png") == 0) { p = (uint8_t *)asset_chain_rgba; width = asset_chain_width; height = asset_chain_height; }
    if(strcmp(resource, "armor/plate.png") == 0) { p = (uint8_t *)asset_plate_rgba; width = asset_plate_width; height = asset_plate_height; }

    if (p == NULL) { printf("Failed to load image %s.\n", resource); }

    uint8_t *pixels = malloc(width * height * sizeof(uint32_t)); //32 bit color
    if(textures->settings->anaglyph) {
        for (int i = 0; i < 4 * width * height; i += 4) {
			uint8_t color[] = { (p[i + 0] * 30 + p[i + 1] * 59 + p[i + 2] * 11) / 100, (p[i + 0] * 30 + p[i + 1] * 70) / 100, (p[i + 0] * 30 + p[i + 2] * 70) / 100, p[i + 3] };
			memcpy(pixels + i, color, sizeof(color));
		}
    }else {
        memcpy(pixels, p, 4 * width * height);
    }

    glGenTextures(1, &textures->id_buffer);
    glBindTexture(GL_TEXTURE_2D, textures->id_buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    free(pixels);

    textures->texture_names = array_list_push(textures->texture_names, &resource);
    textures->textures = array_list_push(textures->textures, &textures->id_buffer);
    return textures->id_buffer;
}

void textures_reload(textures_t *textures) {
	for(int i = 0; i < array_list_length(textures->textures); i++) glDeleteTextures(1, &textures->textures[i]);
	textures->textures = array_list_clear(textures->textures);
	textures->texture_names = array_list_clear(textures->texture_names);
}

void textures_register_animation(textures_t *textures, texture_animated_t *texture) {
	textures->animations = array_list_push(textures->animations, &texture);
	texture->tick(texture);
}

void textures_destroy(textures_t *textures) {
	array_list_free(textures->textures);
	array_list_free(textures->texture_names);
	array_list_free(textures->animations);
	free(textures->texture_buffer);
}