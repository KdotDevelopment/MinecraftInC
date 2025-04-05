#include <renderer/texture/texture_animated.h>

texture_animated_t texture_animated_create(int texture_id) {
    texture_animated_t texture = { 0 };
    texture.texture_id = texture_id;
    texture.tick = texture_animated_tick;

    return texture;
}

void texture_animated_tick(texture_animated_t *texture) {
    return;
}