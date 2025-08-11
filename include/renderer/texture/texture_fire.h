#pragma once

#include <renderer/texture/texture_animated.h>

texture_animated_t texture_fire_create(int type);
void texture_fire_tick(texture_animated_t *texture);