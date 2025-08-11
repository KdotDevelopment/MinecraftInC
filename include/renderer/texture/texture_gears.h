#pragma once

#include <renderer/texture/texture_animated.h>

texture_animated_t texture_gears_create(int rotation);
void texture_gears_tick(texture_animated_t *texture);